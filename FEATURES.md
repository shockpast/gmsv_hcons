# hcons.*
It's just a superior table for other tables or properties, that will be contained in there, nothing to see there~

## :version[]
[gmsv_hcons_cpp](https://github.com/shockpast/gmsv_hcons_cpp)'s Version formatted DD/MM/YY, it could be inconsistent and not up to date always, added for debugging and solving issues easier in future.

# hcons.sourcesdk.*
SourceSDK prop(s) and function(s) exported to Lua environment.

## :get_model_cache()
Amount of Models cached on server (4096 is maximum, going above will crash server without warnings), this function can be used for detecting crazy amount of models cached in memory and ask users to restart server to clear it.