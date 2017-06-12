#pragma once

#include <Windows.h>
#include "..\WoW API\Guild.h"

using namespace std;

class HTML
{
private:

public:
	string::size_type lastPos;
	string code;

	HTML(string txt);
	HTML(void);

	~HTML(void);

	string::size_type HTML::NextTag(string tag);

	void HTML::InsertAfter(string entry, string tag);
	void HTML::ResetIterate();

	int HTML::CountTags(string tag);
};

VOID BuildRosterHTML(Guild *guild);
VOID BuildRosterMembersOnlineHTML(Guild *guild);
