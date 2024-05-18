#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/FactoryLoader.hpp>
#include <GarrysMod/InterfacePointers.hpp>
#include <GarrysMod/FunctionPointers.hpp>

#include <eiface.h>
#include <networkstringtabledefs.h>

#include <funcs.h>
#include <hooks.h>

namespace global {
	GarrysMod::Lua::ILuaBase* lua = nullptr;

	INetworkStringTableContainer* stringtable = nullptr;
	IVEngineServer* engine = nullptr;

	static void Initialize(GarrysMod::Lua::ILuaBase* LUA)
	{
		LUA->CreateTable();
		LUA->PushString("05/16/24");
		LUA->SetField(-2, "version");
		LUA->Push(-1);
		LUA->SetField(GarrysMod::Lua::INDEX_GLOBAL, "hcons");
	}

	static void Deinitialize(GarrysMod::Lua::ILuaBase* LUA)
	{
		LUA->PushNil();
		LUA->SetField(GarrysMod::Lua::INDEX_GLOBAL, "hcons");
	}
}

GMOD_MODULE_OPEN()
{
	global::lua = LUA;

	global::stringtable = InterfacePointers::NetworkStringTableContainer();
	if (global::stringtable == nullptr)
		LUA->ThrowError("Couldn't retrieve 'NetworkStringTableContainer' interface!");

	global::engine = InterfacePointers::VEngineServer();
	if (global::engine == nullptr)
		LUA->ThrowError("Couldn't retrieve 'EngineServer' interface!");

	global::Initialize(LUA);
	funcs::Initialize(LUA);
	hooks::Initialize(LUA);

	return 0;
}

GMOD_MODULE_CLOSE()
{
	funcs::Deinitialize(LUA);
	hooks::Deinitialize(LUA);
	global::Deinitialize(LUA);

	return 0;
}