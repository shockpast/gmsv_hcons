#ifdef _WIN32
#pragma once
#endif

#include <vector>
#include <string>

#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/Symbol.hpp>
#include <GarrysMod/ModuleLoader.hpp>
#include <detouring/hook.hpp>

#include <player.h>

namespace helpers
{
	bool FindStringInArray(const char* input, std::vector<std::string> array);
	void CreateDetour(GarrysMod::Lua::ILuaBase* LUA, SourceSDK::ModuleLoader module, Symbol symbol, Detouring::Hook* detour, void* hook);
	void* GetFunction(GarrysMod::Lua::ILuaBase* LUA, SourceSDK::ModuleLoader module, Symbol symbol);
	CBasePlayer* GetPlayer(GarrysMod::Lua::ILuaBase* LUA, int index);
}