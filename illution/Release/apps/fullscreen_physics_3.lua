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

mouse = mouse_cursor:new();
mouse.visible = 0;
mouse.parent = main.surface;

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
    ball[i].z = 0.5+((z-1)*.65);
    ball[i].r_z = 64; --+(i*32);
    ball[i].r_y = 64;
    ball[i].r_x = 128;
    ball[i].scale = 1;
end
    
    i = ball_count;
    ball[i].texture = ball_texture;
    ball[i].x = -55;
    ball[i].y = 46;
    ball[i].z = 1.5;
    ball[i].r_z = 64; --+(i*32);
    ball[i].r_y = 64;
    ball[i].r_x = 128;
    ball[i].scale = .1;


main.x = -52.5;
main.y = 52.5;
main.z = .5;
main.pitch = 0;
main.heading = 0;


ws = worldspace_class:new();
ws.ground = ws.create_plane();

for i=1,ball_count-1,1 do
    ball[i].body = body_class:new(ws, create_body(ws.ws, "box", .5, .5, .5, 1));
    ball[i].body.set_rotation(ball[i].r_x, ball[i].r_y, ball[i].r_z);
    ball[i].body.set_position(ball[i].x, ball[i].y, ball[i].z);
end
ball[ball_count].body = body_class:new(ws, create_body(ws.ws, "box", .05, .05, .05, 1));
ball[ball_count].body.set_rotation(ball[ball_count].r_x, ball[ball_count].r_y, ball[ball_count].r_z);
ball[ball_count].body.set_position(ball[ball_count].x, ball[ball_count].y, ball[ball_count].z);

player = entity_class:new();
player.model = ball_model;
player.x = main.x;
player.y = main.y;
player.z = main.z;
player.scale = .1;
player.body = body_class:new(ws, create_body(ws.ws, "sphere", .2, .1));
player.body.set_rotation(player.r_x, player.r_y, player.r_z);
player.body.set_position(player.x, player.y, player.z);


fps = 120;
frame_duration = 1/fps;

main.loop = 0;
main.i = 0;
main.start = timer();

move_speed = 2;
jump_speed = move_speed * 10;
friction_factor = move_speed * 8;
control_mode = 1;

main.surface.on_keypress = function(keycode, keyval)
    move_amount = .1;
    if (keycode == 23) then
	if (player.body.get_plane_depth(ws.ground) > -0.25) then
	    dx = math.sin(math.rad(main.heading/-1));
	    dy = math.cos(math.rad(main.heading/-1));
	    player.body.add_force(dx*move_speed, dy*move_speed, 0);
	    rx, ry, rz = normalize_vector(player.body.get_angular_velocity());
	end
    elseif (keycode == 19) then
	if (player.body.get_plane_depth(ws.ground) > -0.25) then
	    dx = 0-math.sin(math.rad(main.heading/-1));
	    dy = 0-math.cos(math.rad(main.heading/-1));
	    player.body.add_force(dx * move_speed, dy * move_speed, 0);
	end
--	main.x = main.x - (math.sin(math.rad(main.heading/-1)) * math.cos(math.rad(main.pitch/-1))*.1);
--	main.y = main.y - (math.cos(math.rad(main.heading/-1)) * math.cos(math.rad(main.pitch/-1))*.1);
--	main.z = main.z - (math.sin(math.rad(main.pitch/-1))*.1);
    elseif (keycode == 1) then
	if (player.body.get_plane_depth(ws.ground) > -0.25) then
	    dx = math.sin(math.rad((main.heading+90)/-1));
	    dy = math.cos(math.rad((main.heading+90)/-1));
	    player.body.add_force(dx * move_speed, dy * move_speed, 0);
	end
--	main.x = main.x + (math.sin(math.rad((main.heading+90)/-1)) * math.cos(math.rad(main.pitch/-1))*.1);
--	main.y = main.y + (math.cos(math.rad((main.heading+90)/-1)) * math.cos(math.rad(main.pitch/-1))*.1);
--	main.z = main.z + (math.sin(math.rad(main.pitch/-1))*.1);
    elseif (keycode == 4) then
	if (player.body.get_plane_depth(ws.ground) > -0.25) then
	    dx = math.sin(math.rad((main.heading-90)/-1));
	    dy = math.cos(math.rad((main.heading-90)/-1));
	    player.body.add_force(dx * move_speed, dy * move_speed, 0);
	end
