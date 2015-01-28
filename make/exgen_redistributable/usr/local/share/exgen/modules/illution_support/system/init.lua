
mod_load("illution");

lua_init_script = [[
include("lib/gui.lua");
include("lib/term.lua");
]];
setvar("configuration", "lua-init-script", lua_init_script);
reinit();

--set_debug_mode(1);

include("system/config/system_config.lua");
include("system/config/colors.lua");
include("system/config/desktop_config.lua");

