--[[

shows how to create a fullscreen application for the best graphic performance

Creates a 3D scene with 20 textured spheres. A physics simulation runs and drops the
spheres from a height. The spheres fall to the ground and bounce around.

]]

-- these libraries wrap the 3D and physics functions provided by illution
-- into nice friendly Lua classes
include("lib/render3d.lua");
include("lib/libode.lua");

-- Create the 3D scene and camera
main = camera_class:new();
main.surface.width = screen_width();
main.surface.height = screen_height();
main.surface.top = 0;
main.surface.left = 0;
main.surface.backcolor = makecol(255, 255, 255);
main.surface.clear();
main.surface.set_focus();
main.initialize();

-- Make a button for quitting the 
button1 = button:new();
button1.parent = main.surface;
button1.top = 0;
button1.left = 0;

button1.on_click = function() 
-- Destroying the main surface causes the render_loop() function to return,
-- allowing execution to reach the end of the script, whereupon the app terminates
-- and all resources are released
    main.surface.destroy();
end

-- Create a 3D model object using planet.3ds
ball_model = model_class:new('spacegame/models/planet.3ds');

-- Load the texture for the model
ball_texture = surface:new();
ball_texture.bmp = 'spacegame/textures/earth.bmp';
ball_texture.visible = 0;

-- Create an array to hold the entity objects we'll use in the simulation
ball_count = 20;
ball = {};

-- Populate the array with entities and set their properties
-- An `entity' takes a 3D model, texture, coordinates etc and represents
-- something that can be rendered in 3D space
for i=1,ball_count,1 do
    ball[i] = entity_class:new();
    ball[i].model = ball_model;
    ball[i].texture = ball_texture;
    ball[i].x = 0;
    ball[i].y = i*.5;
    ball[i].z = 3 * i;
    ball[i].r_x = 64;
    ball[i].r_y = 64;
    ball[i].r_z = 128;
    ball[i].scale = .015;
end

-- Position and orient the camera
main.x = -15;
main.y = 0;
main.z = 10;
main.pitch = 0;
main.heading = 0;

-- Create the world space for the physics simulation
ws = worldspace_class:new();
ws.ground = ws.create_plane();

-- A `body' is an object in the physics simulation
-- This creates bodies for each 3D ball and sets their position an
-- orientation.
for i=1,ball_count,1 do
    ball[i].body = body_class:new(ws);
    ball[i].body.set_rotation(ball[i].r_x, ball[i].r_y, ball[i].r_z);
    ball[i].body.set_position(ball[i].x, ball[i].y, ball[i].z);
end

-- Allow the user to move the camera using the arrow keys
main.surface.on_keypress = function(keycode, keyval)
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

end

-- This is called for every frame rendered
main.surface.on_draw = function()
    -- Update the camera
    main.update();    
    
    -- Update the 3D entities to match the bodies which have moved in the 
    -- simulation since last frame
    for i=1,ball_count,1 do
	if type(ball[i].body.body) ~= "nil" then
	    ball[i].x, ball[i].y, ball[i].z = ball[i].body.get_position();
	    ball[i].set_rotation(ball[i].body.get_rotation());
	    ball[i].update();
	    ball[i].render(main);
	end
    end

    -- Step the simulation
    ws.simulate_step(0.01); 
    
    main.surface.set_dirty();
end

set_root_surface(main.surface.surface);

-- This function does not return until main.surface.surface no longer exists 
-- or the program terminates some other way. When render_loop returns there is
-- nothing left in the script to execute and it will terminate.
-- While render_loop is running the events above will be called.
render_loop(main.surface.surface);
