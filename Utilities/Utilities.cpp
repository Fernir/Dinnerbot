#include "Utilities.h"

#include "Game\Game.h"
#include "WoW API\Lua.h"
#include "WoW API\Chat.h"
#include "Main\Dinnerbot.h"
#include "Dialog\Resource.h"
#include "Memory\Memory.h"

#include <TlHelp32.h>
#include <stdarg.h>
#include <memory> 

#include <iostream>
#include <fstream>

using namespace std;

std::string chatlogfile_path;
std::string statusfile_path;
std::string logfile_path;

BOOL log_init = false;

bool approxEqual(float v1, float v2)
{
	return abs(v1 - v2) < pow(10, -7);
}

void toLower(char *str)
{
	int len = strlen(str);

	for (int x = 0; x < len; x++)
		str[x] = tolower(str[x]);
}

void toUpper(std::string &str)
{
	int len = str.size();

	for (int x = 0; x < len; x++)
		str[x] = toupper(str[x]);
}

void toUpper(char *str)
{
	int len = strlen(str);

	for (int x = 0; x < len; x++)
		str[x] = toupper(str[x]);
}

void toLower(std::string &str)
{
	for (int x = 0; x < str.size(); x++)
		str[x] = tolower(str[x]);
}

FLOAT CalculateSpeedPercent(FLOAT speed)
{
	return (FLOAT)((speed / 7) * 100);
}

float decodeFloat(long ay)
{
	const float scale = 1024.0;

	if (ay < 0)
	{
		return ((float)ay + 0.5) / scale;
	}
	else
	{
		return ((float)ay - 0.5) / scale;
	}
}


UINT ParseBetween(CHAR *szBuffer, CHAR *szStart, CHAR *szEnd, CHAR *szOut)
{
	CHAR *start = strstr(szBuffer, szStart);
	CHAR *end = strstr(szBuffer, szEnd);
	CHAR *s = szBuffer;
	CHAR szBuffers[256];

	if (start != NULL && end != NULL) 
	{
		memset(szBuffer, 0, 256);
		start += strlen(szStart);
		
		strncpy(szBuffers, start, strlen(start) - strlen(end));
		strcpy(szOut, szBuffers);
	}

	return 0;
}

long encodeFloat(float ay)
{
	const float scale = 1024.0;

	if (ay < 0)
	{
		return ay * scale - 0.5;
	}
	else
	{
		return ay * scale + 0.5;
	}
}


// Ty someone elseee
int toBin(int d)
{
	long rem,i=1,sum=0;
	do
	{
		rem=d%2;
		sum=sum + (i*rem);
		d=d/2;
		i=i*10;
	}while(d>0);

	return sum;
}

bool isfloat (const char *s)
{
     char *ep = NULL;
     double f;

	 if (s == NULL) return false;
	 f = strtod (s, &ep);

     if (!ep  ||  *ep)
         return false;  // has non-floating digits after number, if any

     return true;
}

bool WipeFile(const char *filepath)
{
	FILE *fp = NULL;

	fopen_s(&fp, filepath, "w");
	if (fp == NULL) return false;

	fputs("", fp);
	fclose(fp);
	return true;
}

VOID CheckLogFileSize()
{
	CHAR szBuffer[256];

	UINT size = 0;
	FILE *logPtr = NULL;

	fopen_s(&logPtr, logfile_path.c_str(), "r");
	if (logPtr == NULL) return;
	
	fseek(logPtr, 0L, SEEK_END);
	size = ftell(logPtr);

	fseek(logPtr, 0L, SEEK_SET);
	fclose(logPtr);

	if (size > (2^20))
		WipeFile(logfile_path.c_str());
	
	sprintf_s(szBuffer, "%d", size);
	log(szBuffer);
}


// Get Main thread ID from process.
DWORD GetProcessThreadID(DWORD dwProcessID)
{   // Written by someone else.
    THREADENTRY32 te = { sizeof(THREADENTRY32) };
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    if(Thread32First(hSnapshot, &te))
        while(Thread32Next(hSnapshot, &te))
            if(te.th32OwnerProcessID == dwProcessID)
                return te.th32ThreadID;
	
    return NULL;
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) 
{
    size_t start_pos = 0;

    while((start_pos = str.find(from, start_pos)) != std::string::npos) 
	{
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }

    return str;
}

void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);

    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
	}
}

