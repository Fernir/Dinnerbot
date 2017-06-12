#pragma once

#include <Windows.h>
#include <string>

#include <Main\Structures.h>

namespace Lua
{
	VOID RunScript(HWND hWnd);

	std::string GetText(std::string &var);
	std::string GetText(CONST CHAR *variable);
	std::string GenerateLocalVariable(uint length);

	DWORD GetText(CONST CHAR *variable, CHAR *result);

	DWORD Run(CONST CHAR *script);
	DWORD vDo(CONST CHAR *format, ...);
	DWORD DoString(CONST CHAR *script);
	DWORD DoString(std::string script);

	DWORD sendWhisper(std::string message, std::string name);
	DWORD sendReply(std::string message);

	DWORD AddChatMessage(const char *text, float r, float g, float b);
	DWORD FrameScriptExecute(CONST CHAR *script);
};