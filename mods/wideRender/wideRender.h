#pragma once
static DWORD baseAddr = reinterpret_cast<DWORD>(GetModuleHandleA(NULL));
struct Size {
	float width;
	float height;
};
typedef DWORD Director;
typedef Size(__thiscall* GetVisibleSize)(DWORD director);
typedef const Size& (__thiscall* GetWinSize)(DWORD director);

DWORD addresses960[7] = { 0x27BE6,  0xC6476, 0xC72F6, 0xC9CFF, 0xCE0F5, 0x1B9F9C, 0x1D3088 };
