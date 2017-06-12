#include "ClientDB.h"
#include <Game\Variable.h>

// g_ChrClassesDB = 0x00AD3404

bool ClientDB::isDBCCompressed()
{
	WoW::Variable<int> buffer(Offsets::isDBCCompressed);
	return (bool)buffer;
}

ClientDB::ClientDB(void)
{
	address = NULL;
}

ClientDB::ClientDB(DWORD dbPtr)
{
	address = dbPtr;
	UpdateHeader();
}

ClientDB::~ClientDB(void)
{
}

// for some reason, vTable2 is passed, so we need to offset the dbBase back by 0x18, assuming we use IDA's decompilation.
void ClientDB::UpdateHeader()
{
	rpm(this->address, &clientDB, 36);
}

// *** NEED TO REVERSE
int ClientDB::DecompressRow(int in, int size, int out)
{
	int v3; // edi@1
	int result; // eax@1
	int i; // ecx@1
	int j; // esi@3	 

	// IDA Decompiled.
	v3 = size + out;
	*(byte *)out = *(byte *)in;

	result = out + 1;
	for (i = in + 1; result < (unsigned int)v3; ++i)
	{
		*(byte *)result++ = *(byte *)i;
		if (*(byte *)i == *(byte *)(i - 1))
		{
			for (j = *(byte *)(i + 1); j; *(byte *)result++ = *(byte *)i)
				--j;
			i += 2;
			if (result < (unsigned int)v3)
				*(byte *)result++ = *(byte *)i;
		}
	}

	return result;
}

// *** NEED TO REVERSE
int ClientDB::GetLocalizedRow(int recordId, DWORD rowOut) // I think it's rowOut
{
	WoW::Variable<DWORD> rowTable;
	int *rowTableCompressed; // eax@3
	signed int result; // eax@5

	rowTable &= this->GetRow(recordId);

	if (recordId >= clientDB.minIndex
		&& recordId <= clientDB.maxIndex
		&& &rowTable
		&& (DWORD)rowTable)
	{
		if (ClientDB::isDBCCompressed())
		{
			ClientDB::DecompressRow(*rowTableCompressed, 0x2A8u, (int)rowOut);
			result = 1;
		}
		else
		{
			//memcpy((void *)rowOut, (const void *)*rowTableCompressed, 0x2A8u);
			result = 1;
		}
	}
	else
	{
		result = 0;
	}

	return result;
}

// Returns pointer to row.
// ClientDB__GetRow in WoW.exe
DWORD ClientDB::GetRow(int recordId)
{   
	WoW::Variable<DWORD> rowPtr;

	// Make sure the passed recordId is valid.
	if (recordId < clientDB.minIndex || recordId > clientDB.maxIndex)
		return 0;
	else
	{
		// (recordId - minIndex) is the index of the desired row.
		int index = (recordId - clientDB.minIndex);

		// Look up the nth row and return a pointer to that row.
		rowPtr &= clientDB.rowTable + 4 * index;
	}

	return (DWORD)rowPtr;
}

/*
DWORD ClientDB::GetRow(int recordId)
{
	int index = client;
	DWORD row = NULL; // eax@3
	WoW::Variable<DWORD> rowPtr(clientDB.rowTable + 4 * (recordId - clientDB.minIndex));

	// Make sure the passed recordId is valid.
	if (recordId < clientDB.minIndex || recordId > clientDB.maxIndex)
		return 0;
	else
	{
		// Looks up the nth row and return a pointer to that row.
		// (recordId - minIndex) is the index of the desired row.
		rpm(clientDB.rowTable + 4 * (recordId - clientDB.minIndex), &row, sizeof(row));
	}

	return row;
}*/

// a1 - row from clientDB
// a2 - idkk
// a3 - 0 for our purposes.
// Decompiled from IDA.
DWORD ClientDB::LookupRow(int a1, int a2, int a3)
{
	byte buff;
	int result; // eax@4
	int buffer;

	if (a3)
	{
		rpm(a3, &buffer, sizeof(buffer));
		wpm(buffer, &a2, sizeof(a2));
	}

	if (!a1)
		return 0;

	if (!a2)
	{
		rpm(a1 + 24, &result, sizeof(result));
		rpm(result, &buff, sizeof(buff));

		if (buff)
			return result;

		rpm(a1 + 20, &buffer, sizeof(buffer));
		rpm(buffer, &buff, sizeof(buff));
		if (buff)
		{
			if (a3)
			{
				rpm(a3, &buffer, sizeof(buffer));
				result = 1;

				wpm(buffer, &result, sizeof(result));
			}

			rpm(a1 + 20, &result, sizeof(result));
			return result;
		}

		rpm(a1 + 16, &result, sizeof(result));
		return result;
	}

	return 0;
}