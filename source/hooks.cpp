#include <vector>

#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/Lua/Helpers.hpp>
#include <GarrysMod/ModuleLoader.hpp>
#include <GarrysMod/Symbol.hpp>
#include <detouring/hook.hpp>
#include <scanning/symbolfinder.hpp>

#include <inetchannel.h>
#include <netmessages.h>
#include <net.h>

#include <helpers.h>
#include <main.h>

// symbols are mostly likely will work only on linux, not on windows
// windows requires knowledge of signatures/patterns
namespace symbols
{
	const Symbol ConMsg =
#ifdef SYSTEM_WINDOWS
		Symbol::FromName("?ConMsg@@YAXPBDZZ");
#elif SYSTEM_LINUX
		Symbol::FromName("_Z6ConMsgPKcz");
#endif

	const Symbol SpewMessage =
#ifdef SYSTEM_WINDOWS
		// it doesn't work
		// https://shckpst.ru/f/SVTOqL.png
		Symbol::FromSignature("\x55\x8B\xEC\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x53");
#elif SYSTEM_LINUX
		Symbol::FromName("_ZL12_SpewMessage10SpewType_tPKciPK5ColorS1_Pc");
#endif

	const Symbol IVPMessage =
#ifdef SYSTEM_WINDOWS
		Symbol::FromSignature("\x55\x8B\xEC\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x68");
#elif SYSTEM_LINUX
		Symbol::FromName("_Z11ivp_messagePKcz");
#endif

	const Symbol ShouldHideServer =
#ifdef SYSTEM_WINDOWS
		Symbol::FromName("nah");
#elif SYSTEM_LINUX
		Symbol::FromName("_ZN14CServerGameDLL16ShouldHideServerEv");
#endif

	// unused
	const Symbol ProcessMessages =
#ifdef SYSTEM_WINDOWS
		Symbol::FromName("nah");
#elif SYSTEM_LINUX
		Symbol::FromName("_ZN8CNetChan15ProcessMessagesER7bf_read");
#endif

	const Symbol HandleLuaFileChange =
#ifdef SYSTEM_WINDOWS
		Symbol::FromName("nah");
#elif SYSTEM_LINUX
		Symbol::FromName("_ZN9GarrysMod11AutoRefresh16HandleChange_LuaERKSsS2_S2_");
#endif
}

// bin/libtier0_srv.so
// ConMsg(const char *, ...)
typedef void (*ConMsg_t)(const tchar*, ...);
Detouring::Hook detour_ConMsg;

// ref: https://github.com/sr2echa/TF2-Source-Code/blob/c61d9750613a088617fa183a56708becfd38b81a/tf2_src/tier0/dbg.cpp#L269
// bin/libtier0_srv.so
// _SpewMessage(int, const char*, int, const Color*, const tchar, ...)
typedef void (*SpewMessage_t)(int, const char*, int, const Color*, const tchar*, ...);
Detouring::Hook detour_SpewMessage;

// ref: https://github.com/Source-Authors/source-physics/blob/94312b412f319dd3ba1ee38f6c8b540cc5e2381b/ivp_utility/ivu_string.cxx#L323-L342
// <3 @redox
//
// bin/vphysics_srv.so
// ivp_message(const char *, ...)
typedef void (*IVPMessage_t)(const char*, ...);
Detouring::Hook detour_IVPMessage;

// ref: https://gmodwiki.com/Binary_Modules_Detouring_Functions
typedef bool (*ShouldHideServer_t)();
Detouring::Hook detour_ShouldHideServer;

// i'm pretty sure there a lot more arguments in it
// but i don't have much knowledge about this function/hook
// ref: https://shckpst.ru/f/2jMqh2.png
typedef void (*HandleLuaFileChange_t)(std::string*, std::string*);
Detouring::Hook detour_HandleLuaFileChange;

namespace hooks
{
	SymbolFinder sym_finder;

	std::vector<std::string> bad_messages;

	//
	void hook_ConMsg(const char* pFormat, ...)
	{
		if (helpers::FindStringInArray(pFormat, bad_messages))
			return;

		va_list args;
		va_start(args, pFormat);
		vprintf(pFormat, args);
		va_end(args);

		// return detour_ConMsg.GetTrampoline<ConMsgFn>()(pFormat, args);
	}

	void hook_SpewMessage(SpewType_t spewType, const char* pGroupName, int nLevel, const Color* pColor, const char* pFormat, ...)
	{
		if (helpers::FindStringInArray(pFormat, bad_messages))
			return;

		va_list args;
		va_start(args, pFormat);
		va_end(args);

		return detour_SpewMessage.GetTrampoline<SpewMessage_t>()(spewType, pGroupName, nLevel, pColor, pFormat, args);
	}

