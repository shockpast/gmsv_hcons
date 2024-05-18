#include <vector>

#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/Lua/Helpers.hpp>
#include <GarrysMod/ModuleLoader.hpp>
#include <GarrysMod/Symbol.hpp>
#include <detouring/hook.hpp>
#include <scanning/symbolfinder.hpp>

#include <main.h>

// symbols are mostly likely will work only on linux, not on windows
// windows requires knowledge of signatures/patterns
namespace symbols
{
	const Symbol ConMsgSymbol =
#ifdef SYSTEM_WINDOWS
		Symbol::FromName("?ConMsg@@YAXPBDZZ");
#elif SYSTEM_LINUX
		Symbol::FromName("_Z6ConMsgPKcz");
#endif

	const Symbol SpewMessageSymbol =
#ifdef SYSTEM_WINDOWS
		Symbol::FromSignature("\x55\x8B\xEC\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x53");
#elif SYSTEM_LINUX
		Symbol::FromName("_ZL12_SpewMessage10SpewType_tPKciPK5ColorS1_Pc");
#endif

	const Symbol IVPMessageSymbol =
#ifdef SYSTEM_WINDOWS
		Symbol::FromSignature("\x55\x8B\xEC\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x68");
#elif SYSTEM_LINUX
		Symbol::FromName("_Z11ivp_messagePKcz");
#endif
}

// bin/libtier0_srv.so
// ConMsg(const char *, ...)
typedef void (*ConMsgFn)(const tchar*, ...);
Detouring::Hook detour_ConMsg;

// ref: https://github.com/sr2echa/TF2-Source-Code/blob/c61d9750613a088617fa183a56708becfd38b81a/tf2_src/tier0/dbg.cpp#L269
// bin/libtier0_srv.so
// _SpewMessage(int, const char*, int, const Color*, const tchar, ...)
typedef void (*SpewMessageFn)(int, const char*, int, const Color*, const tchar*, ...);
Detouring::Hook detour_SpewMessage;

// ref: https://github.com/Source-Authors/source-physics/blob/94312b412f319dd3ba1ee38f6c8b540cc5e2381b/ivp_utility/ivu_string.cxx#L323-L342
// <3 @redox
// 
// bin/vphysics_srv.so
// ivp_message(const char *, ...)
typedef void (*IVPMessageFn)(const char*, ...);
Detouring::Hook detour_IVPMessage;

// this could be done much better in other ways; 
// but since i'm a failure in C++ learning, we have this.
namespace hooks
{
	SymbolFinder sym_finder;

	std::vector<std::string> bad_messages;

	static void hook_ConMsg(const char* pFormat, ...)
	{
		for (const auto& input : bad_messages)
		{
			if (pFormat == " " || pFormat == "\0" || pFormat == 0)
				break;

			if (std::string(pFormat).find(input) != std::string::npos)
				return;
		}

		va_list args;
		va_start(args, pFormat);
		vprintf(pFormat, args);
		va_end(args);

		// return detour_ConMsg.GetTrampoline<ConMsgFn>()(pFormat, args);
	}

	static void hook_SpewMessage(SpewType_t spewType, const char* pGroupName, int nLevel, const Color* pColor, const char* pFormat, ...)
	{
		for (const auto& input : bad_messages)
		{
			if (pFormat == " " || pFormat == "\0" || pFormat == 0)
				break;

			if (std::string(pFormat).find(input) != std::string::npos)
				return;
		}

		va_list args;
		va_start(args, pFormat);
		va_end(args);

		return detour_SpewMessage.GetTrampoline<SpewMessageFn>()(spewType, pGroupName, nLevel, pColor, pFormat, args);
	}

	static void hook_IVPMessage(const char* pFormat, ...)
	{
		for (const auto& input : bad_messages)
		{
			if (pFormat == " " || pFormat == "\0" || pFormat == 0)
				break;

			if (std::string(pFormat).find(input) != std::string::npos)
				return;
		}

		va_list args;
		va_start(args, pFormat);
		va_end(args);

		return detour_IVPMessage.GetTrampoline<IVPMessageFn>()(pFormat, args);
	}

	static void detour_MsgFilter_Initialize(GarrysMod::Lua::ILuaBase* LUA)
	{
#ifdef SYSTEM_WINDOWS
		SourceSDK::ModuleLoader tier0("tier0");
		SourceSDK::ModuleLoader server("server");
		SourceSDK::ModuleLoader vphysics("vphysics");
#elif SYSTEM_LINUX
		SourceSDK::ModuleLoader tier0("libtier0_srv");
		SourceSDK::ModuleLoader server("server_srv");
		SourceSDK::ModuleLoader vphysics("vphysics_srv");
#endif

		void* ConMsgPointer = nullptr;
		void* SpewMessagePointer = nullptr;
		void* IVPMessagePointer = nullptr;

		ConMsgPointer = sym_finder.Resolve(tier0.GetModule(), symbols::ConMsgSymbol.name.c_str(), symbols::ConMsgSymbol.length);
		SpewMessagePointer = sym_finder.Resolve(tier0.GetModule(), symbols::SpewMessageSymbol.name.c_str(), symbols::SpewMessageSymbol.length);
		IVPMessagePointer = sym_finder.Resolve(vphysics.GetModule(), symbols::IVPMessageSymbol.name.c_str(), symbols::IVPMessageSymbol.length);

		if (ConMsgPointer == nullptr)
			LUA->ThrowError("'ConMsg' symbol couldn't be initialized.");
		if (SpewMessagePointer == nullptr)
			LUA->ThrowError("'SpewMessagePointer' symbol couldn't be initialized.");
		if (IVPMessagePointer == nullptr)
			LUA->ThrowError("'IVPMessagePointer' symbol couldn't be initialized.");

		detour_ConMsg.Create(reinterpret_cast<void*>(ConMsgPointer), reinterpret_cast<void*>(&hook_ConMsg));
		detour_ConMsg.Enable();

		if (!detour_ConMsg.IsValid())
			LUA->ThrowError("'ConMsg' symbol was located, but hook couldn't be created/enabled.");

		detour_SpewMessage.Create(reinterpret_cast<void*>(SpewMessagePointer), reinterpret_cast<void*>(&hook_SpewMessage));
		detour_SpewMessage.Enable();

		if (!detour_SpewMessage.IsValid())
			LUA->ThrowError("'SpewMessage' symbol was located, but hook couldn't be created/enabled.");

		detour_IVPMessage.Create(reinterpret_cast<void*>(IVPMessagePointer), reinterpret_cast<void*>(&hook_IVPMessage));
		detour_IVPMessage.Enable();

		if (!detour_IVPMessage.IsValid())
			LUA->ThrowError("'IVPMessage' symbol was located, but hook couldn't be created/enabled.");
	}

	LUA_FUNCTION_STATIC(ignore_message)
	{
		bad_messages.push_back(LUA->GetString(1));
		return 0;
	}

	void Initialize(GarrysMod::Lua::ILuaBase* LUA)
	{
		detour_MsgFilter_Initialize(LUA);

		LUA->CreateTable();

		LUA->PushCFunction(ignore_message);
		LUA->SetField(-2, "ignore_message");

		LUA->SetField(-2, "msg_filter");
	}

	void Deinitialize(GarrysMod::Lua::ILuaBase* LUA)
	{
		LUA->PushNil();
		LUA->SetField(-2, "msg_filter");
	}
}