#pragma once
static DWORD baseAddr = reinterpret_cast<DWORD>(GetModuleHandleA(NULL));
struct Size {
	float width;
	float height;
};
typedef DWORD Director;
typedef Size(__thiscall* GetVisibleSize)(DWORD director);
typedef const Size& (__thiscall* GetWinSize)(DWORD director);

#define MRDX 1

#if MRDX == 1
static DWORD widthAddress = baseAddr + 0xCC88A;
static DWORD overlayControlHook = baseAddr + 0xC2AAA;
static DWORD hideOverlayControlReturn = baseAddr + 0xC2AB0;
#else
static DWORD widthAddress = baseAddr + 0x165C9A;
static DWORD overlayControlHook = baseAddr + 0x1584E9;
static DWORD hideOverlayControlReturn = baseAddr + 0x1584EF;
#endif