	void hook_IVPMessage(const char* pFormat, ...)
	{
		if (helpers::FindStringInArray(pFormat, bad_messages))
			return;

		va_list args;
		va_start(args, pFormat);
		va_end(args);

		return detour_IVPMessage.GetTrampoline<IVPMessage_t>()(pFormat, args);
	}

	bool hook_ShouldHideServer()
	{
		return variables::hide_server;
	}

	// ref: https://github.com/klen-list/gmsv_network/blob/370733d8cf2e3d8cf0013daacbb029beff804fce/source/autorefresh.cpp#L15-L32
	// similar to his implementation
	void hook_HandleLuaFileChange(std::string* filePath, std::string* fileName)
	{
		GarrysMod::Lua::ILuaBase* LUA = global::lua;

		LuaHelpers::PushHookRun(LUA, "HandleLuaFileChange");
		LUA->PushString(filePath->c_str());
		LUA->PushString(fileName->c_str());
		LuaHelpers::CallHookRun(LUA, 2, 1);

		// no autoreload this file
		if (LUA->IsType(-1, GarrysMod::Lua::Type::Bool))
		{
			LUA->Pop();
			return;
		}

		LUA->Pop();

		return detour_HandleLuaFileChange.GetTrampoline<HandleLuaFileChange_t>()(filePath, fileName);
	}

	//
	void detour_MsgFilter_Initialize(GarrysMod::Lua::ILuaBase* LUA)
	{
		helpers::CreateDetour(LUA, modules::tier0, symbols::ConMsg, &detour_ConMsg, (void*)&hook_ConMsg);
		helpers::CreateDetour(LUA, modules::tier0, symbols::SpewMessage, &detour_SpewMessage, (void*)&hook_SpewMessage);
		helpers::CreateDetour(LUA, modules::vphysics, symbols::IVPMessage, &detour_IVPMessage, (void*)&hook_IVPMessage);
	}

	void detour_ServerVisibility_Initialize(GarrysMod::Lua::ILuaBase* LUA)
	{
		helpers::CreateDetour(LUA, modules::server, symbols::ShouldHideServer, &detour_ShouldHideServer, (void*)&hook_ShouldHideServer);
	}

	void detour_HandleLuaFileChange_Initialize(GarrysMod::Lua::ILuaBase* LUA)
	{
		helpers::CreateDetour(LUA, modules::server, symbols::HandleLuaFileChange, &detour_HandleLuaFileChange, (void*)&hook_HandleLuaFileChange);
	}

	//
	LUA_FUNCTION_STATIC(IgnoreMessage)
	{
		bad_messages.push_back(LUA->GetString(1));
		return 0;
	}

	LUA_FUNCTION_STATIC(HideServer)
	{
		variables::hide_server = LUA->GetBool(1);
		return 0;
	}

	// ugly
	LUA_FUNCTION_STATIC(EnableMessageFilterHandle)
	{
		if (LUA->GetBool())
		{
			detour_ConMsg.Enable();
			detour_IVPMessage.Enable();
			detour_SpewMessage.Enable();
		}
		else
		{
			detour_ConMsg.Disable();
			detour_IVPMessage.Disable();
			detour_SpewMessage.Disable();
		}

		return 0;
	}

	LUA_FUNCTION_STATIC(EnableServerVisibilityHandle)
	{
		LUA->GetBool() ?
			detour_ShouldHideServer.Enable() :
			detour_ShouldHideServer.Disable();

		return 0;
	}

	LUA_FUNCTION_STATIC(EnableAutoRefreshHandle)
	{
		LUA->GetBool() ?
			detour_HandleLuaFileChange.Enable() :
			detour_HandleLuaFileChange.Disable();

		return 0;
	}

	void Initialize(GarrysMod::Lua::ILuaBase* LUA)
	{
		detour_MsgFilter_Initialize(LUA);
		detour_ServerVisibility_Initialize(LUA);
		detour_HandleLuaFileChange_Initialize(LUA);

		LUA->PushCFunction(IgnoreMessage);
		LUA->SetField(-2, "IgnoreMessage");

		// inspired by this page
		// https://gmodwiki.com/Binary_Modules_Detouring_Functions
		LUA->PushCFunction(HideServer);
		LUA->SetField(-2, "HideServer");

		// detour handlers
		LUA->PushCFunction(EnableMessageFilterHandle);
		LUA->SetField(-2, "EnableMessageFilterHandle");
		LUA->PushCFunction(EnableServerVisibilityHandle);
		LUA->SetField(-2, "EnableServerVisibilityHandle");
		LUA->PushCFunction(EnableAutoRefreshHandle);
		LUA->SetField(-2, "EnableAutoRefreshHandle");
	}

	void Deinitialize(GarrysMod::Lua::ILuaBase* LUA)
	{
	}
}