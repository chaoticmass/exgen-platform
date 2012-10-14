
mod_load("illution");

lua_init_script = [[
include("lib/application.lua");
include("lib/gui.lua");
include("lib/term.lua");
include("lib/db.lua");
]];
setvar("configuration", "lua-init-script", lua_init_script);
reinit();

set_debug_mode(0);

initialize_video(800, 600);
--initialize_video(320, 240);
-- 34x63
--laptop
--initialize_video(1280, 800);
--initialize_video(1280, 800-56);
--initialize_video(1280, 720);

--desktop
--initialize_video(1280, 1024-56);
--initialize_video(320, 240);

--littleiron
--initialize_video(1024, 768);
--initialize_video(640,480);
--initialize_video(240, 320);

include("system/config/system_config.lua");
include("system/config/colors.lua");
include("system/config/desktop_config.lua");
--include("system/desktop.lua");
--include("system/taskbar.lua");
include("apps/fullscreen_physics_sia.lua");
--include("glowerk/glowerk.lua");

--branch("apps/dead_simple.lua");
--branch("apps/windowed_physics_sia.lua");
--branch("apps/catalog.lua");
--include("apps/title.lua");

render_loop();