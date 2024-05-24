#ifdef _WIN32
#pragma once
#endif

#include <vector>
#include <string>

#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/Symbol.hpp>

namespace symbols
{
	const Symbol ConMsg;
	const Symbol SpewMessage;
	const Symbol IVPMessage;
	const Symbol ShouldHideServer;
	const Symbol ProcessMessages;
	const Symbol HandleLuaFileChange;
}

namespace hooks
{
	extern std::vector<std::string> conmsg_blacklist;

	void Initialize(GarrysMod::Lua::ILuaBase* LUA);

	void Deinitialize(GarrysMod::Lua::ILuaBase* LUA);
}