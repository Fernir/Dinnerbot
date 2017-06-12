#include "HTML.h"
#include <Windows.h>

#include <string>
#include <fstream>
#include <iostream>

#include "..\WoW API\Guild.h"
#include "..\WoW API\GuildEntry.h"

typedef struct 
{
	string key;
	string url;
} ClassEntry;

int FindClassByKey(string key, ClassEntry *classes, size_t size)
{
	for (int x = 0; x < size; x++)
	{
		if (!_stricmp(key.c_str(), (classes+x)->key.c_str()))
			return x;
	}

	return -1;
}

// Number of guild members online.
VOID BuildRosterMembersOnlineHTML(Guild *guild)
{
	if (!g_rosterMode) return;
	// http://tomeko.net/online_tools/cpp_text_escape.php?lang=en is an excellent tool for converting HTML into cstring format.
	ofstream membersFile;
	string	newEntry;
	string	base =	
			"<html>\n"
				"\t<head>\n"
					"\t\t<meta content=\"IE=Edge,chrome=1\" http-equiv=\"X-UA-Compatible\">\n"
					"\t\t<meta charset=\"UTF-8\">\n"
					"\t\t<meta content=\"en-us\" http-equiv=\"Content-Language\">\n"
					"\t\t<link rel=\"stylesheet\" media=\"all\" href=\"style.css\">\n"
				"\t</head>\n\n"
				"\t<body style=\"margin-top: 5px; color: #CAAD83; text-align: center; font-family: Verdana, sans-serif; font-size: 11px;\">\n"
				"\t</body>\n"
			"</html>";

	HTML numOnline(base);

	if (!WoW::InGame() || guild == NULL)
		newEntry = "\t\tGuild Roster Unavailable";
	else
		newEntry = string_format("\t\t%d / %d Online", guild->online-1, guild->total);

	numOnline.InsertAfter(newEntry, "<body style=\"margin-top: 5px; color: #CAAD83; text-align: center; font-family: Verdana, sans-serif; font-size: 11px;\">\n");

	membersFile.open("guild//members.html");
	membersFile << numOnline.code;
	membersFile.close();
}


VOID BuildRosterHTML(Guild *guild)
{
	ClassEntry classes[] = {
		{"death knight","http://cloudfront.shivtr.com/game_classes/7/out.png?1356758984"},
		{"druid",		"http://cloudfront.shivtr.com/game_classes/11/out.png?1356758989"},
		{"hunter",		"http://cloudfront.shivtr.com/game_classes/4/out.png?1356758995"},
		{"mage",		"http://cloudfront.shivtr.com/game_classes/9/out.png?1356759011"},
		{"paladin",		"http://cloudfront.shivtr.com/game_classes/3/out.png?1356759023"},
		{"priest",		"http://cloudfront.shivtr.com/game_classes/6/out.png?1356759030"},
		{"rogue",		"http://cloudfront.shivtr.com/game_classes/5/out.png?1356759037"},
		{"shaman",		"http://cloudfront.shivtr.com/game_classes/8/out.png?1356759044"},
		{"warlock",		"http://cloudfront.shivtr.com/game_classes/10/out.png?1356759050"},
		{"warrior",		"http://cloudfront.shivtr.com/game_classes/2/out.png?1356759056"},
	};
		
	string base =	"<html>\n"
						"\t<head>\n"
							"\t\t<meta content=\"IE=Edge,chrome=1\" http-equiv=\"X-UA-Compatible\">\n"
							"\t\t<meta charset=\"UTF-8\">\n"
							"\t\t<meta content=\"en-us\" http-equiv=\"Content-Language\">\n"
							"\t\t<link rel=\"stylesheet\" media=\"all\" href=\"style.css\">\n"
							"\t\t<script src=\"zepto.js\"></script>\n"
							"\t\t<script src=\"fittext.js\"></script>"
						"\t</head>\n\n"
						"\t<body>\n"
							"\n" // table
						"\t</body>\n"
					"</html>\n";
		
	string table = "\t\t<div class=\"scollbar\">\n"
						"\t\t\t<table align=\"left\" class=\"stripe dTable no-footer\" role=\"grid\" style=\"width: 100%;\">\n"
							"\t\t\t\t<tbody>\n"
							"\t\t\t\t\t<script type=\"text/javascript\">\n"
								"\t\t\t\t\t\t$(\".dTable\").fitText(1.7);\n"
							"\t\t\t\t\t</script>"
							"\t\t\t\t</tbody>\n"
						"\t\t\t</table>\n"
					"\t\t</div>";


	string newEntry = 
					"\t\t\t\t\t<tr role=\"row\" class=\"%s\">\n" // odd or even
						"\t\t\t\t\t\t<td style=\"text-align: center;\"><img src=\"%s\"></td>\n"
						"\t\t\t\t\t\t<td><a target=\"_blank\" href=\"http://armory.warmane.com/character/%s/Icecrown/profile\">%s</a></td>\n"
						"\t\t\t\t\t\t<td style=\"text-align: center; font-size: 8px;\">%s</td>\n"
					"\t\t\t\t\t</tr>\n";

	int index;
	bool parity = true; // false = even, true = odd.

	ofstream rosterFile;
	HTML roster(base);

	roster.InsertAfter(table, "<body>\n");
	for (int x = 0; x < guild->roster.size(); x++)
	{
		// Pont remains hidden.
		if (!_stricmp(guild->roster[x].name.c_str(), "Pont")) continue;

		// Get WoW class picture by url from className.
		index = FindClassByKey(guild->roster[x].className, classes, 10);

		// Insert new table data after <tbody> tag.
		roster.InsertAfter(
			string_format(newEntry, (parity)? "even":"odd", classes[index].url.c_str(), guild->roster[x].name.c_str(), guild->roster[x].name.c_str(), guild->roster[x].zone.c_str()), "<tbody>\n");
		
		// For table css style.
		parity = !parity; 
	}

	// Output html rosterBar into rosterbar.html.
	rosterFile.open("guild//rosterbar.html");
	rosterFile << roster.code;
	rosterFile.close();
}

string::size_type HTML::NextTag(string tag)
{
	lastPos = code.find(tag, lastPos);

	if (lastPos == string::npos) return string::npos;

	lastPos += tag.length();
	return lastPos;
}

void HTML::ResetIterate()
{
	lastPos = 0;
}

// Inserts given string after tag.
void HTML::InsertAfter(string entry, string tag)
{
	string::size_type old = lastPos;

	NextTag(tag);
	code.insert(lastPos, entry);
	lastPos = old;
}

int HTML::CountTags(string tag)
{
	string::size_type pos;
	int x = 0;

	for (; ; x++)
	{
		pos = NextTag(tag);
		if (pos == string::npos)
			break;
	}

	ResetIterate();
	return x;
}


HTML::HTML(string init)
{
	code = init;
	lastPos = 0;
}

HTML::~HTML(void)
{
}
