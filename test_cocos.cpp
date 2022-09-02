#include "test_cocos.h"

//typedef uintptr_t Director;
//typedef uintptr_t GLView;
//typedef GLView * (__thiscall * GetOpenGLView)(Director*);
//typedef GLFWwindow * (__thiscall * GetWindowFunc)(GLView*);
//
//static GLView * glView;
//static GLFWwindow * window;
//static bool imguiInitialized = false;
//std::ofstream imguiLog("imgui.txt");

Microsoft::WRL::ComPtr<IMFMediaSession> mediaSession;
Microsoft::WRL::ComPtr<IMFSimpleAudioVolume> simpleVolume;

static Config config;

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

int setVolume()
{
	if (hasRun) {
		return 1;
	}

	hasRun = true;

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

	if (! cfgFile.good())
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

		WriteProcessMemory(proc, (LPVOID) defaultSfx, &config.sfxVolume, 4, &b);

		DWORD old_protect;
		VirtualProtect((void*) hookAddr, 6, PAGE_EXECUTE_READWRITE, &old_protect);
		* hookAddr = 0xE9;
		* (DWORD *)(hookAddr + 1) = (DWORD) &setFMVolume - ((DWORD) hookAddr + 5);
		* (hookAddr + 5) = 0x90;
	}

	WriteProcessMemory(proc, (LPVOID) musicVolAddr, &config.musicVolume, 4, &b);
	WriteProcessMemory(proc, (LPVOID) sfxVolAddr, &config.sfxVolume, 4, &b);

	return 0;
}

//void testing()
//{
//	if (window != 0)
//	{
//		return;
//	}
//
//	HMODULE cocos2d = GetModuleHandle(L"libcocos2d.dll");
//	std::ofstream logFile("log.txt");
//
//	if (cocos2d != nullptr)
//	{
//		FARPROC instanceFunc = GetProcAddress(cocos2d, (LPCSTR)"?getInstance@Director@cocos2d@@SAPAV12@XZ");
//		FARPROC getApplicationInstance = GetProcAddress(cocos2d, (LPCSTR)"?getInstance@Application@cocos2d@@SAPAV12@XZ");
//		Director* instance = (Director*) instanceFunc();
//
//		if (instance != 0)
//		{
//			if (*(instance + 34) != 0)
//			{
//				glView = (instance + 34);
//
//				GetWindowFunc getWindow = (GetWindowFunc) GetProcAddress(cocos2d, (LPCSTR)"?getWindow@GLViewImpl@cocos2d@@QBEPAUGLFWwindow@@XZ");
//				window = getWindow(glView);
//				imguiLog << "window = " << window << std::endl;
//			}
//		}
//	}
//
//	logFile.flush();
//	logFile.close();
//}
//
//static void glfw_error_callback(int error, const char* description)
//{
//	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
//}
//
//void testImgui()
//{
//	if (window == 0)
//	{
//		return;
//	}
//
//	if (imguiInitialized == false)
//	{
//		// Setup window
//		glfwSetErrorCallback(glfw_error_callback);
//		if (!glfwInit())
//			return;
//
//		glfwMakeContextCurrent(window);
//
//		// GL 3.0 + GLSL 130
//		const char* glsl_version = "#version 130";
//		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
//		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
//
//		ImGui::CreateContext();
//		ImGuiIO& io = ImGui::GetIO(); (void)io;
//		ImGui::StyleColorsDark();
//		ImGui_ImplGlfw_InitForOpenGL(window, true);
//		ImGui_ImplOpenGL3_Init(glsl_version);
//		imguiInitialized = true;
//
//	}
//
//	// Our state
//	bool show_demo_window = true;
//	bool show_another_window = true;
//	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
//
//	// Main loop
//	while (!glfwWindowShouldClose(window))
//	{
//		// Poll and handle events (inputs, window resize, etc.)
//		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
//		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
//		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
//		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
//		glfwPollEvents();
//
//		// Start the Dear ImGui frame
//		ImGui_ImplOpenGL3_NewFrame();
//		ImGui_ImplGlfw_NewFrame();
//		ImGui::NewFrame();
//
//		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
//		if (show_demo_window)
//			ImGui::ShowDemoWindow(&show_demo_window);
//		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
//		{
//			static float f = 0.0f;
//			static int counter = 0;
//
//			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
//
//			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
//			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
//			ImGui::Checkbox("Another Window", &show_another_window);
//
//			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
//			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
//
//			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
//				counter++;
//			ImGui::SameLine();
//			ImGui::Text("counter = %d", counter);
//
//			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//			ImGui::End();
//		}
//
//		// 3. Show another simple window.
//		if (show_another_window)
//		{
//			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
//			ImGui::Text("Hello from another window!");
//			if (ImGui::Button("Close Me"))
//				show_another_window = false;
//			ImGui::End();
//		}
//
//		// Rendering
//		ImGui::Render();
//		int display_w, display_h;
//		glfwGetFramebufferSize(window, &display_w, &display_h);
//		glViewport(0, 0, display_w, display_h);
//		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
//		glClear(GL_COLOR_BUFFER_BIT);
//		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//		glfwSwapBuffers(window);
//	}
//
//	imguiLog << "called" << std::endl;
//
//	imguiLog.flush();
//}
