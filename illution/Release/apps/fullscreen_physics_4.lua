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

ball_count = 2;
ball = {};
z = 0;
for i=1,ball_count,1 do
    ball[i] = entity_class:new();
    --ball[i].model = ball_model;
    ball[i].texture = brick_texture;

    ball[i].r_z = 64; --+(i*32);
    ball[i].r_y = 64;
    ball[i].r_x = 128;
    ball[i].scale = 1;
end

scale = 1;
solar_mass_unit = 1/scale; --(1/ ( (3.7*math.pow(10,14)) *8 )); -- kg to solar_mass_unit
solar_scale = 1/scale; -- 1/100000; -- km to solar_scale;

echo("Mass of the Earth: "..((5.972*math.pow(10,24))*solar_mass_unit).."\n");

earth_model = model_class:new();
create_cube(earth_model, 512, 512, 636700.5*solar_scale, 0, 0, 0, makecol(0,128, 0));
ball[1].model = earth_model;
ball[1].x = 0;
ball[1].y = (1.479*math.pow(10,8))*solar_scale;
ball[1].z = 0;

sun_model = model_class:new();
create_cube(sun_model, 512, 512, 6955000*solar_scale, 0, 0, 0, makecol(128,0, 0));
ball[2].model = sun_model;
ball[2].x = 0;
ball[2].y = 0;
ball[2].z = 0;


main.x = 0-((3*math.pow(10, 8))*solar_scale);
main.y = 0;
main.z = 0;
main.pitch = 0;
main.heading = -90;

ws = worldspace_class:new(0);
--ws.ground = ws.create_plane();

ball[1].body = body_class:new(ws, create_body(ws.ws, "box", 6367.5*solar_scale, 6367.5*solar_scale, 6367.5*solar_scale, (5.972*math.pow(10,24))*solar_mass_unit));
ball[1].body.set_rotation(ball[1].r_x, ball[1].r_y, ball[1].r_z);
ball[1].body.set_position(ball[1].x, ball[1].y, ball[1].z);

ball[2].body = body_class:new(ws, create_body(ws.ws, "box", 6955000*solar_scale, 6955000*solar_scale, 6955000*solar_scale, (1.988*math.pow(10,30))*solar_mass_unit));
ball[2].body.set_rotation(ball[2].r_x, ball[2].r_y, ball[2].r_z);
ball[2].body.set_position(ball[2].x, ball[2].y, ball[2].z);


fps = 33;
frame_duration = 4;

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
--	echo("Simulating!\n");
	ws.simulate_step(frame_duration);
    elseif (keyval == 101) then
--[[	echo("X: "..(math.sin(math.rad(main.heading))).."\n");
        echo("Y: "..(math.cos(math.rad(main.heading))).."\n");
	ball[ball_count].x = main.x + ((math.sin(math.rad(main.heading/-1)) * math.cos(math.rad(main.pitch/-1))*.1) * 10);
        ball[ball_count].y = main.y + ((math.cos(math.rad(main.heading/-1)) * math.cos(math.rad(main.pitch/-1))*.1) * 10);
        ball[ball_count].z = main.z + ((math.sin(math.rad(main.pitch/-1))*.1)*10);
        ball[ball_count].r_z = 64; --+(i*32);
        ball[ball_count].r_y = 64;
        ball[ball_count].r_x = 128;
        ball[ball_count].body.set_rotation(ball[ball_count].r_x, ball[ball_count].r_y, ball[ball_count].r_z);
        ball[ball_count].body.set_position(ball[ball_count].x, ball[ball_count].y, ball[ball_count].z);
        ball[ball_count].body.set_force(0, 0, 0);]]
    end
--    echo("Main X: "..main.x..", Y: "..main.y..", Z: "..main.z.."\n");
end

last_mouse_x = screen_width() * .5;
last_mouse_y = screen_height() * .5;
mouse_handler = function()
    mouse_x, mouse_y = mouse_xy();
    mouse_l, mouse_r = mouse_lr();
    x_sensitivity = 1;
    y_sensitivity = 1;
    
--    main.heading = main.heading + ((last_mouse_x - mouse_x) * x_sensitivity);
--    main.pitch = main.pitch + ((last_mouse_y - mouse_y) * y_sensitivity);
    
    last_mouse_x = mouse_x;
    last_mouse_y = mouse_y;

    if (mouse_r == 1) then
--	ball[1].body.add_force(0, 0, 100000);
    end
    
    if (mouse_l == 1) then
    end
end

--[[hud = surface.new();
hud.parent = main.surface;
hud.]]
main.fps = 0;
main.enable_gravity_test = 1;
push = (ball[1].body.get_mass()*500000) / frame_duration;
ball[1].body.add_force(0, 0-(push), push);
days = 0;
main.frames = 0;
main.surface.on_draw = function()
    main.frames = main.frames + 1;
    main.update();    
    b1x, b1y, b1z = ball[1].body.get_position();
    b2x, b2y, b2z = ball[2].body.get_position();

    for i=1,ball_count,1 do
	if type(ball[i].body.body) ~= "nil" then
	    ball[i].x, ball[i].y, ball[i].z = ball[i].body.get_position();
	    ball[i].set_rotation(ball[i].body.get_rotation());
	    ball[i].update();
	    ball[i].render_gouraud(main);
	end
    end
    if (main.enable_gravity_test == 1) then
	b1x, b1y, b1z = ball[1].body.get_position();
	b2x, b2y, b2z = ball[2].body.get_position();
	m1 = ball[1].body.get_mass();
	m2 = ball[2].body.get_mass();

	gx = b1x - b2x;
	gy = b1y - b2y;
	gz = b1z - b2z;
	gd = math.sqrt((gx*gx) + (gy*gy) + (gz*gz));
	gc = 6.674 * (10^-11);
	gf = ((gc*m1*m2)/math.pow(gd,2))*(frame_duration);
	gx, gy, gz = normalize_vector(gx, gy, gz);
	gx = gx * gf;
	gy = gy * gf;
	gz = gz * gf;
	ball[1].body.add_force(gx/-1, gy/-1, gz/-1);
	ball[2].body.add_force(gx, gy, gz);
	echo("b1y: "..b1y.." b1z: "..b1z.."\n");
	echo("GF: "..gf.." GD: "..gd.."\n");
    end

--    if (days < 365) then
	ws.simulate_step(frame_duration);
--    end

    
    
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
    main.surface.print(0, 20, "Time: "..(main.frames*frame_duration).." seconds.");
end


--include("apps/
--set_root_surface(main.surface.surface);
--main.surface.transparent = 1;
--main.surface.backcolor = makecol(255, 0, 255);
render_loop();
