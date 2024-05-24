#include <vector>
#include <string>

#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/Symbol.hpp>
#include <GarrysMod/ModuleLoader.hpp>
#include <scanning/symbolfinder.hpp>
#include <detouring/hook.hpp>

#include <player.h>

namespace helpers
{
	SymbolFinder sym_finder;

	bool FindStringInArray(const char* input, std::vector<std::string> array)
	{
		for (const auto& array_input : array)
		{
			if (input == 0)
				return false;

			if (std::string(input).find(array_input) != std::string::npos)
				return true;
		}

		return false;
	}

	// took an idea for these helper functions from this wholesome guy
	// https://github.com/RaphaelIT7/gmod-vprof/blob/main/source/detours.cpp
	// 
	// LUA can be removed to make arguments less overloaded;
	// but it will also remove the ability to log error
	void* GetFunction(GarrysMod::Lua::ILuaBase* LUA, SourceSDK::ModuleLoader module, Symbol symbol)
	{
		void* ptr = nullptr;
		ptr = sym_finder.Resolve(module.GetModule(), symbol.name.c_str(), symbol.length);

		if (ptr == nullptr)
			LUA->ThrowError(strcat((char*)symbol.name.c_str(), " couldn't be retrieved."));

		return ptr;
	}

	// same as GetFunction comment
	void CreateDetour(GarrysMod::Lua::ILuaBase* LUA, SourceSDK::ModuleLoader module, Symbol symbol, Detouring::Hook* detour, void* hook)
	{
		if (!module.IsValid())
			LUA->ThrowError("Module is not valid.");

		void* fn = sym_finder.Resolve(module.GetModule(), symbol.name.c_str(), symbol.length);
		if (!fn)	
			LUA->ThrowError(strcat((char*)symbol.name.c_str(), " couldn't be found in presented module."));

		detour->Create(fn, hook);
		detour->Enable();

		if (!detour->IsValid())
			LUA->ThrowError(strcat((char*)symbol.name.c_str(), " detour couldn't be initialized."));
	}

	CBasePlayer* GetPlayer(GarrysMod::Lua::ILuaBase* LUA, int index)
	{
		return (CBasePlayer*)LUA->GetUserType<CBasePlayer*>(index, GarrysMod::Lua::Type::Entity);
	}
}