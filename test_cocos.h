#pragma once
#include <Windows.h>
#include <string>
#include <fstream>
#include <regex>
//#include "imgui/imgui.h"
//#include "imgui/backends/imgui_impl_glfw.h"
//#include "imgui/backends/imgui_impl_opengl3.h"
//#include <glfw3.h>
#include <limits>
#define NOBITMAP
#include <mfidl.h>
#include <wrl/client.h>
#include <unordered_map>

static bool hasRun = false;

static const uintptr_t MR1_OFFSET = 0xE4C428;
static const uintptr_t MR2_OFFSET = 0x1D481D0;
// For MR2DX the value is loaded from the code every time it seems (?)
static const uintptr_t MR2_SFX_OFFSET = 0x1677DF;
// Additional offset for default volume that can potentially get called when triggering the same audio rapidly
static const uintptr_t MR2_SFX_DEFAULT_OFFSET = 0x1677FB;

struct Config {
	float sfxVolume = 1.0;
	float musicVolume = 1.0;
	float fmvVolume = 1.0;
};

static uintptr_t baseAddr = reinterpret_cast<uintptr_t>(GetModuleHandleA(NULL));
static unsigned char* hookAddr = (unsigned char*) baseAddr + 0x147B77;
static DWORD returnAddr = baseAddr + 0x147B7D;
static DWORD codeCaveOffset = baseAddr + 0x1DF0000;

int setVolume();
//void testing();
//void testImgui();