ULONGLONG GetTimerFormat(ULONGLONG time, char *out, size_t maxOut)
{
	char szBuffer[1024];

	ULONGLONG seconds = time / 1000;
	ULONGLONG minutes = seconds / 60;
	ULONGLONG hours = minutes / 60;
	
	sprintf_s(szBuffer, "%02lld:%02lld:%02lld", hours % 24, minutes % 60, seconds % 60);
	strncpy(out, szBuffer, (strlen(szBuffer) > maxOut)?(maxOut):(strlen(szBuffer) + 1));
	return strlen(szBuffer);
}


DWORD PatternScanRemote(BYTE *signature, char *Mask, DWORD length)
{
	DWORD currentAddr = (DWORD)0x40000;
	DWORD Max = 99999;

	int i = 0;

	DWORD old;
	MEMORY_BASIC_INFORMATION mbi;

	BYTE buffer = 0;

	do 
	{
		VirtualQueryEx(WoW::handle, (LPVOID)currentAddr, &mbi, sizeof(mbi));

		if((mbi.RegionSize <= 0x9000) && (mbi.State == 4096) && (mbi.Type == 131072))
		{
			VirtualProtectEx(WoW::handle, (LPVOID)currentAddr, mbi.RegionSize, 0x40, &old);

			if(currentAddr < Max) 
			{
				return 0;
			}
			else
				Max = currentAddr;

			for (int x = currentAddr; x < (currentAddr + mbi.RegionSize); x++)
			{
				ReadProcessMemory(WoW::handle, (LPVOID)(x), &buffer, sizeof(buffer), NULL);
				if ((buffer == signature[i]) || Mask[i] == '?')
					i++;
				else 
					i = 0;

				if(i >= length){
					return (x - length + 1);
				}
			}
		}

		currentAddr += mbi.RegionSize;

	} while(true);

	return 0;
}

BOOL IsProcessRunning(DWORD pid)
{
    HANDLE process = NULL;
    DWORD ret = NULL;

	if (!pid) return FALSE;

	process = OpenProcess(SYNCHRONIZE, FALSE, pid);
	if (!process) return FALSE;

	ret = WaitForSingleObject(process, 0);
    CloseHandle(process);
    return ret == WAIT_TIMEOUT;
}

// Reversed from WoW.exe
unsigned __int64 OsGetAsyncTimeMs()
{
    unsigned __int64 result = 0; // qax@3
    LARGE_INTEGER PerformanceCount; // [sp+4h] [bp-10h]@3
    LARGE_INTEGER v4; // [sp+Ch] [bp-8h]@3

	double dBuffer = 0;  // *(double *)a1
	double dBuffer0 = 0; // *(double *)(a1 + 24)
	
	DWORD ptr = NULL;
	DWORD buffer = NULL;
	DWORD DWORD_D4159C = 0xD4159C;

	rpm(DWORD_D4159C, &ptr, sizeof(ptr));
	rpm(ptr + 0x8, &buffer, sizeof(buffer));
	
	rpm(ptr, &dBuffer, sizeof(dBuffer));
	rpm(ptr + 0x18, &dBuffer0, sizeof(dBuffer0));

    //if ( *(_DWORD *)(a1 + 8) == 1 || *(_DWORD *)(a1 + 8) != 2 )
	if (buffer == 1 || buffer != 2)
    {
        //result = (unsigned __int64)((double)GetTickCount() * *(double *)a1 + *(double *)(a1 + 24));
		result = (unsigned __int64)((double)GetTickCount() * dBuffer + dBuffer0);
    }
    else
    {
        QueryPerformanceCounter(&PerformanceCount);
        v4 = PerformanceCount;

        //result = (unsigned __int64)((double)*(signed __int64 *)&PerformanceCount * *(double *)v1 + *(double *)(v1 + 24));
		result = (unsigned __int64)((double)*(signed __int64 *)&PerformanceCount * dBuffer + dBuffer0);
    }

    return result;
}

DWORD WaitOnWoW()
{
	DWORD start = GetTickCount();

	while (IsWoWFrozen())
	{
		Sleep(200);
		if (!IsProcessRunning(GetProcessId(WoW::handle))) return WOW_CRASHED;

		if (GetTickCount() - start > 10000)
		{
			log("I'm sorry, there's nothing I can do. He's dead.");
			LogFile("Wow has not responded for more than 10 seconds.");
			return WOW_UNRESPONSIVE;
		}
	}

	return WOW_ALIVE;
}

BOOL IsWoWFrozen()
{
	return WoWHangTime() > 400;
}

