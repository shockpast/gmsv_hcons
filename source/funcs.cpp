#include <GarrysMod/Lua/Helpers.hpp>

#include <networkstringtabledefs.h>
#include <player.h>
#include <net.h>

#include <helpers.h>
#include <hooks.h>
#include <funcs.h>
#include <main.h>

namespace funcs
{
	LUA_FUNCTION_STATIC(GetModelCache)
	{
		LUA->PushNumber(global::stringtable->FindTable(table_modelprecache)->GetNumStrings());
		return 1;
	}

	LUA_FUNCTION_STATIC(TimeoutPlayer)
	{
		CBasePlayer* player = helpers::GetPlayer(LUA, 1);
		CNetChan* netchannel = (CNetChan*)global::engine->GetPlayerNetInfo(player->entindex() + 1);

		if (!netchannel)
			LUA->ThrowError("CNetChan doesn't exist for this player, is this a living being?");

		// https://github.com/sr2echa/TF2-Source-Code/blob/c61d9750613a088617fa183a56708becfd38b81a/tf2_src/engine/net_chan.cpp#L361-L376
		// <3 @redox
		netchannel->Shutdown(0);

		return 0;
	}

	void Initialize(GarrysMod::Lua::ILuaBase* LUA)
	{
		LUA->PushCFunction(GetModelCache);
		LUA->SetField(-2, "GetModelCache");
		LUA->PushCFunction(TimeoutPlayer);
		LUA->SetField(-2, "TimeoutPlayer");
	}

	void Deinitialize(GarrysMod::Lua::ILuaBase* LUA)
	{
	}
}