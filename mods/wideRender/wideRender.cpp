#include "pch.h"
#include "widerender.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    return TRUE;
}

__declspec(naked) void hideOverlayControl()
{
	__asm {
		mov eax, 5
#if MRDX == 1
		mov DWORD PTR[ebx + 0x604], eax
#else
		mov DWORD PTR[ebx + 0x614], eax
#endif
		jmp hideOverlayControlReturn
	}
}

extern "C" {
	__declspec(dllexport) extern void InitializeEarly()
	{
		const auto proc = GetCurrentProcess();

		DWORD oldProtect;
		VirtualProtect((void*)overlayControlHook, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
		*(DWORD*)overlayControlHook = 0xE9;
		*(DWORD*)(overlayControlHook + 1) = (DWORD)&hideOverlayControl - (overlayControlHook + 5);
		*(char*)(overlayControlHook + 5) = 0x90;

		float val = 426.66667f;
		SIZE_T b;
		WriteProcessMemory(proc, (LPVOID) widthAddress, &val, 4, &b);
	}
}