INT WoWHangTime()
{
	// Sometimes GetTimestamp is much different than GetTickCount, so we use WoW's OsGetAsyncTimeMs to compare timestamps.
	return abs((INT)GetTimestamp() - (INT)OsGetAsyncTimeMs());
}

/*
DWORD HasProcessFrozen(HWND window)
{
	SendMessageTimeoutA(window, WM_NULL, NULL, NULL, SMTO_BLOCK | SMTO_ABORTIFHUNG, 1000, NULL);

	return false;
}*/

bool fileExists(std::string name)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(name.c_str(), &FindFileData);

	int found = (handle != INVALID_HANDLE_VALUE);

	if (found)
		FindClose(handle);

	return found;
}

std::string getNextChatlogName()
{
	int x = 0;

	string result;
	char name[MAX_PATH];

	do
	{
		sprintf_s(name, "chatlog%d.txt", x++);

	} while (fileExists(name));

	result = name;
	return result;
}

std::string ResetChatlog()
{
	std::string &newName = getNextChatlogName();

	// Copy chatlog.txt into newName
	std::ifstream source("chatlog.txt", std::ios::binary);
	std::ofstream destination(newName.c_str(), std::ios::binary);

	destination << source.rdbuf();

	source.close();
	destination.close();

	if (!WipeFile("chatlog.txt"))
		velog("Unable to wipe file");

	return newName;
}

VOID InitializeLogFile()
{
	CHAR buffer[MAX_PATH];

	log_init = true;
	GetCurrentDirectory(MAX_PATH, buffer);

	logfile_path = buffer;
	logfile_path += "\\dinnerlog.txt";

	statusfile_path = buffer;
	statusfile_path += "\\status.txt";

	chatlogfile_path = buffer;
	chatlogfile_path += "\\chatlog.txt";
	
	CheckLogFileSize();
	vlog("Log File: %s", logfile_path.c_str());
	vlog("Status File: %s", statusfile_path.c_str());
	vlog("Chatlog File: %s", chatlogfile_path.c_str());
}

VOID LogStatus(BOOL status)
{
	CHAR szBuffer[256];
	long int size = 0;
	FILE *logPtr = NULL;
	
	if (!log_init) InitializeLogFile();

	fopen_s(&logPtr, statusfile_path.c_str(), "w");
	if (logPtr == NULL) return;

	if (status)
		sprintf(szBuffer, "<font face=\"Verdana Bold\"><font color=\"#aaaaaa\">Status:</font><font color=\"#35FF48\">Online</font></font>");
	else
		sprintf(szBuffer, "<font face=\"Verdana Bold\"><font color=\"#aaaaaa\">Status:</font><font color=\"red\">Offline</font></font>");
	
	fputs(szBuffer, logPtr);
	LogFile(szBuffer);
	fclose(logPtr);
}

VOID LogFile(CONST CHAR *message)
{
	SYSTEMTIME st;
	CHAR *szTimeBuffer = NULL;
	long int size = 0;
	FILE *logPtr = NULL;
	
	return;
	if (!log_init) InitializeLogFile();
	if (message == NULL) return;

	fopen_s(&logPtr, logfile_path.c_str(), "a+");
	if (logPtr == NULL) return;

	GetLocalTime(&st);
	szTimeBuffer = (CHAR *)GlobalAlloc(GPTR, (sizeof(CHAR) * 32) + strlen(message));

	if (szTimeBuffer == NULL) return;
	sprintf(szTimeBuffer, "[%02d:%02d:%02d] %s\r\n", st.wHour, st.wMinute, st.wSecond, message);
	
	fputs(szTimeBuffer, logPtr);
    GlobalFree(szTimeBuffer);
	fclose(logPtr);
}

VOID LogChat(CONST CHAR *message)
{
	SYSTEMTIME st;
	CHAR *szTimeBuffer = NULL;
	long int size = 0;
	FILE *logPtr = NULL;
	
	if (!g_chatLog) return;
	if (!log_init) InitializeLogFile();
	if (message == NULL) return;

	fopen_s(&logPtr, chatlogfile_path.c_str(), "a+");
	if (logPtr == NULL) return;

	GetLocalTime(&st);
	szTimeBuffer = (CHAR *)GlobalAlloc(GPTR, (sizeof(CHAR) * 32) + strlen(message));

	if (szTimeBuffer == NULL) return;
	sprintf(szTimeBuffer, "[%02d:%02d:%02d] %s\r\n", st.wHour, st.wMinute, st.wSecond, message);
	
	LogFile(szTimeBuffer); 
	fputs(szTimeBuffer, logPtr);
    GlobalFree(szTimeBuffer);
	fclose(logPtr);
}

