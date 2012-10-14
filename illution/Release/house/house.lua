include("lib/render3d.lua");

title_font = surface:new();
title_font.visible = 0;
title_font.bmp = "fonts/black_small_monospace.bmp";

body_font = surface:new();
body_font.visible = 0;
body_font.bmp = "fonts/black_micro_light.bmp";


main = camera_class:new();
main.surface.width = screen_width();
main.surface.height = screen_height();
main.surface.top = 0;
main.surface.left = 0;
main.surface.backcolor = makecol(128, 128, 128);
main.surface.z = 9999;
main.surface.clear();
main.surface.set_focus();
main.initialize();


mouse = mouse_cursor:new();

main.x = 0;
main.y = 0;
main.z = 0;
main.pitch = 0;
main.heading = 0;


main.loop = 0;
main.i = 0;
main.start = timer();

global_on_keypress = function(keycode, keyval)
    echo(keycode.."\n");
    if (keycode == 84) then
	main.x = main.x - 1;
    elseif (keycode == 85) then
	main.x = main.x + 1;
    end

    if (keycode == 82) then
	main.y = main.y - 1;
    elseif (keycode == 83) then
	main.y = main.y + 1;
    end
    if (keycode == 1) then
	main.z = main.z - 1;
    elseif (keycode == 26) then
	main.z = main.z + 1;
    end
end

set_global_on_keypress(global_on_keypress);

main.surface.on_draw = function()

    main.update();    
    
    main.surface.set_dirty();
end

interactive_console();

render_loop();
