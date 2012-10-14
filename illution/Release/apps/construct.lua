--[[

shows how to create a fullscreen application for the best graphic performance

]]

include("lib/render3d.lua");
include("lib/libode.lua");


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
main.surface.backcolor = makecol(255, 255, 255);
main.surface.z = 9999;
main.surface.clear();
main.surface.set_focus();
main.initialize();


mouse = mouse_cursor:new();

model = {};
model[1] = model_class:new("apps/spaceship1.3ds");
model[2] = model_class:new("apps/spacegame/planet.3ds");

entity_texture = surface:new();
entity_texture.visible = 0;
entity_texture.bmp = "apps/spaceship1.bmp";

entity_count = 1;
entity = {};
for i=1,entity_count,1 do
    entity[i] = entity_class:new();
    entity[i].model = model[1];
    entity[i].texture = entity_texture;
    entity[i].x = 0;
    entity[i].y = 0;
    entity[i].z = 10;
    entity[i].r_x = 64;
    entity[i].r_y = 64;
    entity[i].r_z = 128;
    entity[i].scale = 1.5;
end

main.x = -250;
main.y = 0;
main.z = 10;
main.pitch = -15;
main.heading = 0;

ws = worldspace_class:new();
ws.ground = ws.create_plane();

for i=1,entity_count,1 do
    entity[i].body = body_class:new(ws);
    entity[i].body.set_rotation(entity[i].r_x, entity[i].r_y, entity[i].r_z);
    entity[i].body.set_position(entity[i].x, entity[i].y, entity[i].z);
end

fps = 60;
frame_duration = 1/fps;

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

    if (keycode == 47) then 
	if (model_window.visible == 0) then
	    model_window.visible = 1;
	else 
	    model_window.visible = 0;
	end
    end

    if (keycode == 48) then 
	if (prop_window.visible == 0) then
	    prop_window.visible = 1;
	else 
	    prop_window.visible = 0;
	end
    end

    if (keycode == 49) then 
	if (anim_window.visible == 0) then
	    anim_window.visible = 1;
	else 
	    anim_window.visible = 0;
	end
    end


    if (keycode == 50) then 
	if (term_window.visible == 0) then
	    term_window.visible = 1;
	else 
	    term_window.visible = 0;
	end
    end

end

set_global_on_keypress(global_on_keypress);

main.surface.on_draw = function()

    main.update();    
    
    for i=1,entity_count,1 do
	if type(entity[i].body.body) ~= "nil" then
	    entity[i].x, entity[i].y, entity[i].z = entity[i].body.get_position();
	    --entity[i].set_rotation(entity[i].body.get_rotation());
	    entity[i].update();
	    entity[i].render(main);
	end
    end
--    entity[1].r_y = entity[1].r_y + 1;
--    model_ry_input.text = entity[1].r_y;
--    ws.simulate_step(frame_duration);
    
    main.i = main.i + 1;
    
    
    main.surface.set_dirty();
--    anim_window.rotation = anim_window.rotation + 1;
    --main.surface.save_bmp('/root/projects/c/illution/Release/apps/frames/frame_'.. (1000 + main.i) ..'.bmp');
end


--set_root_surface(main.surface.surface);
include("apps/model_selector.lua");
include("apps/model_properties.lua");
include("apps/exterm.lua");
include("apps/animated_gif.lua");

anim_window.visible = 0;
term_window.visible = 0;
prop_window.visible = 0;
model_window.visible = 0;

render_loop();
