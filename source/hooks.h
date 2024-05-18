#pragma once

#include <vector>
#include <string>

#include <GarrysMod/Lua/Interface.h>

namespace hooks {
	std::vector<std::string> conmsg_blacklist;

	void Initialize(GarrysMod::Lua::ILuaBase* LUA);
	
	void Deinitialize(GarrysMod::Lua::ILuaBase* LUA);
}