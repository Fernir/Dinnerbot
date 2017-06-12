#pragma once

#include "Dinnerbot.h"

#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

class Translate
{

private:
	
	string langFrom;
	string langTo;

	static string	URLEncode(string text);
	static string	ParseLanguage(string text);
	static string	ParseTranslated(string text);
	static size_t	CURLCallback(char *buff, size_t size, size_t nmemb, void *up);

public:
	
	static void Translate::Run();

	bool ErrorCheck(string text);

	string GetKey();
	string GetLastLanguageTo();
	string GetLastLanguageFrom();
	string GetDetectLink(string text);
	string DetectLanguage(string text);
	string TranslateText(string text, string langTo);
	string TranslateText(string text, string langFrom, string langTo);
	string GetTranslateLink(string langFrom, string langTo, string text);
};