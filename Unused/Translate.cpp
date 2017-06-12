#include "Translate.h"
#include "curl/curl.h"

string out;

string Translate::URLEncode(const string text)
{
	// Curtosy of some guy on some website.
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;

	for (string::const_iterator i = text.begin(), n = text.end(); i != n; ++i) {
		string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << '%' << setw(2) << int((unsigned char) c);
    }

    return escaped.str();
}

void Translate::Run()
{
	static bool state = false;

	char szBuffer[3000], name[256];
	char buff[4000];

	Translate translate;

	DWORD currentMessage = NULL;

	state = !state;

	if (state)
	{
		log("Translator started.");
	}
	else if (!state)
	{
		log("Translator exited.");
	}

	while (state)
	{
		if (currentMessage == GetCurrentChatMessagePointer() || GetLastChatType() == 17)
		{
			Sleep(200);
			continue;
		}

		log("Translating...");
		currentMessage = GetLastChatMessage(szBuffer, 3000, name, 256);
	
		translate.DetectLanguage(szBuffer);
		sprintf_s(buff, "[%s] [%s-en]: %s", name, translate.GetLastLanguageFrom().c_str(), translate.TranslateText(szBuffer, "en").c_str());
		log(buff);
		
		AddChatMessage(buff, "DTranslate", SystemMessage);
		Sleep(200);
	}

	ExitCurrentThread();
}

string Translate::GetTranslateLink(string langFrom, string langTo, string text)
{
	string link;
	link.assign("https://translate.yandex.net/api/v1.5/tr.json/translate?key=");
	link.append(GetKey());

	link.append("&lang=");
	link.append(langFrom);

	link.append("-");
	link.append(langTo);

	text = URLEncode(text);
	link.append("&text=");
	link.append(text);
	return link;
}

string Translate::GetKey()
{
	return "trnsl.1.1.20150328T222059Z.4a0209e449e16d73.d4e7aebe3f6d18dd3fad036ee71dc57e167f0970";
}

string Translate::GetDetectLink(string text)
{
	string link = "https://translate.yandex.net/api/v1.5/tr.json/detect?key=";
	link.append(GetKey());
	
	text = URLEncode(text);
	link.append("&text=");
	link.append(text);

	return link;
}

size_t Translate::CURLCallback(char *buff, size_t size, size_t nmemb, void *up)
{
	for (int c = 0; c < size * nmemb; c++)
	{ 
		out.push_back(buff[c]);
	}
		
	return size * nmemb;
}

string Translate::ParseTranslated(string text)
{	
	size_t pos = 0;
	size_t len = 0;

	string token;

	// {"code":200,"lang":"en-en","text":[" a \"\""]}

	pos = text.find("\"text\":[\"") + 9;
	len = text.find("\"]}") - pos;

	return text.substr(pos, len);
}

string Translate::ParseLanguage(string text)
{	
	size_t pos = 0;
	size_t len = 0;

	string token;

	// {"code":200,"lang":"en"}

	pos = text.find("\"lang\":\"") + 8;
	len = text.find("\"}") - pos;

	return text.substr(pos, len);
}

bool Translate::ErrorCheck(string text)
{
	return text.find("\"code\":501,") != string::npos;
}

string Translate::TranslateText(string text, string langTo)
{
	CURL *curl;
	CURLcode res;

	
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	this->langFrom.clear();
	out.clear();

	DetectLanguage(text);
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0");
		curl_easy_setopt(curl, CURLOPT_URL, GetTranslateLink(this->langFrom, langTo, text));
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CURLCallback);

		res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}

	if (ErrorCheck(out))
		return "<Error>";

	return ParseTranslated(out);
}


string Translate::TranslateText(string text, string langFrom, string langTo)
{
	CURL *curl;
	CURLcode res;

	
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	this->langFrom.clear();
	out.clear();

	this->langTo.assign(langTo);
	this->langFrom.assign(langFrom);
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0");
		curl_easy_setopt(curl, CURLOPT_URL, GetTranslateLink(this->langFrom, langTo, text));
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CURLCallback);

		res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}

	if (ErrorCheck(out))
		return "<Error>";

	return ParseTranslated(out);
}

string Translate::GetLastLanguageFrom()
{
	return this->langFrom;
}

string Translate::GetLastLanguageTo()
{
	return this->langTo;
}

string Translate::DetectLanguage(string text)
{
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	out.clear();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0");
		curl_easy_setopt(curl, CURLOPT_URL, GetDetectLink(text));
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CURLCallback);

		res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}

	this->langFrom.assign(ParseLanguage(out));
	return this->langFrom;
}