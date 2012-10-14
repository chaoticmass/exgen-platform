
mod_load("illution");

lua_init_script = [[
include("lib/application.lua");
include("lib/surface.lua");
include("lib/term.lua");
include("lib/db.lua");
]];
setvar("configuration", "lua-init-script", lua_init_script);
reinit();

set_debug_mode(0);

initialize_video(640, 480);

include("system/config/system_config.lua");
include("system/config/colors.lua");
mouse = surface:new();
mouse.top = 0;
mouse.left = 0;
mouse.width = 16;
mouse.height = 16;
mouse.transparent = 1;
mouse.backcolor = makecol(255, 0, 255);
mouse.forecolor = makecol(0, 0, 0);
mouse.clear();
mouse.always_on_top = 1;
--mouse.print(0,0,"X");
mouse.bmp = "system/images/cursor.bmp";
mouse.z = 9999;


mouse.on_draw = function()
    x, y = mouse_xy();
    --echo("mouse on draw\n");
    mouse.top = y;
    mouse.left = x;
--    mouse.z = 9999;    
end
--[[
fader = surface:new();
fader.top = 0;
fader.left = 0;
fader.width = screen_width();
fader.height = screen_height();
fader.z = 9999;
fader.always_on_top = 1;
fader.background = makecol(255, 255, 0);
fader.clear();
fader.alpha = 0;
fader.tick = timer();

fader.on_draw = function()
    if (fader.alpha < 255) then
	if (fader.tick ~= 0) then
	    fader.alpha = fader.alpha + (256 * (timer() - fader.tick));
	else
	    fader.alpha = fader.alpha;
	end	
	fader.tick = timer();
	--echo(fader.tick.." "..fader.alpha.."\n");
    else
	fader.visible = 0;
    end
end
]]
--include("system/config/desktop_config.lua");
run_script_thread_reuse("system/taskbar.lua");
run_script_thread_reuse("system/desktop.lua");
--run_script_thread_reuse("apps/form_client.lua");
--run_script_thread_reuse("apps/inventory_search.lua");
run_script_thread_reuse("apps/ode.lua");


render_loop();
