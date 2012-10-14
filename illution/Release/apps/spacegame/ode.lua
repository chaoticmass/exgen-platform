include("lib/render3d.lua");
include("lib/libode.lua");
include("classes.lua");

areas = {};
items = {};
entities = {};

create_area(areas);


--[[
window1 = window:new();
window1.top = 0;
window1.left = 0;
window1.width = 320 + 6;
window1.height = 240 + 64;
window1.visible = 1;

button1 = button:new();
button1.parent = window1;
button1.top = window1.height - button1.height - 3;
button1.left = 3;


main = camera_class:new();
main.surface.width = 320;
main.surface.height = 240;
main.surface.top = 32;
main.surface.left = 3;
main.surface.backcolor = makecol(255, 255, 255);
main.surface.z = 9999;
main.surface.clear();
main.surface.parent = window1;
main.initialize();


ship_model = model_class:new('/root/projects/exgen/spacegame/models/planet.3ds');

ship_texture = surface:new();
ship_texture.bmp = '/root/projects/exgen/spacegame/textures/earth.bmp';

ship1 = entity_class:new();
ship1.model = ship_model;
ship1.texture = ship_texture;
ship1.x = 0;
ship1.y = 0;
ship1.z = 10;
ship1.r_x = 125;
ship1.scale = .015;

ship2 = entity_class:new();
ship2.model = ship_model;
ship2.texture = ship_texture;
ship2.x = 0;
ship2.y = .1;
ship2.z = 15;
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
body1.set_position(ship1.x, ship1.y, ship1.z);

body2 = body_class:new(ws);
body2.set_position(ship2.x, ship2.y, ship2.z);

button1.on_mouse_click = function()
    if (main.surface.visible == 0) then
	main.surface.visible = 1;
    else
	main.surface.visible = 0;
    end
end

main.loop = 0;
main.i = 0;
main.surface.on_draw = function()
    main.update();    
    
    ship1.x, ship1.y, ship1.z = body1.get_position();
    ship1.update();

    ship1.render(main);

    ship2.x, ship2.y, ship2.z = body2.get_position();
    ship2.update();
    ship2.render(main);

    main.surface.set_dirty();
    ws.simulate_step(0.005);
--    main.i = main.i + 1;
end


render_loop(window1.surface);
]];