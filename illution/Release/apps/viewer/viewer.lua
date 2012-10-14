
include("lib/render3d.lua");
include("lib/libode.lua");


title_font = surface:new();
title_font.visible = 0;
title_font.bmp = "fonts/black_small_monospace.bmp";

body_font = surface:new();
body_font.visible = 0;
body_font.bmp = "fonts/black_micro_light.bmp";

pron = surface:new();
pron.jpg = "/media/disk-1/var/www/images/g/favorites/DSCN3664.JPG";
pron.visible = 1;
pron.set_focus();
pron.on_keypress = function(keycode, keyscan)
-- 89 = +
    if (keycode == 89) then
	pron.width = pron.width + 1;
	pron.height = pron.height + 1;
	
	echo(pron.width.."\n");
    elseif (keycode == 88) then
	pron.width = pron.width - 1;
	pron.height = pron.height - 1;
    elseif (keycode == 84) then
	pron.top = pron.top + 10;
    elseif (keycode == 85) then
	pron.top = pron.top - 10;
    elseif (keycode == 82) then
	pron.left = pron.left + 10;
    elseif (keycode == 83) then
	pron.left = pron.left - 10;

    end
    
-- 88 = -
end

render_loop();
