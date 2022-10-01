#include "loader.h"

int loadDlls()
{
	if (hasRun) {
		return 1;
	}

	hasRun = true;
	const std::filesystem::path modsPath("./mods/");
	std::regex isDll("\\.dll$", std::regex_constants::ECMAScript | std::regex_constants::icase);

	for (const auto& entry : std::filesystem::recursive_directory_iterator(modsPath))
	{
		const std::string path = entry.path().string();
		if (std::regex_search(path, isDll))
		{
			logFile << "Loading " << path << std::endl;

			HMODULE lib = LoadLibrary(entry.path().c_str());
			if (lib != NULL)
			{
				const auto initFunc = GetProcAddress(lib, "Initialize");
				const auto initEarlyFunc = GetProcAddress(lib, "InitializeEarly");

				if (initFunc != 0)
				{
					init.insert({ entry.path().string(), initFunc });
				}

				if (initEarlyFunc != 0)
				{
					initEarly.insert({ entry.path().string(), initEarlyFunc });
				}
			}
			else
			{
				logFile << "Failed to load " << path << std::endl;
			}
		}
	}

	logFile.flush();

	return 0;
}

int callInitializeEarly()
{
	if (hasInitEarly == true)
	{
		return 1;
	}

	hasInitEarly = true;

	callFuncs("init early", initEarly);

	return 0;
}

int callInitialize()
{
	if (hasInit == true)
	{
		return 1;
	}

	hasInit = true;

	callFuncs("init", init);

	return 0;
}

void callFuncs(std::string type, std::unordered_map<std::string, FARPROC> entries)
{
	for (const auto funcEntry : entries)
	{
		logFile << "Calling " << type <<  " from: " << funcEntry.first << std::endl;
		funcEntry.second();
	}

	logFile.flush();
}
