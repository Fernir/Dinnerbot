#include "CDataStore.h"

// signed int __stdcall CDataStore::InternalFetchWrite(int this, int pos, void **address, int a4, int a5, int a6, DWORD a7)

int CDataStore_vTable::InternalFetchWrite(unsigned int pos, unsigned int bytes, unsigned __int8 *data, unsigned int base, unsigned int alloc)
{
/*    unsigned int size; // eax@1
    int v8; // edx@1
    DWORD v9; // edi@2
    DWORD v11; // [sp+28h] [bp+20h]@0

    size = (unsigned int)((char *)address + pos + 255) & 0xFFFFFF00;
    *(DWORD *)a6 = size;
    v8 = a7;
    if ( a7 )
    {
        v9 = v11;
    }
    else
    {
        v9 = 280;
        v8 = (int)"d:\\buildserver\\wow\\1\\work\\wow-code\\branches\\wow-patch-3_3_5_a-bnet\\engine\\source\\base\\CDataStore.h";
    }
    *(DWORD *)a4 = SMemReAlloc(*(void **)a4, size, v8, v9, 0);*/
    return 1;
}

signed int __thiscall CDataStore::CanRead(unsigned int a2, int a3)
{
	CDataStore *v3; // esi@1
	unsigned int v4; // eax@1
	signed int result; // eax@2
	unsigned int v6; // eax@3

	v3 = this;
	v4 = this->size;
	if (a2 + a3 <= v4)
	{
		v6 = this->base;
		if (a2 >= v6 && a2 + a3 <= v6 + this->alloc
			/*|| this->vTable->InternalFetchRead(a2,
			a3,
			(unsigned __int8 *)&this->buffer,
			(unsigned int)&this->base,
			(unsigned int)&this->alloc)*/)
		{
			result = 1;
		}
		else
		{
			v3->read = v3->size + 1;
			result = 0;
		}
	}
	else
	{
		this->read = v4 + 1;
		result = 0;
	}

	return result;
}


int CDataStore_vTable::GetBufferParams(const void **data, unsigned int *size, unsigned int *alloc)
{
    int result; // eax@5
	// Need to write.

    if ( data )
        *data = *(void **)(this + 4);

    if ( size )
        *size = *(DWORD *)(this + 16);

    result = (int)alloc;
    if ( alloc )
        *alloc = *(DWORD *)(this + 12);

    return result;
}