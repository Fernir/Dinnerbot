#include "Drawing.h"

#include <algorithm>

#include "GraphicsObject.h"
#include "Geometry\Net.h"
#include "Geometry\Polygon.h"

#include "Settings.h"
#include "NetEditor.h"

#include "Game\Game.h"

using namespace Radar;
using namespace Gdiplus;
using namespace Geometry;

#define IDC_RADAR_BUTTON_SETTINGS 1139

#define IDT_DRAW 0x102
#define IDT_ZONE 0x103

#define ZOOM_MAX 32
#define ZOOM_MIN 0

// Radar Render Settings
RadarDisplay Radar::displaySettings;

// Show target only on Radar option (for rogues, or something).

// Refresh time of radar in miliseconds
uint Radar::refreshTime = 60;
bool Radar::created = false;

HWND Radar::settingsWnd = NULL;

HWND hwndButton = NULL;

std::vector<std::string> marked;

// Radar object states
RadarClick onClick;
RadarMouse onMouseMove;

bool handleMouseMove = false;

float Radar::zoom = 2;

std::vector<WoWObject>  WoWObjects;
std::vector<RenderObject *> Radar::RenderList;

HBRUSH backgroundBrush;

POINT Pt2WPt(Gdiplus::Point p)
{
	POINT c;

	c.x = p.X;
	c.y = p.Y;
	return c;
}

