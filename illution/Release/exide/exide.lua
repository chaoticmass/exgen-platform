include("lib/render3d.lua");
include("lib/libode.lua");
include("lib/latex.lua");
include("construct/sia_logo.lua");
include("construct/indiebridge_logo.lua");
include("lib/serialize.lua");


set_window_title("exIDE");

title_font = surface:new();
title_font.visible = 0;
title_font.bmp = "fonts/black_small_monospace.bmp";

body_font = surface:new();
body_font.visible = 0;
body_font.bmp = "fonts/black_micro_light.bmp";



screen = surface:new(0);
mouse = mouse_cursor:new();


bg = surface:new();
bg.width = screen_width();
bg.height = screen_height();
bg.gradient(getvar("desktop", "forecolor"), getvar("desktop", "backcolor"));
--bg.jpg = "images/r_colors1.jpg";


global_on_keypress = function(keycode, keyval)
    echo("Keycode: "..keycode..", Keyval: "..keyval.."\n");

    amount = .01;
    

    if (keycode == 58) then
	shutdown();
    end

    -- F5
    if (keycode == 51) then
    end
    -- F6
    if (keycode == 52) then
    end
    
    -- F8
    if (keycode == 54) then
    end
end

--set_global_on_keypress(global_on_keypress);



function round(num, idp)
  local mult = 10^(idp or 0)
  return math.floor(num * mult + 0.5) / mult
end


include("exide/exterm.lua");
term_window.left = 320;
term_window.top = 240;
term.write(openfile("exide/simple.lua"));

--branch("exide/animated_gif.lua");
include("apps/video.lua");
render_loop();