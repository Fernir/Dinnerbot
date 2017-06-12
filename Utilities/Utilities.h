#pragma once
#include <Windows.h>

#include <string>
#include <sstream>
#include <vector>

#define log LogAppend

#define MAX_LOG 200000

#define WOW_UNRESPONSIVE 0x55
#define WOW_CRASHED 0x56
#define WOW_ALIVE 0x54

FLOAT CalculateSpeedPercent(FLOAT speed);

int toBin(int d);

bool isfloat (const char *s);
bool hook(DWORD HookAddress, DWORD CaveAddress);

bool approxEqual(float v1, float v2);

VOID InitializeLogFile();

VOID LogStatus(BOOL message);
VOID LogFile(CONST CHAR *message);
VOID LogChat(CONST CHAR *message);

std::string ResetChatlog();

typedef VOID(*OutputFunction)(char *);
VOID vout(OutputFunction out, CONST CHAR *format, ...);

VOID LogAppend(CHAR *text);
VOID vlog(CONST CHAR *format, ...);
VOID velog(CONST CHAR *format, ...);
VOID vflog(CONST CHAR *format, ...);

void toLower(char *str);
void toLower(std::string &str);
void toUpper(char *str);

INT WoWHangTime();
UINT ParseBetween(CHAR *szBuffer, CHAR *szStart, CHAR *szEnd, CHAR *szOut);

DWORD WaitOnWoW();
DWORD GetProcessThreadID(DWORD dwProcessID);
DWORD PatternScanRemote(BYTE *signature, char *Mask, DWORD length);

BOOL IsWoWFrozen();
BOOL isinteger(CONST CHAR *szBuffer);
bool isInteger(std::string str);

bool isASCIIString(const char *string);

unsigned __int64 OsGetAsyncTimeMs();

float decodeFloat(long ay);
long encodeFloat(float ay);

template <typename T>
bool vectorHas(const T &cmp, const std::vector<T> &v)
{
	for (int x = 0; x < v.size(); x++)
	{
		if (v[x] == cmp)
			return true;
	}

	return false;
}

void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters);

ULONGLONG GetTimerFormat(ULONGLONG start, char *out, size_t maxOut);

std::string string_format(const std::string fmt_str, ...);

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);

extern BOOL g_chatLog;