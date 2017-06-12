#pragma once
#include "Common\Common.h"
#include "Common\Objects.h"

#include <Windowsx.h>
#include <gdiplus.h>
#include <objidl.h>
#include <vector>

#include "Geometry\Polygon.h"
#include "Geometry\Net.h"
#include "RenderObject.h"

#define RENDER_POINT 0x0
#define RENDER_POLY  0x1
#define RENDER_LINE  0x2

// Defines an artificial object to render.
struct RenderObjects
{
	Gdiplus::Color color;
	Geometry::Polygon poly;
	DWORD type;

	bool operator ==(const RenderObjects &t)
	{
		return (t.type == type && t.poly == poly);
	}
};

//  Defines an actual object from WoW to display on the radar.
struct WoWObject
{
	// WoW object's base and type.
	DWORD base;
	DWORD type;

	// Click boundary of object.
	Geometry::Polygon bound;
	bool operator ==(const WoWObject &t)
	{
		return (t.base == base && t.type == type && t.bound == bound);
	}
};

struct RadarMouse
{
	Gdiplus::Point pt;
	bool handled;
};

struct RadarClick
{
	Gdiplus::Point pt;
	bool handled;
};

struct RadarDisplay
{
	bool name;
	bool unit;
	bool enemy;
	bool friendly;
	bool gameObject;
};

namespace Radar
{
	enum eDrawingType
	{
		Arrow,
		Cross,
		Dot
	};

	// Settings
	extern RadarDisplay displaySettings;
	extern std::vector<RenderObject *> RenderList;

	extern float zoom;
	extern bool created;
	extern HWND settingsWnd;
	extern uint refreshTime;
	// ** Add render list for outside of Radar namespace.
	// ** Ex: in main.cpp, Radar::AddRenderObject(waypoint, worldPos, drawType);
	//std::vector<ObjectHeader> RenderList;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	void MoveChild_Settings(HWND hWnd, int x, int y);

	WOWPOS TransformRadarToWorld(WOWPOS origin, Gdiplus::Point radar);

	Gdiplus::Point TransformWorldToRadar(WOWPOS origin, const Geometry::Vec2f &pos);
	Gdiplus::Point TransformWorldToRadar(WOWPOS origin, WOWPOS pos);

	BOOL ShouldRender(CObject *obj);

	VOID HandleRadarSelect(CUnit *unit);
	VOID HandleRenderWoWObject(HWND hWnd, HDC hdc, WoWObject *current);
	VOID DrawWoWObjects(HWND hWnd, HDC hdc);
	VOID EnumerateWoWObjects();

	VOID MarkName(std::string name);
	VOID Unmark(std::string name);
	VOID UnmarkAll();

	uint Add(RenderObject *r);

	VOID Remove(uint x);
	VOID Remove(RenderObject &r);
	VOID Remove(RenderObject *obj);

	// Navigation Nets

	uint AddWorldPoint(WOWPOS p, Gdiplus::Color color);
	uint AddPoint(Gdiplus::Point p, Gdiplus::Color color);
	VOID AddNet(Geometry::Net &net, Gdiplus::Color color);
	uint AddLine(WOWPOS from, WOWPOS to, Gdiplus::Color color);

	VOID Clear();
	VOID OnRenderLine(HWND hWnd, HDC hdc, RenderObject *current);

	VOID DrawRenderObjects(HWND hWnd, HDC hdc);
	VOID DrawNetPolygon(HWND hWnd, HDC hdc, Gdiplus::Color color, RenderObject *node);

	Gdiplus::Region *NetPolygonToRegion(Geometry::Polygon *poly);

	// Radar and NPC drawing.
	VOID DrawWoWObject(HWND hWnd, HDC hdc, Gdiplus::Point p, Gdiplus::Color color, float rot, BOOL fill, Radar::eDrawingType type);
	VOID DrawWoWObject(HWND hWnd, HDC hdc, Gdiplus::Point p, Gdiplus::Color color, float rot, BOOL fill);

	VOID DrawString(HWND hWnd, HDC hdc, Gdiplus::Point p, Gdiplus::Color color, WCHAR *text, int fontSize);
	VOID DrawPoint(HWND hWnd, HDC hdc, Gdiplus::Color color, Gdiplus::Point p, int diameter, bool fill);

	VOID HandlePaint(HWND hWnd);

	VOID OnCreate(HWND hWnd);
	VOID OnPaint(HWND hWnd, HDC hdc);
	VOID OnSize(HWND hWnd, int newW, int newH);
	VOID OnLeftClick(HWND hWnd, LPARAM lParam);
	VOID OnMouseMove(HWND hWnd, LPARAM lParam);
	VOID OnMouseWheel(HWND hWnd, WPARAM wParam);

	VOID Create();
};