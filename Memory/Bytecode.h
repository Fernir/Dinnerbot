#pragma once

#include <Windows.h>
#include <vector>

class Bytecode
{
public:
	std::vector<byte> code;

	Bytecode(byte in) { code.push_back(in); }
	Bytecode() {}
	~Bytecode() {}

	// This works unless code.reallocate is called
	operator byte *()
	{
		return &this->code[0];
	}
};

Bytecode &operator << (Bytecode &bytes, const byte in)
{
	bytes.code.push_back(in);
	return bytes;
}
