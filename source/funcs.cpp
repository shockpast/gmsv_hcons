#include <GarrysMod/FunctionPointers.hpp>

#include <networkstringtabledefs.h>

#include "funcs.hpp"

namespace funcs
{
	static const char metaname[] = "sourcesdk";
	static int32_t metatype = GarrysMod::Lua::Type::NONE;
	static const char tablename[] = "hcons_sourcesdk";

	LUA_FUNCTION_STATIC(get_model_cache)
	{
		LUA->PushNumber(global::stringtable_server->FindTable("modelprecache")->GetNumStrings());
		return 1;
	}

	void Initialize(GarrysMod::Lua::ILuaBase* LUA)
	{
		LUA->CreateTable();
		LUA->PushCFunction(get_model_cache);
		LUA->SetField(-2, "get_model_cache");
		LUA->SetField(-2, "sourcesdk");

		LUA->CreateTable();
		LUA->PushString("nothing yet here");
		LUA->SetField(-2, "_");
		LUA->SetField(-2, "network");
	}

	void Deinitialize(GarrysMod::Lua::ILuaBase* LUA)
	{
		LUA->PushNil();
		LUA->SetField(-2, "sourcesdk");
	}
}