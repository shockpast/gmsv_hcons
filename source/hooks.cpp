#include <vector>

#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/ModuleLoader.hpp>
#include <GarrysMod/Symbol.hpp>
#include <detouring/hook.hpp>
#include <scanning/symbolfinder.hpp>

#include <main.h>

namespace symbols
{
	const Symbol ConMsgSymbol =
#ifdef SYSTEM_WINDOWS
		Symbol::FromName("?ConMsg@@YAXPBDZZ");
#elif SYSTEM_LINUX
		Symbol::FromName("_Z6ConMsgPKcz");
#endif
}

typedef void (*ConMsgFn)(char const*...);
Detouring::Hook detour_ConMsg;

// this could be done much better in other ways;
// but since i'm a failure in C++ learning, we have this.
namespace hooks
{
	SymbolFinder sym_finder;

	std::vector<std::string> bad_messages;

	static void hook_ConMsg(char const* pFormat...)
	{
		for (const auto& input : bad_messages)
		{
			size_t found = std::string(pFormat).find(input);
			if (found != std::string::npos)
				return;
		}

		va_list args;
		va_start(args, pFormat);
		vprintf(pFormat, args);
		va_end(args);
	}

	static void detour_ConMsg_Initialize(GarrysMod::Lua::ILuaBase* LUA)
	{
#ifdef SYSTEM_WINDOWS
		SourceSDK::ModuleLoader tier0("tier0");
#elif SYSTEM_LINUX
		SourceSDK::ModuleLoader tier0("libtier0");
#endif

		void* ConMsgPointer = nullptr;
		ConMsgPointer = sym_finder.Resolve(tier0.GetModule(), symbols::ConMsgSymbol.name.c_str(), symbols::ConMsgSymbol.length);

		if (ConMsgPointer == nullptr)
			LUA->ThrowError("'ConMsg' symbol couldn't be initialized.");

		detour_ConMsg.Create(reinterpret_cast<void*>(ConMsgPointer), reinterpret_cast<void*>(&hook_ConMsg));
		detour_ConMsg.Enable();

		if (!detour_ConMsg.IsValid())
			LUA->ThrowError("'ConMsg' symbol was located, but hook couldn't be created/enabled.");
	}

	LUA_FUNCTION_STATIC(conmsg_ignore_mesasge)
	{
		bad_messages.push_back(LUA->GetString(1));
		return 0;
	}

	void Initialize(GarrysMod::Lua::ILuaBase* LUA)
	{
		detour_ConMsg_Initialize(LUA);

		LUA->CreateTable();
		LUA->PushCFunction(conmsg_ignore_mesasge);
		LUA->SetField(-2, "conmsg_ignore_mesasge");
		LUA->SetField(-2, "hooks");
	}

	void Deinitialize(GarrysMod::Lua::ILuaBase* LUA)
	{
		LUA->PushNil();
		LUA->SetField(-2, "hooks");
	}
}