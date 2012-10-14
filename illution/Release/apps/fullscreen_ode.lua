--[[

shows how to create a fullscreen application for the best graphic performance

]]

include("lib/render3d.lua");
include("lib/libode.lua");




main = camera_class:new();
main.surface.width = screen_width();
main.surface.height = screen_height();
main.surface.top = 0;
main.surface.left = 0;
main.surface.backcolor = makecol(255, 255, 255);
main.surface.z = 9999;
main.surface.clear();
main.surface.set_focus();
--main.surface.parent = window1;
main.initialize();

--[[
window1 = window:new();
window1.top = 0;
window1.left = 0;
window1.width = 320 + 6;
window1.height = 140 + 64;
window1.visible = 1;
window1.parent = main.surface;
echo(window1.surface.." window1 \n");
]]

button1 = button:new();
button1.parent = main.surface;
button1.top = 0;
button1.left = 0;

button1.on_click = function() 
    main.surface.destroy();
end

ship_model = model_class:new('/root/projects/exgen/spacegame/models/planet.3ds');

ship_texture = surface:new();
ship_texture.bmp = '/root/projects/exgen/spacegame/textures/earth.bmp';
--ship_texture.gif = '/root/latex/Talk/main_document1.gif';
ship_texture.visible = 0;

ship1 = entity_class:new();
--ship1 = surface3d_class:new();
ship1.model = ship_model;
ship1.texture = ship_texture;
ship1.x = 0;
ship1.y = 0;
ship1.z = 10;
ship1.r_x = 64;
ship1.r_y = 64;
ship1.r_z = 128;
ship1.scale = .015;

ship2 = entity_class:new();
ship2.model = ship_model;
ship2.texture = ship_texture;
ship2.x = 0;
ship2.y = .1;
ship2.z = 15;
ship2.r_y = 0;
ship2.r_z = 0;
ship2.r_x = 125;
ship2.scale = .015;


main.x = -15;
main.y = 0;
main.z = 10;
main.pitch = 0;
main.heading = 0;



ws = worldspace_class:new();
ws.ground = ws.create_plane();

body1 = body_class:new(ws);
body1.set_rotation(ship1.r_x/255, ship1.r_y/255, ship1.r_z/255);
body1.set_position(ship1.x, ship1.y, ship1.z);


body2 = body_class:new(ws);
body2.set_rotation(ship2.r_x/255, ship2.r_y/255, ship2.r_z/255);
body2.set_position(ship2.x, ship2.y, ship2.z);


main.loop = 0;
main.i = 0;
main.start = timer();
main.surface.on_draw = function()
    if (main.i < 3000) then
    main.update();    
    
    ship1.x, ship1.y, ship1.z = body1.get_position();
    ship1.set_rotation(body1.get_rotation());
    
    ship1.update();

    ship1.render(main);

    ship2.x, ship2.y, ship2.z = body2.get_position();
    ship2.set_rotation(body2.get_rotation());
    ship2.update();
    ship2.render(main);

    
    ws.simulate_step(0.005);
    main.i = main.i + 1;
    
    
    main.surface.set_dirty();
    else 
	echo(timer() - main.start.."\n");
    end
end


set_root_surface(main.surface.surface);
render_loop(main.surface.surface);
