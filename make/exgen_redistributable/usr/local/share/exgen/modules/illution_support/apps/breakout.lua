echo("fuck\n");
--[[

shows how to create a fullscreen application for the best graphic performance

]]
initialize_video(1280, 768);

include("lib/render3d.lua");
include("lib/libode.lua");
include("lib/serialize.lua");

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

serve_force = 300;
bounce_force = 4;
paddle_size = 3;
paddle_speed = 3;
ball_size = 1;
brick_height = 1;
brick_width = 2;

ws = worldspace_class:new(0);
ws.on_collide = function(body_id_1, body_id_2) 
    ball_hit = false;
    paddle_hit = false;
    wall_hit = false;

    echo("Ball Hit: ");
    echo(ball_hit);
    echo("\n");
    if (body_id_1 == ball.body.id) then
	echo("Ball ");
	ball_hit = true;
    elseif (body_id_1 == paddle.body.id) then
	echo("Paddle ");
	paddle_hit = true;
    elseif (body_id_1 == wall[1].body.id) then
	echo("Wall ");
	wall_hit = true;
	wall_index = 1;
    elseif (body_id_1 == wall[2].body.id) then
	echo("Wall ");
	wall_hit = true;
	wall_index = 2;
    elseif (body_id_1 == wall[3].body.id) then
	echo("Wall ");
	wall_hit = true;
	wall_index = 3;
    else
	echo("Brick ");
	brick_hit = true;
	brick_hit_id = body_id_1;
    end

    echo(body_id_1);
    echo(" hit ");
    if (body_id_2 == ball.body.id) then
	echo("Ball ");
	ball_hit = true;
    elseif (body_id_2 == paddle.body.id) then
	echo("Paddle ");
	paddle_hit = true;
    elseif (body_id_2 == wall[1].body.id) then
	echo("Wall ");
	wall_hit = true;
	wall_index = 1;
    elseif (body_id_2 == wall[2].body.id) then
	echo("Wall ");
	wall_hit = true;
	wall_index = 2;
    elseif (body_id_2 == wall[3].body.id) then
	echo("Wall ");
	wall_hit = true;
	wall_index = 3;
    else
	echo("Brick ");
	brick_hit = true;
	brick_hit_id = body_id_2;
    end
    echo(body_id_2);
    echo("\n");

    for i=1, brick_count,1 do
        if (brick[i].body.id == brick_hit_id) then
--	    if (brick[i].expire = false) then
--		brick[i].body.set_linear_velocity(0,0,0);
		brick[i].expire = true;
		brick_index = i;
	    end
--	end
    end

    if (ball_hit) then
	if (brick_hit) then
	    x1, y1, z1 = ball.x, ball.y, ball.z
	    x2, y2, z2 = brick[brick_index].x, brick[brick_index].y, brick[brick_index].z

	    dx = x1-x2;
	    dy = y1-y2;
	    dz = z1-z2;
	    dx, dy, dz = normalize_vector(dx, dy, dz);
	    f = bounce_force;
	    dx = dx * f;
	    dy = dy * f;
	    dz = dz * f;
	    ball.body.set_linear_velocity(dx, dy, dz);
	end
	if (paddle_hit) then
	    x1, y1, z1 = ball.x, ball.y, ball.z
	    x2, y2, z2 = paddle.x, paddle.y, paddle.z

	    dx = x1-x2;
	    dy = y1-y2;
	    dz = z1-z2;
	    dx, dy, dz = normalize_vector(dx, dy, dz);
	    f = bounce_force;
	    dx = dx * f;
	    dy = dy * f;
	    dz = dz * f;
	    ball.body.set_linear_velocity(dx, dy, dz);
	    paddle.body.set_linear_velocity(0,0,0);
	end
	if (wall_hit) then
	    x1, y1, z1 = ball.x, ball.y, ball.z;
	    x2, y2, z2 = ball.l_x, ball.l_y, ball.l_z;
	    --x2, y2, z2 = wall[wall_index].x, wall[wall_index].y, wall[wall_index].z;

	    dx = x1-x2;
	    dy = y1-y2;
	    dz = z1-z2;