--	main.x = main.x - (math.sin(math.rad((main.heading+90)/-1)) * math.cos(math.rad(main.pitch/-1))*.1);
--	main.y = main.y - (math.cos(math.rad((main.heading+90)/-1)) * math.cos(math.rad(main.pitch/-1))*.1);
--	main.z = main.z - (math.sin(math.rad(main.pitch/-1))*.1);
    elseif (keycode == 3) then -- A
	main.z = main.z + move_amount;
    elseif (keycode == 26) then -- Z
	main.z = main.z - move_amount;
    elseif (keyval == 115) then -- S
	main.fov = main.fov + move_amount;
    elseif (keyval == 120) then -- X
	main.fov = main.fov - move_amount;

    elseif (keycode == 60) then -- ~ tilda tilde
	echo("Control Mode toggle "..control_mode.."\n");
	if control_mode == 1 then
	    control_mode = 0;
	else
	    control_mode = 1;
	end
    elseif (keycode == 75) then
	if (player.body.get_plane_depth(ws.ground) > -0.25) then
	    player.body.add_force(0, 0, jump_speed);
	end
	--dx, dy, dz = player.body.get_position();
	--raycast(ws.ws, 10, dx, dy, dz, dx, dy, dz - 1);
    elseif (keyval == 101) then
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
        ball[ball_count].body.set_force(0, 0, 0);
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
    
    if control_mode == 1 then
    
	main.heading = main.heading + ((last_mouse_x - mouse_x) * x_sensitivity);
	main.pitch = main.pitch + ((last_mouse_y - mouse_y) * y_sensitivity);
    
--    player.body.set_rotation(0, 0, 0);
    
	last_mouse_x = mouse_x;
	last_mouse_y = mouse_y;

	if (mouse_r == 1) then
	end
	if (mouse_l == 1) then
	    ball[ball_count].x = main.x + ((math.sin(math.rad(main.heading/-1)) * math.cos(math.rad(main.pitch/-1))*.1) * 10);
    	    ball[ball_count].y = main.y + ((math.cos(math.rad(main.heading/-1)) * math.cos(math.rad(main.pitch/-1))*.1) * 10);
    	    ball[ball_count].z = main.z + ((math.sin(math.rad(main.pitch/-1))*.1)*10);
	    ball[ball_count].body.set_position(ball[ball_count].x, ball[ball_count].y, ball[ball_count].z);
	    ball[ball_count].body.set_force(0, 0, 0);
	    ball[ball_count].body.set_linear_velocity(0, 0, 0);
	    ball[ball_count].body.set_angular_velocity(0, 0, 0);
	    ball[ball_count].body.set_rotation(0, 0, 0);
    
	    x1, y1, z1 = ball[ball_count].x, ball[ball_count].y, ball[ball_count].z;
	    x2, y2, z2 = main.x, main.y, main.z;
	    dx = x1-x2;
	    dy = y1-y2;
	    dz = z1-z2;
	    dx, dy, dz = normalize_vector(dx, dy, dz);
	    f = 1000;
	    dx = dx * f;
	    dy = dy * f;
	    dz = dz * f;
	    ball[ball_count].body.add_relative_force(dx, dy, dz);
	end
	mouse.visible = 0;
    else
	mouse.visible = 1;
    end
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
    
    player.x, player.y, player.z = player.body.get_position();
    player.set_rotation(player.body.get_rotation());
    player.update();
    main.x, main.y, main.z = player.x, player.y, player.z;
    if (player.body.get_plane_depth(ws.ground) > -0.25) then
	rx, ry, rz = normalize_vector(player.body.get_angular_velocity());
	if (rx == rx) then
	    rx = rx * friction_factor * frame_duration;
	    ry = ry * friction_factor * frame_duration;
	    rz = rz * friction_factor * frame_duration;
	    player.body.add_relative_torque(rx/-1, ry/-1, rz/-1);
	    --echo("RX: "..rx..", RY: "..ry..", RZ: "..rz.."\n");
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
    main.surface.print(0, 0, "X: "..main.x.." Y: "..main.y.." Z: "..main.z);
    main.surface.print(0, 10, "Heading: "..main.heading.." Pitch: "..main.pitch);
    main.surface.print(0, 20, "FPS: "..main.fps.." Control Mode:"..control_mode);
end


--include("apps/
set_root_surface(main.surface.surface);
render_loop();
