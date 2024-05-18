#include <GarrysMod/Lua/Helpers.hpp>

#include <networkstringtabledefs.h>

#include <funcs.h>
#include <main.h>

namespace funcs
{
	LUA_FUNCTION_STATIC(get_model_cache)
	{
		LUA->PushNumber(global::stringtable->FindTable("modelprecache")->GetNumStrings());
		return 1;
	}

	void Initialize(GarrysMod::Lua::ILuaBase* LUA)
	{
		LUA->CreateTable();
		LUA->PushCFunction(get_model_cache);
		LUA->SetField(-2, "get_model_cache");
		LUA->SetField(-2, "sourcesdk");
	}

	void Deinitialize(GarrysMod::Lua::ILuaBase* LUA)
	{
		LUA->PushNil();
		LUA->SetField(-2, "sourcesdk");
	}
}