--[[
	    if (wall_index == 1) then
		dx = x1-x2;
		dy = y1-y2;
		if (dy < 0) then
		    dy = dy / -1;
		end
		dz = z1-z2;
	    elseif (wall_index == 2) then
		dx = x1-x2;
		dy = y1-y2;
		if (dy < 0) then
		    dy = dy / -1;
		end
		dz = z1-z2;
	    elseif (wall_index == 3) then
		dx = x1-x2;
		if (ball.x < 0) then
		    if (dx < 0) then
			dx = dx / -1;
		    end
		end
		dy = y1-y2;
		if (dy > 0) then
		    dy = dy / -1;
		end
		dz = z1-z2;
	    end]]
	    dx, dy, dz = normalize_vector(dx, dy, dz);
	    f = bounce_force;
	    dx = dx * f;
	    dy = dy * f;
	    dz = dz * f;
	    ball.body.set_linear_velocity(dx, dy, dz);
	    wall[wall_index].body.set_linear_velocity(0,0,0);	    
	end
    end

    wall[1].body.set_linear_velocity(0,0,0);
    wall[2].body.set_linear_velocity(0,0,0);
    wall[3].body.set_linear_velocity(0,0,0);
end

main.x = 0;
main.y = -10;
main.z = 25;
main.pitch = 80;
main.heading = 0;

ball_model = model_class:new();
create_sphere(ball_model, .75, 6, 0, 0, 0, nil);

brick_model = model_class:new();
--create_cube(brick_model, 512, 512, .5, 0, 0, 0, makecol(128, 0, 0));
create_3d_rectangle(brick_model, brick_width, brick_height, .5, 0, 0, 0, makecol(128, 0, 0), 0);

paddle_model = model_class:new();
create_3d_rectangle(paddle_model, paddle_size, .5, .5, 0, 0, 0, makecol(255, 255, 255), 0);

wall_model_h = model_class:new();
create_3d_rectangle(wall_model_h, 20.5, .5, .5, 0, 0, 0, makecol(128, 128, 128), 0);

wall_model_v = model_class:new();
create_3d_rectangle(wall_model_v, .5, 25, .5, 0, 0, 0, makecol(128, 128, 128), 0);

brick_count = 25;
brick = {};
z = 0;
for i=1,brick_count,1 do
    brick[i] = entity_class:new();
    brick[i].model = brick_model;
    brick[i].x = -5 +((z-1)*(brick_width + .5));
    brick[i].y = -9+(math.floor(i/1.6))-- + math.random(-5,5);
    if (math.mod(i,5) == 1) then
	z = 0;
    end

    z = z + 1;
    brick[i].z = 0;
    brick[i].r_z = 64;
    brick[i].r_y = 64;
    brick[i].r_x = 128;
    brick[i].scale = 1;

    brick[i].body = body_class:new(ws, create_body(ws.ws, "box", brick_width, brick_height, .5, 1));
    brick[i].body.set_rotation(brick[i].r_x, brick[i].r_y, brick[i].r_z);
    brick[i].body.set_position(brick[i].x, brick[i].y, brick[i].z);
end

paddle = entity_class:new();
paddle.model = paddle_model;
paddle.x = 0;
paddle.y = -18;
paddle.z = 0;
paddle.r_x = 0;
paddle.r_y = 0;
paddle.r_z = 0;
paddle.scale = 1;
paddle.body = body_class:new(ws, create_body(ws.ws, "box", paddle_size, .5, .5, 99999));
paddle.body.set_rotation(paddle.r_x, paddle.r_y, paddle.r_z);
paddle.body.set_position(paddle.x, paddle.y, paddle.z);

wall = {};
wall[1] = entity_class:new();
wall[1].model = wall_model_v;
wall[1].x = -10;
wall[1].y = -5;
wall[1].z = 0;
wall[1].r_x = 0;
wall[1].r_y = 0;
wall[1].r_z = 0;
wall[1].scale = 1;
wall[1].body = body_class:new(ws, create_body(ws.ws, "box", .5, 25, .5, 99999));
wall[1].body.set_rotation(wall[1].r_x, wall[1].r_y, wall[1].r_z);
wall[1].body.set_position(wall[1].x, wall[1].y, wall[1].z);

