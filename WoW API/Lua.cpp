#include "Lua.h"
#include "Common\Common.h"
#include "Memory\Endscene.h"
#include "Chat.h"
#include "Utilities\Utilities.h"

#include <time.h>

using namespace Memory;

// Run script from lua editor
VOID Lua::RunScript(HWND hWnd)
{
	CHAR script[5050];
	
	GetDlgItemText(hWnd, IDC_LUA_EDIT, script, 5000);
	
	if (Lua::Run(script))
	{
		vlog("Ran script: \r\n%s", script);
	}
	else
	{
		vlog("Unable to execute Lua");
	}
}

// Lua FSE calls subroutine in WoW.exe
DWORD Lua::FrameScriptExecute(CONST CHAR *script)
{
	DWORD buffer			= NULL;
	DWORD totalSize			= NULL;
	DWORD scriptLocation	= NULL;
	DWORD executeAddress = Offsets::FrameScript_Execute;


	CHAR szBuffer[FUNCTIONSPACE_MAX];
	
	if (!Endscene.CanExecute() || !Endscene.IsHandlerStarted()) return false;

	BYTE code[] = {
		/* Update TLS */
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager	
			
		/** Call function segment. **/
			0xBF, 0x00, 0x00, 0x00, 0x00,		// mov edi, executeAddress
			0x6A, 0x00,							// push 00
			0x68, 0x00, 0x00, 0x00, 0x00,		// push script
			0x68, 0x00, 0x00, 0x00, 0x00,		// push script
			0xFF, 0xD7,							// call edi
			0x83, 0xC4, 0x0C,					// add esp, 0C
			0xC3								// retn
	};

	if (strlen(script) + 1 + sizeof(code) > FUNCTIONSPACE_MAX) return false;

	Stub codeStub(Endscene.getFunctionSpace(), code, sizeof(code));

	strncpy(szBuffer, script, strlen(script) + 1);
	
	totalSize = sizeof(code) + strlen(script) + 1;
	scriptLocation = codeStub.getAddress() + sizeof(code);

	Stub scriptStub(scriptLocation, (byte *)szBuffer, strlen(szBuffer) + 1);

	memcpy(&code[24], &executeAddress, sizeof(executeAddress));
	memcpy(&code[31], &scriptLocation, sizeof(scriptLocation));
	memcpy(&code[36], &scriptLocation, sizeof(scriptLocation));

	Endscene.ClearFunctionSpace();

	// Inject code stub and script.
	codeStub.inject();
	scriptStub.inject();

	return Endscene.Execute(codeStub);
}

DWORD Lua::AddChatMessage(const char *text, float r, float g, float b)
{
	DWORD result = 0;

	if (!Endscene.CanExecute()) return 0;

	char *printScript = new char[strlen(text) + 70];

	sprintf(printScript, "DEFAULT_CHAT_FRAME:AddMessage(\"[D] %s\", %0.2f, %0.2f, %0.2f)", text, r, g, b);
	result = Lua::DoString(printScript);
	Lua::vDo("dank = \"%s\"", text);

	delete printScript;
	return result;
}

// Ensures Endscene is hooked before running script
DWORD Lua::Run(CONST CHAR *script)
{
	Endscene.Start();
	return Lua::DoString(script);
}

// Calls FSE
DWORD Lua::DoString(CONST CHAR *script)
{
	return FrameScriptExecute(script);
}

// Calls FSE
DWORD Lua::DoString(std::string script)
{
	return FrameScriptExecute(script.c_str());
}

std::string Lua::GenerateLocalVariable(uint length)
{
	std::string variable = "";
	srand(time(NULL));

	for (int x = 0; x < length; x++)
		variable += 'A' + rand() % 24;

	return variable;
}

DWORD Lua::sendWhisper(std::string message, std::string name)
{
	return Lua::vDo("SendChatMessage(\"%s\", \"WHISPER\", nil, \"%s\")", message.c_str(), name.c_str());
}

// Variable Lua::DoString.
DWORD Lua::vDo(CONST CHAR *format, ...)
{
	va_list		args;
	CHAR buffer[4098];

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	return Lua::DoString(buffer);
}

std::string Lua::GetText(CONST CHAR *variable)
{
	char buffer[FUNCTIONSPACE_MAX];
	std::string result;

	Lua::GetText(variable, buffer);
	result = buffer;
	return result;
}

std::string Lua::GetText(std::string &var)
{
	char buffer[FUNCTIONSPACE_MAX];
	std::string result;

	Lua::GetText(var.c_str(), buffer);
	result = buffer;
	return result;
}

// TODO: max size?
DWORD Lua::GetText(CONST CHAR *variable, CHAR *result)
{
	DWORD time = NULL;
	DWORD valueLocation = NULL;
	DWORD executeAddress = Offsets::FrameScript_GetText;

	CHAR szBuffer[FUNCTIONSPACE_MAX];

	//if (!IsIngame()) return FALSE; 
	//if (IsWardenLoaded()) return false;
	if (!Endscene.CanExecute()) return false;

	BYTE code[] = {
		/* Update TLS */
			0x8B, 0x15, 0xE0, 0x9C, 0xC7, 0x00, // MOV EDX, DWORD PTR DS:[C79CE0]    
			0x8B, 0x92, 0xD0, 0x2E, 0x00, 0x00, // MOV EDX, DWORD PTR DS:[EDX+2ED0]		: EDX = s_curManager
			0x64, 0xA1, 0x2C, 0x00, 0x00, 0x00, // MOV EAX, DWORD PTR FS:[2C]
			0x8B, 0x00,							// MOV EAX, DWORD PTR DS:[EAX]
			0x89, 0x50, 0x08,					// MOV DWORD PTR DS:[EAX+8], EDX		: Set address = s_curManager	
			
		/** Call function segment. **/
			0xBF, 0x00, 0x00, 0x00, 0x00,		// MOV EDI, executeAddress
			0x6A, 0x00,							// push 0
			0x6A, 0xFF,							// push -1
			0x68, 0x00, 0x00, 0x00, 0x00,		// push valueLocation (variable name string)
			0xFF, 0xD7,							// call edi
			0x83, 0xC4, 0x0C,					// add esp, 0C
			0xC3								// retn

	};
	
	strncpy(szBuffer, variable, strlen(variable) + 1);
	Stub codeStub(Endscene.getFunctionSpace(), code, sizeof(code));

	valueLocation = codeStub.getAddress() + sizeof(code);
	Stub valueStub(codeStub.getAddress() + sizeof(code), (byte *)szBuffer, strlen(szBuffer) + 1);

	memcpy(&code[24], &executeAddress, sizeof(executeAddress));
	memcpy(&code[33], &valueLocation, sizeof(valueLocation));
	
	Endscene.ClearFunctionSpace();
	valueStub.inject();
	codeStub.inject();

	time = Endscene.Execute(codeStub); // Buffer used for time to return.
	rpm(Endscene.ReadFunctionReturn(), &szBuffer, FUNCTIONSPACE_MAX); // Read pointer to get result.

	strncpy(result, szBuffer, strlen(szBuffer) + 1);
	return time;
}