Point GetWindowCenter(HWND hWnd)
{
	Point center;
	RECT rect;
	int width, height;

	GetWindowRect(hWnd, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	center.X = width / 2;
	center.Y = height / 2;
	return center;
}

Point OffsetToCenter(HWND hWnd, Point pt)
{
	Point p;
	Point c = GetWindowCenter(hWnd);
	p = pt + c;

	return pt;
}

Point Radar::TransformWorldToRadar(WOWPOS origin, const Vec2f &pos)
{
	WOWPOS conv;
	conv.X = pos.x;
	conv.Y = pos.y;

	return Radar::TransformWorldToRadar(origin, conv);
}

Point Radar::TransformWorldToRadar(WOWPOS origin, WOWPOS pos)
{
	// Translate pos to the coordinate origin.
	Vec2f d(pos.X - origin.X, pos.Y - origin.Y);
	Point result;

	float dist;
	float angle;
	
	// Calculate distance between origin and pos.
	dist  = d.norm();

	// Calculate the angle of the translated d vector.
	angle = atan2(d.x, d.y) + PI;

	// Using the angle and distance, we have enough information to transform 
	// from the World of Warcraft inner product space to our own (metric and angle).
	// Note that this is indeed an isometric isomorphism of vector spaces as we preserve angle and distance.
	result.X = zoom * dist * cos(angle);
	result.Y = zoom * dist * sin(angle);
	return result;
}

// Could use some work, not very accurate.
// Assuming we're based on the origin of the radar.
WOWPOS Radar::TransformRadarToWorld(WOWPOS origin, Point radar)
{
	WOWPOS result;
	float  angle = 0;
	float  dist = 0;

	angle = atan2(radar.X / (zoom * cos(angle)), radar.Y / (zoom * cos(angle)));
	dist = fabs(radar.X / (zoom * cos(angle)));
	vlog("Dist: %f", dist);

	// WoW's angles are -PI off the normal compass-based map.
	angle -= PI;

	result.X = origin.X + dist * cos(angle);
	result.Y = origin.Y + dist * sin(angle);
	result.Z = origin.Z;
	return result;
}

VOID Radar::DrawString(HWND hWnd, HDC hdc, Point p, Color color, WCHAR *text, int fontSize)
{
	PointF pt;

	SolidBrush  brush(color);
	Graphics	Gx(hdc);
	Font		font(L"Arial", fontSize, UnitWorld);

	pt.X = p.X;
	pt.Y = p.Y;
	Gx.DrawString(text, lstrlenW(text), &font, pt, &brush);
}

VOID Radar::DrawWoWObject(HWND hWnd, HDC hdc, Point p, Color color, float rot, BOOL fill)
{
	Radar::DrawWoWObject(hWnd, hdc, p, color, rot, fill, eDrawingType::Arrow);
}

VOID Radar::DrawWoWObject(HWND hWnd, HDC hdc, Point p, Color color, float rot, BOOL fill, Radar::eDrawingType type)
{
	Point	center;

	SolidBrush  brush(color);
	Graphics	graphics(hdc);
	Pen			pen(color);

	GraphicsObject object(&graphics, &pen, &brush);

	center = GetWindowCenter(hWnd);
	center.X += p.X;
	center.Y += p.Y;

	switch (type)
	{
		case Radar::Arrow:
			object.DrawArrow(center, rot, 7, 40, fill);
			break;

		case Radar::Cross:
			object.DrawCross(center, 15);
			break;
	}
}

Region *CreatePointRegion(POINT p, int w, int h)
{
	POINT   points[4] = {p, p, p, p};
	Region *bound;
	HRGN    poly;

	// Create rectangle around point.
	points[0].x -= w / 2;
	points[0].y += h / 2;

	points[1].x += w / 2;
	points[1].y += h / 2;

	points[2].x += w / 2;
	points[2].y -= h / 2;

	points[3].x -= w / 2;
	points[3].y -= h / 2;

	poly = CreatePolygonRgn(points, 4, ALTERNATE);
	bound = Region::FromHRGN(poly);

	return bound;
}

BOOL CreatePointPolygon(POINT p, int w, int h, Geometry::Polygon *reg)
{
	reg->vertices.push_back(Vec2f(p.x - w/2, p.y + h/2));
	reg->vertices.push_back(Vec2f(p.x + w/2, p.y + h/2));
	reg->vertices.push_back(Vec2f(p.x + w/2, p.y - h/2));
	reg->vertices.push_back(Vec2f(p.x - w/2, p.y - h/2));

	return true;
}

VOID Radar::HandleRadarSelect(CUnit *unit)
{
	if (GetCurrentTargetGUID() != unit->guid())
	{
		unit->target();
	}
}

VOID Radar::MarkName(std::string name)
{
	marked.push_back(name);
}

VOID Radar::UnmarkAll()
{
	marked.clear();
}

VOID Radar::Unmark(std::string name)
{
	marked.erase(std::remove(marked.begin(), marked.end(), name));
}

Gdiplus::Color GetRenderColor(CObject *object)
{
	CUnit *unit = NULL;
	std::string name;

	object->getName();

	name = object->name();
	//toLower(name);

	if (vectorHas<std::string>(name, marked))
	{
		return Color::Blue;
	}

	switch (object->type())
	{
		case OT_UNIT:
			return Color::Yellow;

		case OT_GAMEOBJ:
			return Color::DeepPink;

		case OT_PLAYER:
			// Get unitfield base + faction.
			unit = (CUnit *)(object);
			unit->updateFieldBase();
			unit->updateFaction();

			if (unit->isFriendly(&LocalPlayer)) 
				return Color::SpringGreen;
			else 
				return Color::Red;
	}

	return Color::White;
}

VOID Radar::HandleRenderWoWObject(HWND hWnd, HDC hdc, WoWObject *current)
{
	CUnit object;

	RECT client;
	Color color = Color::DeepPink;

	WCHAR name[64];

	Point center;
	Point p;

	// Create square boundary around each radar object of given width.
	int boundWidth = 20;

	object.setBase(current->base);
	object.update(LocationInfo);

	// Transform wow coordinates into radar coordinates.
	p = TransformWorldToRadar(LocalPlayer.pos(), object.pos());
			
	GetWindowRect(hWnd, &client);
	center = GetWindowCenter(hWnd);
	p = center + p;

	/*vlog("p.X: %d, p.Y: %d", p.X, p.Y);
	vlog("left: %d, right: %d", client.left, client.right);
	vlog("top: %d, bottom: %d", client.top, client.bottom);*/

	// Only render if the point is inside the window.
	if (/*p.X < client.left   ||*/ p.X > client.right ||
		p.Y > client.bottom /*|| p.Y < client.top*/)
		return;

	// Create boundary around p.
	CreatePointPolygon(Pt2WPt(p), boundWidth, boundWidth, &current->bound);
	
	// Check to see if the client's click is within the bound of the current object.
	if (!onClick.handled)
	{
		if (current->bound.contains(Pt2WPt(onClick.pt)))
		{
			HandleRadarSelect(&object);
			onClick.handled = true;
		}
	}

	// Determine which color to render the object with.
	color = GetRenderColor(&object);
	p = p - center;
	
	// Draw game objects filled in and without an arrow.
	if (object.isGameObject())
	{			
		DrawWoWObject(hWnd, hdc, p, color, 0, true);
	}

	else if (object.isUnit() || object.isPlayer())
	{
		object.updateFieldBase();
		object.updateHealth();

		// If the object is dead, draw a cross
		if (object.isDead())
			DrawWoWObject(hWnd, hdc, p, color, object.pos().Rotation, false, Radar::Cross);

		// If the local player is targetting this object, fill in its circle.
		else if (LocalPlayer.hasTarget(object))
			DrawWoWObject(hWnd, hdc, p, color, object.pos().Rotation, true, Radar::Arrow);

		// Otherwise draw an unfilled circle.
		else
			DrawWoWObject(hWnd, hdc, p, color, object.pos().Rotation, false, Radar::Arrow);
	}

	else
		DrawWoWObject(hWnd, hdc, p, color, object.pos().Rotation, false);
		
	// Draw object names.
	if (Radar::displaySettings.name)
	{
		object.getName();

		// Convert ASCII to wide byte string.
		mbstowcs(name, object.name(), 64);
		p = p + center;

		// Offset string for readability.
		p.X -= 3 * strlen(object.name());
		p.Y += 6;

		// Render object name.
		DrawString(hWnd, hdc, p, color, name, 8);
	}
}

VOID Radar::DrawWoWObjects(HWND hWnd, HDC hdc)
{
	WoWObject *current;

	// Iterate through list of this instance's render list.
	for (int x = 0; x < WoWObjects.size(); x++)
	{
		current = &WoWObjects[x];
		HandleRenderWoWObject(hWnd, hdc, current);
	}

	// Notify that we've  handled mouse clicks and movements.
	// Clear the render list so we can populate it for the next instance.
	WoWObjects.clear();

	//onClick.handled = false;
	onMouseMove.handled = false;
}

BOOL Radar::ShouldRender(CObject *obj)
{
	CUnit *unit = NULL;

	switch (obj->type())
	{
	case OT_UNIT:
		return Radar::displaySettings.unit;

	case OT_GAMEOBJ:
		return Radar::displaySettings.gameObject;

	case OT_PLAYER:
		// Downcast object to unit class since we know it is at least a unit.
		unit = (CUnit *)(obj);
		unit->updateFieldBase();
		unit->updateFaction();

		if (unit->isFriendly(&LocalPlayer)) 
			return Radar::displaySettings.friendly;
		else
			return Radar::displaySettings.enemy;
	}

	return false;
}

VOID Radar::EnumerateWoWObjects()
{
	CObject         current;
	CObject		    next;
	WoWObject		obj;

	// Get the start of the object manager.
	rpm(GetCurrentManager() + Offsets::eObjectManager::FirstEntry, &current.Obj.BaseAddress, sizeof(current.Obj.BaseAddress));

	current.update();
	while (current.isValid())
	{
		if (ShouldRender(&current) && !current.isLocalPlayer())
		{
			obj.base = current.base();
			obj.type = current.type();
			WoWObjects.push_back(obj);
		}

		// Move to the next object in the object manager.
		next.setBase(current.nextPtr());
		next.update();

		if (next.base() == current.base())
			break;
		else
			current.set(next.object());
	}
}

VOID Radar::DrawPoint(HWND hWnd, HDC hdc, Color color, Point p, int diameter, bool fill)
{

	SolidBrush  brush(color);
	Graphics	graphics(hdc);
	Pen			pen(color);

	Point			center;
	GraphicsObject  circle(&graphics, &pen, &brush);

	center = GetWindowCenter(hWnd);
	center.X += p.X;
	center.Y += p.Y;

	circle.width = diameter;
	circle.height = diameter;
	circle.source.x = center.X - (circle.width / 2);
	circle.source.y = center.Y - (circle.height / 2);

	circle.DrawEllipse();
	if (fill) circle.FillEllipse();
}

Point TPt2Pt(Vec2f v)
{
	Point p;

	p.X = v.x;
	p.Y = v.x;
	return p;
}

VOID Radar::DrawNetPolygon(HWND hWnd, HDC hdc, Color color, RenderObject *obj)
{
	Point			p;
	Point			c;
	POINT			temp;

	GraphicsPath hull;
	
	SolidBrush  brush(Color(85, color.GetR(), color.GetG(), color.GetB()));

	Graphics	graphics(hdc);
	Pen			circlePen(color);
	Pen			hullPen(color);

	GraphicsObject circle(&graphics, &circlePen, &brush);

	if (obj == NULL) return;

	c = GetWindowCenter(hWnd);

	for (int x = 0; x < obj->size(); x++)
	{
		// Retrieve world vector.
		p = TransformWorldToRadar(LocalPlayer.pos(), obj->worldBoundary()[x]);

		temp.x = c.X + p.X;
		temp.y = c.Y + p.Y;

		// Convert world vector to render vector.
		obj->addRender(temp);
		DrawPoint(hWnd, hdc, Color::White, p, 5, true);
	}

	//vlog("Area: %f", obj->poly.area());
	circle.bound = NetPolygonToRegion(&obj->polygon());

	if (circle.bound == NULL) return;
	circle.FillRegion();

	circle.pen->SetColor(Color::Red);
	circle.DrawHull(&obj->polygon());

	// Clear render vertices so we can update its position on next draw.
	// Otherwise the polygon is fixed and doesn't move when we move in-game.
	obj->clearRender();
}

VOID Radar::OnRenderLine(HWND hWnd, HDC hdc, RenderObject *current)
{
	Point  c;
	Point  to;
	Point  source;
	WOWPOS pos;

	SolidBrush  brush(current->color());
	Graphics	graphics(hdc);
	Pen			pen(current->color());

	GraphicsObject line(&graphics, &pen, &brush);

	c = GetWindowCenter(hWnd);

	pos.X = current->worldBoundary()[0].x;
	pos.Y = current->worldBoundary()[0].y;
	to = TransformWorldToRadar(LocalPlayer.pos(), pos);
	to = to + c;
	
	pos.X = current->worldBoundary()[1].x;
	pos.Y = current->worldBoundary()[1].y;
	source = TransformWorldToRadar(LocalPlayer.pos(), pos);
	source = source + c;

	line.source.x = source.X;
	line.source.y = source.Y;
	line.DrawLine(&to); 
}

VOID Radar::DrawRenderObjects(HWND hWnd, HDC hdc)
{
	RenderObject *current = NULL;
	Point p;

	for (int x = 0; x < RenderList.size(); x++)
	{
		current = RenderList[x];

		if (current == NULL)
		{
			Radar::Remove(x);
			continue;
		}

		switch (current->type())
		{
			case RenderObject::eType::Point:
				p = TransformWorldToRadar(LocalPlayer.pos(), current->worldBoundary()[0]);
				DrawPoint(hWnd, hdc, current->color(), p, 7, true);
				break;

			case RenderObject::eType::Polygon:
				DrawNetPolygon(hWnd, hdc, Color::DeepSkyBlue, current);
				break;

			case RenderObject::eType::Line:
				OnRenderLine(hWnd, hdc, current);
				break;
		}
	}	
}

Region *Radar::NetPolygonToRegion(Geometry::Polygon *poly)
{
	POINT *p;
	HRGN reg;

	Gdiplus::Region *result = NULL;

	if (poly == NULL) return NULL;
	p = new POINT[poly->order()];

	for (int x = 0; x < poly->order(); x++)
	{
		p[x] = poly->transformed[x];
	}

	reg = CreatePolygonRgn(p, poly->order(), ALTERNATE);
	delete[] p;

	if (reg == NULL) return NULL;
	result = Region::FromHRGN(reg);
	DeleteObject(reg);
	return result;
}

uint Radar::AddLine(WOWPOS from, WOWPOS to, Color color)
{
	if (!Radar::created) return 0;
	RenderObject *obj = new RenderObject(RenderObject::eType::Line);

	obj->setColor(color);
	obj->addWorld(from);
	obj->addWorld(to);

	RenderList.push_back(obj);
	return RenderList.size() - 1;
}

uint Radar::AddWorldPoint(WOWPOS p, Color color)
{
	if (!Radar::created) return 0;
	RenderObject *obj = new RenderObject(RenderObject::eType::Point);
	Point pt(p.X, p.Y);

	obj->setColor(color);
	obj->addWorld(p);

	RenderList.push_back(obj);
	return RenderList.size() - 1;
}

uint Radar::AddPoint(Point p, Color color)
{
	if (!Radar::created) return 0;

	RenderObject *obj = new RenderObject(RenderObject::eType::Point);
	POINT pt = Pt2WPt(p);

	obj->setColor(color);
	obj->addRender(pt);

	RenderList.push_back(obj);
	return RenderList.size() - 1;
}

VOID Radar::AddNet(Geometry::Net &net, Color color)
{
	RenderObject *obj = NULL;  
	int x;

	if (!Radar::created) return;
	//obj.poly.vertices.reserve(net.order() * 3);

	for (x = 0; x < net.order(); x++)
	{
		obj = new RenderObject(RenderObject::eType::Polygon);
		obj->clear();
		obj->setColor(color);
		obj->addWorld(net.getTriangle(x));

		RenderList.push_back(obj);
	}
}

uint Radar::Add(RenderObject *r)
{
	if (!Radar::created) return 0;
	RenderList.push_back(r);
	return RenderList.size() - 1;
}

VOID Radar::Clear()
{
	RenderList.clear();
}

VOID Radar::Remove(uint x)
{
	if (x >= RenderList.size()) return;
	
	RenderList.erase(RenderList.begin() + x);
}

VOID Radar::Remove(RenderObject *obj)
{
	for (int x = 0; x < RenderList.size(); x++)
	{
		if (RenderList[x] == obj)
		{
			RenderList.erase(RenderList.begin() + x);
		}
	}
}

VOID Radar::Remove(RenderObject &obj)
{
	std::vector<RenderObject *> *r = &Radar::RenderList;
	std::vector<RenderObject *>::iterator it;

	//it = std::remove(r->begin(), r->end(), obj);
	//r->erase(it);
}

// Draw function.
VOID Radar::OnPaint(HWND hWnd, HDC hdc)
{	
	Point o(0, 0);

	// Get local player information.
	LocalPlayer.update(LocationInfo | UnitFieldInfo);

	LocalPlayer.updateFieldBase();
	LocalPlayer.updateFaction();

	// Determine which objects in the object manager to render.
	EnumerateWoWObjects();
	
	// Draw every RadarObject.
	DrawRenderObjects(hWnd, hdc);

	// Draw local player circle and arrow.
	DrawWoWObject(hWnd, hdc, o, Color::Magenta, LocalPlayer.pos().Rotation, true);

	// Draw objects to be rendered.
	DrawWoWObjects(hWnd, hdc);
}

// Double buffering enabled.
VOID Radar::HandlePaint(HWND hWnd)
{
	HDC			 hdcMem;
	HBITMAP		 hbmMem;
	HGDIOBJ      hOld;
	HDC          hdc;
	PAINTSTRUCT  ps;

	RECT rect;
	int width, height;

	GetWindowRect(hWnd, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	
	rect.top = 0;
	rect.left = 0;
	rect.right = width;
	rect.bottom = height;
 
	hdc = BeginPaint(hWnd, &ps);
 
	// Create an off-screen DC for double-buffering
	hdcMem = CreateCompatibleDC(hdc);
	hbmMem = CreateCompatibleBitmap(hdc, width, height);
	hOld   = SelectObject(hdcMem, hbmMem);

	// Paint section
	FillRect(hdcMem, &rect, backgroundBrush);
	OnPaint(hWnd, hdcMem);

	// Transfer the off-screen DC to the screen
	BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);
 
	// Free-up the off-screen DC
	SelectObject(hdcMem, hOld);
	DeleteObject(hbmMem);
	DeleteDC    (hdcMem);
 
	EndPaint(hWnd, &ps);
}

// ** Target clicked object
// ** On Hover => display name.
// ** Mouse listener for GDI objects (shapes).
// ** Each render object will have a bounding region so that we can check every (possibly nearby, using distance) object
// ** to see if the clicked point is contained in the boundary. Use Region::IsVisible(mouse point, etc).
// Done.
VOID Radar::OnLeftClick(HWND hWnd, LPARAM lParam)
{
	// Flag onClick structure so that any outstanding radar function can handle the mouse click.
	onClick.pt.X = GET_X_LPARAM(lParam);
	onClick.pt.Y = GET_Y_LPARAM(lParam);
	onClick.handled = false;
} 

VOID Radar::OnMouseMove(HWND hWnd, LPARAM lParam)
{
	if (!handleMouseMove) return;

	// Get the mouse pointer's coordinates.
	onMouseMove.pt.X = GET_X_LPARAM(lParam);
	onMouseMove.pt.Y = GET_Y_LPARAM(lParam);
	onMouseMove.handled = false;
}

// Handle radar zoom.
VOID Radar::OnMouseWheel(HWND hWnd, WPARAM wParam)
{
	// Get wheel zoom parameter.
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	// If the new mouse scroll parameter violates the zoom maximum or minimum,
	// then ignore this data.
	if (zDelta > 0)
		if (Radar::zoom + (float)(zDelta) / 1000 >= ZOOM_MAX) 
			return;
	if (zDelta < 0)
		if (Radar::zoom + (float)(zDelta) / 1000 <= ZOOM_MIN) 
			return;

	// Otherwise, zoom the radar in.
	Radar::zoom += (float)(zDelta) / 1000;
}

VOID Radar::MoveChild_Settings(HWND hWnd, int x, int y)
{
	RECT parentR, childR;

	int width, height;

	if (!Radar::settingsWnd) return;

	// Get dimensions of the radar and settings windows.
	GetWindowRect(hWnd, &parentR);
	GetWindowRect(settingsWnd, &childR);

	// Width and height of settings window.
	width = childR.right - childR.left;
	height = childR.bottom - childR.top;
	
	// Move settings window to the right of the radar.
	MoveWindow(settingsWnd, x + (parentR.right - parentR.left), parentR.top, width, height, false); 
}

VOID Radar::OnSize(HWND hWnd, int newWidth, int newHeight)
{
	RECT rect;
	RECT child;

	int width, height;

	if (!Radar::settingsWnd) return;

	// Get dimensions of the radar and settings windows.
	GetWindowRect(hWnd, &rect);
	GetWindowRect(settingsWnd, &child);

	// Width and height of settings window.
	width = child.right - child.left;
	height = child.bottom - child.top;

	// Initialize first instance of lastR.
	static int lastWidth = rect.right - rect.left;

	// Move settings window to the right of the newly rezised radar.
	MoveWindow(settingsWnd, child.left + (newWidth - lastWidth), rect.top, width, height, false);
	lastWidth = newWidth;
}

VOID UpdateZone(HWND hWnd)
{
	char buffer[256];

	static DWORD zonePtr = NULL;
	static DWORD subZonePtr = NULL;

	std::string zone;
	std::string subZone;

	// Set radar window title.
	// Compare zone text pointers for any differences.
	// NOTE: This is more efficient than comparing strings.
	if (WoW::GetZoneTextPtr() != zonePtr || WoW::GetSubZoneTextPtr() != subZonePtr)
	{
		// Get zone text.
		zone = WoW::GetZoneText();
		subZone = WoW::GetSubZoneText();

		if (subZone.size() == 0)
			sprintf_s(buffer, "%s", zone.c_str());
		else
			sprintf_s(buffer, "%s - %s", zone.c_str(), subZone.c_str());
		
		SetWindowText(hWnd, buffer);

		// Update last zone text pointers for future comparisons.
		zonePtr    = WoW::GetZoneTextPtr();
		subZonePtr = WoW::GetSubZoneTextPtr();
	}
}

VOID SettingsButton(HWND hWnd)
{

}

VOID Radar::OnCreate(HWND hWnd)
{
	// Render refresh rate.
	SetTimer(hWnd, IDT_DRAW, refreshTime, 0);

	// Window text zone refresh rate.
	SetTimer(hWnd, IDT_ZONE, 2000, 0);

	// On create update zone.
	UpdateZone(hWnd);

	// Initialize object list and signal our creation.
	WoWObjects.reserve(20);
	created = true;

	// Radar display settings.
	displaySettings.name = false;
	displaySettings.unit = false;
	displaySettings.enemy = true;
	displaySettings.friendly = true;
	displaySettings.gameObject = false;

	if (NetEditor::IsLoaded())
		Radar::AddNet(NetEditor::net, Gdiplus::Color::White);
	/*HBITMAP hBitmap;
	Gdiplus::Bitmap b(L"test.ico");
	b.GetHBITMAP(NULL, &hBitmap);

	SendMessage(GetDlgItem(hDlg, IDC_BUTTON1), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);*/

	// Initialize render variables and brush.
	RenderList.reserve(100);
	backgroundBrush = CreateSolidBrush(RGB(85, 85, 85));

	// Set radar to be topmost.	
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

LRESULT CALLBACK Radar::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HICON hIcon;
	HBRUSH hBrushBtn;

	switch (wParam)
	{
		// If we are asked to draw, then invalidate the whole window for redrawing.
		// NOTE: Selectively invalidate certain regions that need to be redrawn, not the whole window.
		case IDT_DRAW:
			InvalidateRect(hWnd, NULL, FALSE);
			break;

		// Handle update zone text.
		case IDT_ZONE:
			UpdateZone(hWnd);
			break;
	}

	switch(message)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_RADAR_BUTTON_SETTINGS:
			Radar::ToggleSettings();
			break;
		}

		break;

	case WM_CREATE:
		/*hwndButton = CreateWindow("Button", " ", WS_CLIPCHILDREN | WS_VISIBLE | WS_CHILD, 100, 100, 40, 30, hWnd, (HMENU)IDC_RADAR_BUTTON_SETTINGS, GetModuleHandle(NULL), 0);
		hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_SETTINGS));
		SendMessage(hwndButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);
		SetLayeredWindowAttributes(hwndButton, 0, (255 * 70) / 100, LWA_ALPHA);*/
		OnCreate(hWnd);
		return 0;

	case WM_MOUSEWHEEL:
		OnMouseWheel(hWnd, wParam);
		break;

	case WM_MOUSEMOVE:
		OnMouseMove(hWnd, lParam);
		break;

	case WM_LBUTTONUP:
		OnLeftClick(hWnd, lParam);
		break;

	case WM_SIZE:
		OnSize(hWnd, (int)(short) LOWORD(lParam), (int)(short) HIWORD(lParam));
		break;

	case WM_MOVE:
		MoveChild_Settings(hWnd, (int)(short) LOWORD(lParam), (int)(short) HIWORD(lParam));
		break;

	case WM_PAINT:
		HandlePaint(hWnd);
		return 0;

	case WM_ERASEBKGND:
		return TRUE;

	case WM_DESTROY:
		SendMessage(settingsWnd, WM_CLOSE, NULL, NULL);  
		ReleaseDC(hWnd, GetDC(hWnd));
		PostQuitMessage(0);
		WoWObjects.clear();
		RenderList.clear();
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

// Create window
void Radar::Create()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	HWND                hWnd;
	MSG                 msg;
	WNDCLASS            wndClass;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
   
	if (created) return;

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	
	// Describe the window style and properties.
	wndClass.style          = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc    = Radar::WndProc;
	wndClass.cbClsExtra     = 0;
	wndClass.cbWndExtra     = 0;
	wndClass.hInstance      = hInstance;
	wndClass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName   = NULL;
	wndClass.lpszClassName  = TEXT("Radar");
   
	RegisterClass(&wndClass); 
	
	hWnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
		"Radar",					// window class name
		"Radar",					// window caption
		WS_OVERLAPPEDWINDOW,		// window style
		CW_USEDEFAULT,				// initial x position
		CW_USEDEFAULT,				// initial y position
		300,						// initial x size
		400,						// initial y size
		NULL,						// parent window handle
		NULL,						// window menu handle
		hInstance,					// program instance handle
		NULL);						// creation parameters

	/*HWND hWndButtons = CreateWindowEx(WS_EX_TRANSPARENT,
		"BUTTON",
		"OK",
		WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		50,
		220,
		100,
		24,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	HWND hWndButton = CreateWindowEx(
        0,
        "BUTTON", //ascii
        "Button text",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        10, 
        145,
        50,
        50,
        hWnd,
        NULL,
        //GetModuleHandle(NULL)
        (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
        NULL);


	SendMessage((HWND) hWndButton,
        (UINT) BM_SETIMAGE,
        (WPARAM) IMAGE_BITMAP,
        (LPARAM) LoadBitmap(hInstance, MAKEINTRESOURCE(IDI_SETTINGS)));*/

	// Create settings window as a child of hWnd.
	CreateSettings(hWnd);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
   
	// Message pump
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
   
	GdiplusShutdown(gdiplusToken);
	created = false;
} 