bool isInteger(std::string str)
{
	int len = str.size();
	for (auto &each : str)
	{
		if (!isdigit(each))
			return false;
	}

	return true;
}

BOOL isinteger(CONST CHAR *szBuffer)
{
	if (szBuffer == NULL) return false;

	int len = strlen(szBuffer);
	for (int x = 0; x < len; x++)
	{
		if (!isdigit(*(szBuffer + x))) return FALSE;
	}

	return TRUE;
}

// **WARNING: Be very careful of buffer overflows.
VOID vflog(CONST CHAR *format, ...)
{
	va_list		args;
	CHAR buffer[10000];

	va_start(args, format);
	vsprintf_s(buffer, format, args);
	va_end(args);

	LogFile(buffer);
}

// **WARNING: Be very careful of buffer overflows.
VOID velog(CONST CHAR *format, ...)
{
	va_list		args;
	CHAR buffer[10000];

	va_start(args, format);
	vsprintf_s(buffer, format, args);
	va_end(args);

	vlog("Error (%d): %s", GetLastError(), buffer);
	vflog("Error (%d): %s", GetLastError(), buffer);
}

// **WARNING: Be very careful of buffer overflows.
VOID vlog(CONST CHAR *format, ...)
{
	va_list		args;
	CHAR buffer[4096];

	va_start(args, format);
	vsprintf_s(buffer, format, args);
	va_end(args);

	LogAppend(buffer);
}

VOID vout(OutputFunction out, CONST CHAR *format, ...)
{
	va_list		args;
	CHAR buffer[4096];

	va_start(args, format);
	vsprintf_s(buffer, format, args);
	va_end(args);

	out(buffer);
}

VOID LogAppend(CHAR *text)
{
	SYSTEMTIME	st;

	HWND		hWndLog			= GetDlgItem(g_hwMainWindow, IDC_MAIN_TAB_EDIT_LOG);
	CHAR		*timeBuffer 	= NULL;
	DWORD		outLength		= NULL;
    DWORD		start			= NULL;
	DWORD		end				= NULL;

	if (text == NULL) return;
	if (g_hwMainWindow == NULL || hWndLog == NULL) return;

	GetLocalTime(&st);
	timeBuffer = (CHAR *)GlobalAlloc(GPTR, (sizeof(CHAR) * 32) + strlen(text));
	if (timeBuffer == NULL) 
	{
		LogFile("LogAppend: Global allocation error.");
		return;
	}

    SendMessage(hWndLog, EM_GETSEL, (WPARAM)(&start), (WPARAM)(&end));
    outLength = GetWindowTextLength(hWndLog);
	
	if (g_VerboseInGameChat)
	{
		// TODO: break 20 line message into separate 5-10 line messages
		AddChatMessage(text, TurqoiseMessage);
		//Lua::AddChatMessage(text, 0, 0.1, 1);
	}

	sprintf(timeBuffer, "[%02d:%02d:%02d] %s", st.wHour, st.wMinute, st.wSecond, text);
	strncat(timeBuffer, "\r\n", 2);
	if (outLength == 0 || outLength >= MAX_LOG)
	{
		// If the log is full or empty then clear it.
		SetWindowTextA(hWndLog, timeBuffer);
		GlobalFree(timeBuffer);
		return;
	}
	
    // move the caret to the end of the text
    SendMessageA(hWndLog, EM_SETSEL, outLength, outLength);

    // insert the text at the new caret position
    SendMessageA(hWndLog, EM_REPLACESEL, TRUE, (LPARAM)(timeBuffer));

    // restore the previous selection
    SendMessage(hWndLog, EM_SETSEL, start, end);
    GlobalFree(timeBuffer);
	return;
}

bool isASCIIString(const char *string)
{
	if (string == NULL) return false;

	for (int x = 0; x < strlen(string); x++)
	{
		if (!isascii(*(string + x))) return false;
	}

	return true;
}

// Not mine, but nice
std::string string_format(const std::string fmt_str, ...)
{
    int final_n, n = ((int)fmt_str.size()) * 2; //Reserve two times as much as the length of the fmt_str 
    std::string str;
    std::unique_ptr<char[]> formatted;
    va_list ap;

    while (1)
	{
        formatted.reset(new char[n]); // Wrap the plain char array into the unique_ptr 
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);

        final_n = vsprintf_s(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);

        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }

    return std::string(formatted.get());
}