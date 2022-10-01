#include "pch.h"
#include "widerender.h"
#include <fstream>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    return TRUE;
}

extern "C" {
	__declspec(dllexport) extern void InitializeEarly()
	{
		const auto proc = GetCurrentProcess();

		std::ofstream log("size.txt");


		for (auto address : addresses960)
		{
			SIZE_T b;
			float newVal = 1280.0f;
			address = baseAddr + address;
			float oldVal;
			ReadProcessMemory(proc, (LPVOID) address, &oldVal, 4, &b);
			log << oldVal << std::endl;
			//WriteProcessMemory(proc, (LPVOID) address, &newVal, 4, &b);
		}

		log.flush();
		log.close();


		DWORD gameWidthAddr = baseAddr + 0xCC88A;
		SIZE_T b;
		float val = 426.66667f;
		WriteProcessMemory(proc, (LPVOID) gameWidthAddr, &val, 4, &b);
	}
}
