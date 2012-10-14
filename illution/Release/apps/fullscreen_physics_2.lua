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
main.surface.backcolor = makecol(0, 0, 0);
main.surface.z = 9999;
main.surface.clear();
main.surface.set_focus();
--main.surface.parent = window1;
main.initialize();

--mouse = mouse_cursor:new();
--mouse.parent = main.surface;

--ball_model = model_class:new('apps/spacegame/planet.3ds');
ball_model = model_class:new();
create_cube(ball_model, 512, 512, .5, 0, 0, 0, makecol(128,0, 0));

ball_texture = surface:new();
ball_texture.jpg = 'images/logo_512x512.jpg';
ball_texture.visible = 0;

brick_texture = surface:new();
brick_texture.jpg = 'images/concrete_512x512.jpg';
brick_texture.visible = 0;


topo_texture = surface:new();
topo_texture.jpg = 'images/earth_topo.jpg';
topo_texture.visible = 0;

earth_texture = surface:new();
earth_texture.jpg = 'images/earth.jpg';
earth_texture.visible = 0;

ball_count = 99;
ball = {};
z = 0;
for i=1,ball_count,1 do
    ball[i] = entity_class:new();
    ball[i].model = ball_model;
    ball[i].texture = brick_texture;
    ball[i].x = -55--+math.random(-5,5);
    ball[i].y = 55+(math.floor(i/5))-- + math.random(-5,5);
    if (math.mod(i,5) == 1) then
	z = 0;
    end

    z = z + 1;
    ball[i].z = 0.2+((z-1)*.65);
    ball[i].r_z = 64; --+(i*32);
    ball[i].r_y = 64;
    ball[i].r_x = 128;
    ball[i].scale = 1;
end

    i = ball_count;
    ball[i].texture = ball_texture;
    ball[i].x = -60;
    ball[i].y = 55;
    ball[i].z = 3;
    ball[i].r_z = 64; --+(i*32);
    ball[i].r_y = 64;
    ball[i].r_x = 128;
    ball[i].scale = .1;


main.x = -52.5;
main.y = 52.5;
main.z = 3;
main.pitch = 0;
main.heading = 0;

ws = worldspace_class:new();
ws.ground = ws.create_plane();

for i=1,ball_count - 1,1 do
    ball[i].body = body_class:new(ws, create_body(ws.ws, "box", .5, .5, .5, 1));
    ball[i].body.set_rotation(ball[i].r_x, ball[i].r_y, ball[i].r_z);
    ball[i].body.set_position(ball[i].x, ball[i].y, ball[i].z);
end
ball[ball_count].body = body_class:new(ws, create_body(ws.ws, "box", .05, .05, .05, 1));


fps = 120;
frame_duration = 1/fps;

main.loop = 0;
main.i = 0;
main.start = timer();

main.surface.on_keypress = function(keycode, keyval)
    move_amount = .1;
    if (keycode == 84) then
	main.x = main.x + (math.sin(math.rad(main.heading/-1)) * math.cos(math.rad(main.pitch/-1))*.1);
	main.y = main.y + (math.cos(math.rad(main.heading/-1)) * math.cos(math.rad(main.pitch/-1))*.1);
	main.z = main.z + (math.sin(math.rad(main.pitch/-1))*.1);

    elseif (keycode == 85) then
	main.x = main.x - (math.sin(math.rad(main.heading/-1)) * math.cos(math.rad(main.pitch/-1))*.1);
	main.y = main.y - (math.cos(math.rad(main.heading/-1)) * math.cos(math.rad(main.pitch/-1))*.1);
	main.z = main.z - (math.sin(math.rad(main.pitch/-1))*.1);
    elseif (keycode == 82) then
	main.x = main.x + (math.sin(math.rad((main.heading+90)/-1)) * math.cos(math.rad(main.pitch/-1))*.1);
	main.y = main.y + (math.cos(math.rad((main.heading+90)/-1)) * math.cos(math.rad(main.pitch/-1))*.1);
	main.z = main.z + (math.sin(math.rad(main.pitch/-1))*.1);
    elseif (keycode == 83) then
	main.x = main.x - (math.sin(math.rad((main.heading+90)/-1)) * math.cos(math.rad(main.pitch/-1))*.1);
	main.y = main.y - (math.cos(math.rad((main.heading+90)/-1)) * math.cos(math.rad(main.pitch/-1))*.1);
	main.z = main.z - (math.sin(math.rad(main.pitch/-1))*.1);
    elseif (keyval == 97) then -- A
	main.z = main.z + move_amount;
    elseif (keyval == 122) then -- Z
	main.z = main.z - move_amount;
    elseif (keyval == 115) then -- S
	main.fov = main.fov + move_amount;
    elseif (keyval == 120) then -- X
	main.fov = main.fov - move_amount;

    elseif (keycode == 75) then
	echo("Simulating!\n");
	ws.simulate_step(frame_duration);
    end
    echo("Main X: "..main.x..", Y: "..main.y..", Z: "..main.z.."\n");
end

last_mouse_x = screen_width() * .5;
last_mouse_y = screen_height() * .5;
mouse_handler = function()
    mouse_x, mouse_y = mouse_xy();
    mouse_l, mouse_r = mouse_lr();
    x_sensitivity = 1;
    y_sensitivity = 1;
    
    main.heading = main.heading + ((last_mouse_x - mouse_x) * x_sensitivity);
    main.pitch = main.pitch + ((last_mouse_y - mouse_y) * y_sensitivity);
    
    last_mouse_x = mouse_x;
    last_mouse_y = mouse_y;
	
    echo("X: "..(math.sin(math.rad(main.heading))).."\n");
    echo("Y: "..(math.cos(math.rad(main.heading))).."\n");
    ball[ball_count].x = main.x + ((math.sin(math.rad(main.heading/-1)) * math.cos(math.rad(main.pitch/-1))*.1) * 10);
    ball[ball_count].y = main.y + ((math.cos(math.rad(main.heading/-1)) * math.cos(math.rad(main.pitch/-1))*.1) * 10);
    ball[ball_count].z = main.z + ((math.sin(math.rad(main.pitch/-1))*.1)*10);
    ball[ball_count].r_z = 64; --+(i*32);
    ball[ball_count].r_y = 64;
    ball[ball_count].r_x = 128;
    ball[ball_count].body.set_rotation(ball[ball_count].r_x, ball[ball_count].r_y, ball[ball_count].r_z);
    ball[ball_count].body.set_position(ball[ball_count].x, ball[ball_count].y, ball[ball_count].z);
end

--[[hud = surface.new();
hud.parent = main.surface;
hud.]]
main.fps = 0;
main.surface.on_draw = function()
    main.update();    
    for i=1,ball_count,1 do
	if type(ball[i].body.body) ~= "nil" then
	    ball[i].x, ball[i].y, ball[i].z = ball[i].body.get_position();
	    ball[i].set_rotation(ball[i].body.get_rotation());
	    ball[i].update();
	    ball[i].render(main);
	end
    end

    ws.simulate_step(frame_duration);
    
    main.i = main.i + 1;
    main.surface.set_dirty();

    if (timer() - main.start > 1) then
	main.fps = main.i;
	main.i = 0;
	main.start = timer();
    end
    
    mouse_handler();
    main.surface.print(0, 0, "X: "..main.x.." Y: "..main.y.." Z: "..main.z.." Heading: "..main.heading.." Pitch: "..main.pitch);
    main.surface.print(0, 10, "FPS: "..main.fps);
end


set_root_surface(main.surface.surface);
render_loop();
