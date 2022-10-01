#pragma once
#include <Windows.h>
#include <regex>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <unordered_map>

static bool hasRun = false;
static bool hasInit = false;
static bool hasInitEarly = false;
static std::ofstream logFile("loader.log");
static std::unordered_map<std::string, FARPROC> init;
static std::unordered_map<std::string, FARPROC> initEarly;

int loadDlls();
int callInitializeEarly();
int callInitialize();
void callFuncs(std:: string, std::unordered_map<std::string, FARPROC>);
