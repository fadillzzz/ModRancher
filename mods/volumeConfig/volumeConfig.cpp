#include "pch.h"
#include "volumeConfig.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    return TRUE;
}

Config getConfig()
{
	std::unordered_map<std::string, float> config;

	std::ifstream cfgFile("cfg.txt");

	std::vector<std::regex> regexes = {
		std::regex("^sfxVolume\\s*=\\s*([0-9.]+)", std::regex_constants::ECMAScript | std::regex_constants::icase),
		std::regex("^musicVolume\\s*=\\s*([0-9.]+)", std::regex_constants::ECMAScript | std::regex_constants::icase),
		std::regex("^fmvVolume\\s*=\\s*([0-9.]+)", std::regex_constants::ECMAScript | std::regex_constants::icase),
	};

	std::vector<std::string> keys = {
		"sfxVolume", "musicVolume", "fmvVolume"
	};

	for (std::string line; getline(cfgFile, line);)
	{
		std::smatch matches;

		for (int i = 0; i < regexes.size(); i++)
		{
			std::regex pattern = regexes[i];
			std::string key = keys[i];

			if (std::regex_search(line, matches, pattern))
			{
				config.insert({ key, std::stof(matches[1]) });
			}
		}
	}

	cfgFile.close();

	Config structuredConfig = {
		config.find("sfxVolume")->second,
		config.find("musicVolume")->second,
		config.find("fmvVolume") == config.end() ? 1.0 : config.find("fmvVolume")->second,
	};

	return structuredConfig;
}

__declspec(naked) void setFMVolume()
{
	__asm {
		mov eax, [esi + 8]
		mov mediaSession, eax
	}

	MFGetService(mediaSession.Get(), MR_POLICY_VOLUME_SERVICE, IID_PPV_ARGS(&simpleVolume));
	simpleVolume->SetMasterVolume(config.fmvVolume);

	__asm {
		mov eax, [esi + 8]
		lea edx, [ebp - 0x14]
		jmp returnAddr
	}
}

extern "C" {
	__declspec(dllexport) extern void Initialize()
	{
		HANDLE proc = GetCurrentProcess();
		uintptr_t musicVolAddr = baseAddr + MR1_OFFSET;
		uintptr_t sfxVolAddr = musicVolAddr + 4;

		float musicVol;
		float sfxVol;
		SIZE_T b;
		bool isMR2DX = false;

		ReadProcessMemory(proc, (LPVOID)musicVolAddr, &musicVol, 4, &b);
		ReadProcessMemory(proc, (LPVOID)sfxVolAddr, &sfxVol, 4, &b);

		constexpr float epsilon = std::numeric_limits<float>::epsilon();

		if (fabs(1.0 - musicVol) > epsilon || fabs(1.0 - sfxVol) > epsilon)
		{
			// If the current volume isn't 1.0, then we assume we're on MR2DX
			isMR2DX = true;
		}

		std::ifstream cfgFile("cfg.txt");

		if (!cfgFile.good())
		{
			std::ofstream newCfg("cfg.txt");
			newCfg << "sfxVolume=" << config.sfxVolume << std::endl;
			newCfg << "musicVolume=" << config.musicVolume << std::endl;

			if (isMR2DX)
			{
				newCfg << "fmvVolume=" << config.fmvVolume << std::endl;
			}

			newCfg.flush();
			newCfg.close();
		}

		config = getConfig();

		if (isMR2DX)
		{
			// If the current volume isn't 1.0, then we assume we're on MR2DX
			musicVolAddr = baseAddr + MR2_OFFSET;
			sfxVolAddr = baseAddr + MR2_SFX_OFFSET;
			uintptr_t defaultSfx = baseAddr + MR2_SFX_DEFAULT_OFFSET;

			WriteProcessMemory(proc, (LPVOID)defaultSfx, &config.sfxVolume, 4, &b);

			DWORD oldProtect;
			VirtualProtect((void*)hookAddr, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
			*hookAddr = 0xE9;
			*(DWORD*)(hookAddr + 1) = (DWORD)&setFMVolume - ((DWORD)hookAddr + 5);
			*(hookAddr + 5) = 0x90;
		}

		WriteProcessMemory(proc, (LPVOID)musicVolAddr, &config.musicVolume, 4, &b);
		WriteProcessMemory(proc, (LPVOID)sfxVolAddr, &config.sfxVolume, 4, &b);
	}
}
