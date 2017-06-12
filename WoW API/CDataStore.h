#pragma once
#include <Windows.h>
#include <memory.h>

class CDataStore_vTable
{
public:
	~CDataStore_vTable(); // Destructor.

	void *InternalInitialize(unsigned __int8 *data, unsigned int base, unsigned int alloc);
	void *InternalDestroy(unsigned __int8 *data, unsigned int base, unsigned int alloc);

	int *InternalFetchRead(unsigned int pos, unsigned int bytes, unsigned __int8 *data, unsigned int base, unsigned int alloc);
	int InternalFetchWrite(unsigned int pos, unsigned int bytes, unsigned __int8 *data, unsigned int base, unsigned int alloc);

	void *Reset();
	void *IsRead();
	void *Finalize();
	int GetBufferParams(const void **data, unsigned int *size, unsigned int *alloc);
	void *DetachBuffer(void **buffer, unsigned int *size, unsigned int *alloc);
};

//00000000 CDataStore      struc; (sizeof = 0x18)
//00000000 vTable          dd ? ; offset
//00000004 m_buffer        dd ? ; offset
//00000008 m_base          dd ?
//0000000C m_alloc         dd ?
//00000010 m_size          dd ?
//00000014 m_read          dd ?

class CDataStore
{
private:
	CDataStore_vTable *vTable;
	unsigned __int8 *buffer;
	DWORD base;
	DWORD alloc;
	DWORD size;
	DWORD read;

public:
	signed int CanRead(unsigned int a2, int a3);

};
