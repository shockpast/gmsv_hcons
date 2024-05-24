#ifdef _WIN32
#pragma once
#endif

#include <GarrysMod/Lua/Interface.h>

namespace funcs
{
	void Initialize(GarrysMod::Lua::ILuaBase* LUA);

	void Deinitialize(GarrysMod::Lua::ILuaBase* LUA);
}