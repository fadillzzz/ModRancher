#pragma once
static DWORD baseAddr = reinterpret_cast<DWORD>(GetModuleHandleA(NULL));
struct Size {
	float width;
	float height;
};
typedef DWORD Director;
typedef Size(__thiscall* GetVisibleSize)(DWORD director);
typedef const Size& (__thiscall* GetWinSize)(DWORD director);

static DWORD widthAddress = baseAddr + 0xCC88A;
static DWORD overlayControlHook = baseAddr + 0xC2AAA;
static DWORD hideOverlayControlReturn = baseAddr + 0xC2AB0;
