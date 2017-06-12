#pragma once

#include "Variable.h"

namespace WoW
{
	template <typename T>
	class Variable;

	template <typename T>
	class Pointer
	{
	private:
		DWORD address;

	public:
		Pointer(Variable<T>& var) { this->address = &var; }
		Pointer(DWORD addr) : address(addr) {}

		Pointer() : address(0) {}

		DWORD operator &()
		{
			return address;
		}

		T operator *()
		{
			return *Variable<T>(this->address);
		}

		Pointer<T> &operator = (DWORD &rhs)
		{
			this->address = rhs;
			return *this;
		}

		Pointer<T> &operator = (Variable<T> &rhs)
		{
			this->address = &rhs;
			return *this;
		}
	};
};

// ptr + val
template <typename T>
inline bool operator ==(WoW::Pointer<T> &ptr, WoW::Pointer<T> &ptr2)
{
	return &(WoW::Pointer<T>)ptr == &(WoW::Pointer<T>)ptr2;
}

// ptr + val
template <typename T>
inline bool operator !=(WoW::Pointer<T> &ptr, WoW::Pointer<T> &ptr2)
{
	return &(WoW::Pointer<T>)ptr != &(WoW::Pointer<T>)ptr2;
}

// ptr + val
template <typename T>
WoW::Pointer<T> inline operator +(const WoW::Pointer<T> &ptr, const DWORD &val)
{
	return WoW::Pointer<T>(&(WoW::Pointer<T>)ptr + val);
}

// val + ptr
template <typename T>
WoW::Pointer<T> inline operator +(const DWORD &val, const WoW::Pointer<T> &ptr)
{
	return WoW::Pointer<T>(&(WoW::Pointer<T>)ptr + val);
}

// val * ptr
template <typename T>
WoW::Pointer<T> inline operator *(const DWORD &val, const WoW::Pointer<T> &ptr)
{
	return WoW::Pointer<T>(&(WoW::Pointer<T>)ptr * val);
}

// ptr * val
template <typename T>
WoW::Pointer<T> inline operator *(const WoW::Pointer<T> &ptr, const DWORD &val)
{
	return WoW::Pointer<T>(&(WoW::Pointer<T>)ptr * val);
}

// ptr & val
template <typename T>
WoW::Pointer<T> inline operator &(WoW::Pointer<T> &ptr, DWORD &val)
{
	return WoW::Pointer<T>(&(WoW::Pointer<T>)ptr & val);
}

// val & ptr
template <typename T>
WoW::Pointer<T> inline operator &(DWORD &val, WoW::Pointer<T> &ptr)
{
	return WoW::Pointer<T>(&(WoW::Pointer<T>)ptr & val);
}

// val & ptr
template <typename T>
WoW::Pointer<T> inline operator &(WoW::Pointer<T> &lhs, WoW::Pointer<T> &ptr)
{
	return WoW::Pointer<T>(&(WoW::Pointer<T>)ptr & &(WoW::Pointer<T>)lhs);
}

template <typename T>
WoW::Pointer<T> inline operator +(const WoW::Pointer<T> &lhs, const WoW::Pointer<T> &rhs)
{
	return WoW::Pointer<T>(&(WoW::Pointer<T>)lhs + &(WoW::Pointer<T>)rhs);
}

template <typename T>
WoW::Pointer<T> inline operator *(const WoW::Pointer<T> &lhs, const WoW::Pointer<T> &rhs)
{
	return WoW::Pointer<T>(&(WoW::Pointer<T>)p(WoW::Pointer<T>)tr + &rhs);
}