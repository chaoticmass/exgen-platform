screen = surface:new(0);
screen.on_keypress = function(keyval, keycode) 
    if (keyval == 60) then
	echo("Root Surface = "..get_root_surface().."\n");
	if term.active ~= 1 then
	    term.active = 1;
	    intro2.visible = 1;
	    old_root = get_root_surface();
--	    intro2.alpha = 0;
	    set_root_surface(new_root); --intro2.surface);
	    intro2.z = 9999;
	    term_window.z = intro2.z + 1;
	    term.set_focus();
	else
	    term.active = 0;
	    set_root_surface(old_root);
	    intro2.visible = 0;
	    intro2.z = -9999;
	    term_window.z = intro2.z + 1;

	end
    end
end

mouse = mouse_cursor:new();
--mouse.visible = 0;
new_root = get_root_surface();
old_root = new_root;

title_font = surface:new();
title_font.visible = 0;
title_font.bmp = "fonts/black_small_monospace.bmp";

body_font = surface:new();
body_font.visible = 0;
body_font.bmp = "fonts/black_micro_light.bmp";

include("system/exterm.lua");
term.active = 1;
term_window.left = (screen_width() * 0.5) - (term_window.width * 0.5);
term_window.top = screen_height() - term_window.height; --image2.top + image2.height + (image2.text_height()*2);

intro1 = surface:new();
intro1.font = title_font;
intro1.width = screen_width();
intro1.height = screen_height();
intro1.backcolor = makecol(0, 0, 0);
intro1.clear();
intro1.z = 9999;

image1 = surface:new();
image1.gif = "images/exgen_logo_i.gif";
image1.parent = intro1;
image1.top = term_window.top - image1.height - (image1.text_height()*2); --(screen_height() * 0.5) - (image1.height * 1);
image1.left = (screen_width() * 0.5) - (image1.width * 0.5);

intro1.print(image1.left, image1.top + image1.height, "exgen version "..getvar("configuration", "version"));


intro2 = surface:new();
intro2.font = title_font;
intro2.width = screen_width();
intro2.height = screen_height();
intro2.backcolor = makecol(0,0,0);
intro2.clear();
intro2.z = 9999;

image2 = surface:new();
image2.gif = "images/illution_logo_i.gif";
image2.parent = intro2;
image2.top = term_window.top - image2.height - (image2.text_height()*2); --(screen_height() * 0.5) - (image2.height * 1);
image2.left = (screen_width() * 0.5) - (image2.width * 0.5);
intro2.print(image2.left, image2.top + image2.height, "illution version "..getvar("configuration", "illution_version"));



intro2.visible = 1;
intro1.z = intro2.z + 1;
render();
--[[for i = 0, 255, 0.5 do
    render();
end]]
--term.write("include(\"apps/fullscreen_physics_sia.lua\");");
--term.write("include(\"apps/dead_simple.lua\");");


for i = 0, 255, 4 do
    intro1.alpha = i;
    render();
end
intro1.visible = 0;

term_window.z = 9999;
term.write("-- illution ("..getvar("configuration", "illution_version")..") ready.\n");


branch(".illutionrc");
render_loop();
--term:after_keypress(51);


intro_fadeout = function()
    intro1.visible = 0;
    intro2.z = 9999;
--    term_window.visible = 0;
    for i = 0, 255, 16 do
	intro2.alpha = i;
	render();
    end
    
    intro2.visible = 0;
--    intro1.destroy();
--    intro2.destroy();
end

render();