wall[2] = entity_class:new();
wall[2].model = wall_model_v;
wall[2].x = 10;
wall[2].y = -5;
wall[2].z = 0;
wall[2].r_x = 0;
wall[2].r_y = 0;
wall[2].r_z = 0;
wall[2].scale = 1;
wall[2].body = body_class:new(ws, create_body(ws.ws, "box", .5, 25, .5, 99999));
wall[2].body.set_rotation(wall[2].r_x, wall[2].r_y, wall[2].r_z);
wall[2].body.set_position(wall[2].x, wall[2].y, wall[2].z);

wall[3] = entity_class:new();
wall[3].model = wall_model_h;
wall[3].x = 0;
wall[3].y = 7.5;
wall[3].z = 0;
wall[3].r_x = 0;
wall[3].r_y = 0;
wall[3].r_z = 0;
wall[3].scale = 1;
wall[3].body = body_class:new(ws, create_body(ws.ws, "box", 20.5, .5, .5, 99999));
wall[3].body.set_rotation(wall[3].r_x, wall[3].r_y, wall[3].r_z);
wall[3].body.set_position(wall[3].x, wall[3].y, wall[3].z);

ball = entity_class:new();
ball.model = ball_model;
ball.x = 0;
ball.y = -17;
ball.z = 0;
ball.r_x = 0;
ball.r_y = 0;
ball.r_z = 0;
ball.scale = 1;
ball.body = body_class:new(ws, create_body(ws.ws, "sphere", .5, 1));
ball.body.set_rotation(ball.r_x, ball.r_y, ball.r_z);
ball.body.set_position(ball.x, ball.y, ball.z);
echo("Ball Body ID: "..ball.body.id.."\n\n");
fps = 30;
frame_duration = 1/fps;

main.loop = 0;
main.i = 0;
main.start = timer();

main.fps = 0;
main.wall_count = 0;

main.surface.on_draw = function()
    main.update();    
    for i=1,brick_count,1 do
	if type(brick[i].body.body) ~= "nil" then
	    brick[i].x, brick[i].y, brick[i].z = brick[i].body.get_position();
	    brick[i].set_rotation(brick[i].body.get_rotation());
	    brick[i].update();
	    brick[i].render_gouraud(main);
	    
	    if (brick[i].expire) then
		brick[i].body.destroy();
	    end
	end
    end

    for i=1,3,1 do
	--wall[i].body.set_linear_velocity(0,0,0);

	wall[i].x, wall[i].y, wall[i].z = wall[i].body.get_position();
	wall[i].set_rotation(wall[i].body.get_rotation());
	--wall[i].body.set_rotation(wall[i].r_x, wall[i].r_y, wall[i].r_z);
	--wall[i].body.set_position(wall[i].x, wall[i].y, wall[i].z);

	wall[i].update();
	wall[i].render_gouraud(main);
    end

    paddle.x, paddle.y, paddle.z = paddle.body.get_position();
    paddle.set_rotation(paddle.body.get_rotation);
    paddle.update();
    paddle.render_gouraud(main);

    ball.l_x, ball.l_y, ball.l_z = ball.x, ball.y, ball.z;
    ball.x, ball.y, ball.z = ball.body.get_position();
    ball.z = 0;    
    ball.body.set_position(ball.x, ball.y, ball.z);
    
    ball.set_rotation(ball.body.get_rotation);
    ball.update();
    ball.render_gouraud(main);

    ws.simulate_step(frame_duration);
    main.i = main.i + 1;
    main.surface.set_dirty();
    main.surface.print(0, 0, "Heading: "..main.heading.." Pitch: "..main.pitch.."\n");
    if (timer() - main.start > 1) then
	main.fps = main.i;
	main.i = 0;
	main.start = timer();
    end
    
end

main.surface.on_keypress = function(keycode, keyval)
    echo(keycode.."\n");
    move_amount = 1;
    if (keycode == 1) then -- A
	paddle.body.set_linear_velocity(0-paddle_speed,0,0);
    end
    if (keycode == 4) then -- D
	paddle.body.set_linear_velocity(paddle_speed,0,0);
    end
    if (keycode == 23) then -- W
	main.pitch = main.pitch + move_amount;
    end
    if (keycode == 19) then -- D
	main.pitch = main.pitch - move_amount;
    end
    if (keycode == 75) then
	force = serve_force;
	ball.body.set_force(0,force,0);
    end
end

render_loop();
