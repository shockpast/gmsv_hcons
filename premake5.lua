PROJECT_GENERATOR_VERSION = 3

newoption({
    trigger = "gmcommon",
    description = "Sets the path to the garrysmod_common (https://github.com/danielga/garrysmod_common) directory",
    default = "../garrysmod_common"
})

local gmcommon = assert(_OPTIONS.gmcommon or os.getenv("GARRYSMOD_COMMON"),
    "you didn't provide a path to your garrysmod_common (https://github.com/danielga/garrysmod_common) directory")
include(gmcommon)

CreateWorkspace({ name = "hcons", abi_compatible = false })
    CreateProject({ serverside = true, manual_files = false })
        -- Remove some or all of these includes if they're not needed
        IncludeHelpersExtended()
        IncludeLuaShared()
        IncludeSDKCommon()
        IncludeSDKTier0()
        IncludeSDKTier1()
        IncludeSDKTier3()
        IncludeDetouring()
        IncludeScanning()
        --IncludeSDKMathlib()
        --IncludeSDKRaytrace()
        --IncludeSDKBitmap()
        --IncludeSDKVTF()
        --IncludeSteamAPI()

        filter("system:windows")
            files({"source/win32/*.cpp", "source/win32/*.hpp"})

        filter("system:linux or macosx")
            files({"source/posix/*.cpp", "source/posix/*.hpp"})