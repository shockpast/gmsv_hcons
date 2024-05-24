#ifdef _WIN32
#pragma once
#endif

#include <GarrysMod/ModuleLoader.hpp>
#include <GarrysMod/FactoryLoader.hpp>
#include <GarrysMod/FunctionPointers.hpp>
#include <GarrysMod/InterfacePointers.hpp>
#include <GarrysMod/Lua/Interface.h>

#include "eiface.h"

// todo: put it in another file
#define table_downloadables "downloadables"
#define table_modelprecache "modelprecache"
#define table_genericprecache "genericprecache"
#define table_soundprecache "soundprecache"
#define table_decalprecache "decalprecache"

namespace GarrysMod
{
	namespace Lua
	{
		class ILuaBase;
	}
}

namespace global
{
	extern GarrysMod::Lua::ILuaBase* lua;

	extern INetworkStringTableContainer* stringtable;
	extern IVEngineServer* engine;
}

namespace modules
{
	extern SourceSDK::ModuleLoader tier0;
	extern SourceSDK::ModuleLoader vphysics;
	extern SourceSDK::ModuleLoader server;
	extern SourceSDK::ModuleLoader engine;
}

namespace variables
{
	extern bool hide_server;
}