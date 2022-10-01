#pragma once
#include <fstream>
#include <unordered_map>
#include <limits>
#include <regex>
#include <mfidl.h>
#include <wrl/client.h>

static const DWORD MR1_OFFSET = 0xE4C428;
static const DWORD MR2_OFFSET = 0x1D481D0;
// For MR2DX the value is loaded from the code every time it seems (?)
static const DWORD MR2_SFX_OFFSET = 0x1677DF;
// Additional offset for default volume that can potentially get called when triggering the same audio rapidly
static const DWORD MR2_SFX_DEFAULT_OFFSET = 0x1677FB;

struct Config {
	float sfxVolume = 1.0;
	float musicVolume = 1.0;
	float fmvVolume = 1.0;
};

static Config config;
Microsoft::WRL::ComPtr<IMFMediaSession> mediaSession;
Microsoft::WRL::ComPtr<IMFSimpleAudioVolume> simpleVolume;

static DWORD baseAddr = reinterpret_cast<DWORD>(GetModuleHandleA(NULL));
static unsigned char* hookAddr = (unsigned char*)baseAddr + 0x147B77;
static DWORD returnAddr = baseAddr + 0x147B7D;
static DWORD codeCaveOffset = baseAddr + 0x1DF0000;
