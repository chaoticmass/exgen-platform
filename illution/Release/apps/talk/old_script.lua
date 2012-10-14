fps = 60;
frame_duration = 1 / fps;

slide = surface:new();
slide.left = 0;
slide.visible = 0;
slide.backcolor = makecol(255, 255, 255);
slide.width = 640;
slide.height = 480;
slide.gif = "apps/talk/images/small_slide1.gif";

ball_texture = surface:new();
ball_texture.gif = "apps/talk/images/face.gif";
--slide.clear();

globals = {};

start = 0;



script = {
    {
	name = "fall",
	start_time = 0,
	end_time = 300.00,
	start_function = function(me)
	    globals.world = worldspace_class:new();
	    world = globals.world;
	    
	    world.ground = world.create_plane();
	    
	    ball1 = entity_class:new();
	    ball1.model = model_class:new("apps/talk/models/sphere.3ds");
	    ball1.model.set_offset(25, 0, -15);
	    ball1.texture = ball_texture;
	    
	    ball1.x = 0;
	    ball1.y = 0;
	    ball1.z = 2;
	    ball1.r_x = 0;
	    ball1.r_y = 0;
	    ball1.r_z = 0;
	    ball1.scale = .010;

	    
	    cube1 = entity_class:new();
	    cube1.texture = slide;
	    cube1.model = model_class:new();
	    create_cube(cube1.model, 512, 512, 64, 0, 0, 0, makecol(255, 255, 255));
	    cube1.x = 0;
	    cube1.y = 0;
	    cube1.z = 2;
	    cube1.r_x = 64;
	    cube1.r_y = 64;
	    cube1.r_z = 128;
	    cube1.scale = .018;
	    
	    camera.x = -2;
	    camera.y = 0;
	    camera.z = 2;
	    camera.pitch = 0;
	    camera.heading = 0;
	    
	    ball1.body = body_class:new(world);
	    ball1.body.set_rotation(cube1.r_x, cube1.r_y, cube1.r_z);
	    ball1.body.set_position(cube1.x, cube1.y, cube1.z);
	    
	    
	end,
	run_function = function(me)
	    camera.update();
	    --cube1.x, cube1.y, cube1.z = cube1.body.get_position();
	    --cube1.set_rotation(cube1.body.get_rotation());

	    ball1.update();
	    ball1.render(camera);
	    
	    --cube1.r_y = cube1.r_y + .5;
	    cube1.update();
	    cube1.render_wireframe(camera);
	    
	    kv, kc = readkey();
	    inc = 32;
	    
	    if (kv == 1) then
		ball1.r_x = ball1.r_x + inc;
	    elseif (kv == 26) then
		ball1.r_x = ball1.r_x - inc;
	    elseif (kv == 19) then
		ball1.r_y = ball1.r_y + inc;
	    elseif (kv == 24) then
		ball1.r_y = ball1.r_y - inc;
	    elseif (kv == 4) then
		ball1.r_z = ball1.r_z + inc;
	    elseif (kv == 3) then
		ball1.r_z = ball1.r_z - inc;
	    end
	    echo(kv.." X: "..ball1.r_x.." Y: "..ball1.r_y.." Z: "..ball1.r_z.."\n");
	    
	    world.simulate_step(frame_duration);
	    --echo("render\n");
	end,
	end_function = function(me)
	    --world.destroy();
	    --cube1.destroy;
	    --camera.destroy();
	end
    },
--[[    {
	name = "intro",
	start_time = 0,
	end_time = 19.72,
	start_function = function(me)
	    screen.backcolor = makecol(0, 0, 0);
	    screen.clear();
	    slide.parent = screen;
	    slide.alpha = 255;
	    slide.visible = 1;	    
	    slide.step = 255 / me.duration;
	    slide.alpha_float = slide.alpha;
	end,
	run_function = function(me)
	    --slide.alpha_float = slide.alpha_float - (slide.step * me.time_step);
	    slide.alpha_float = 255 - (slide.step * me.current_time);
	    slide.alpha = slide.alpha_float;
	end,
	end_function = function(me)
	end
    },]]
    {
	name = "ba-da-da",
	start_time = 19.72,
	end_time = 23.63,
	start_function = function(me)
	    slide.step = 255 / me.duration;
	    slide.alpha_float = slide.alpha;	
	end,
	run_function = function(me)
	    slide.alpha_float = (slide.step * me.current_time);
	    slide.alpha = slide.alpha_float;
	end,
	end_function = function(me)
	end
    },
    {
	name = "ba-da-da",
	start_time = 27.54,
	end_time = 31.45,
	start_function = function(me)
	end,
	run_function = function(me)
	end,
	end_function = function(me)
	end
    },

    {
	name = "Oh brother I can't, I can't get through.",
	start_time = 36.01,
	end_time = 42.5,
	start_function = function(me)
	    slide.gif = "apps/talk/images/small_slide1.gif";
	end,
	run_function = function(me)
	    --surface_stretch_blit(slide.surface, camera.surface.surface);
	end,
	end_function = function(me)
	    slide.clear();
	end
    },
    {
	name = "I've been trying hard to reach you",
	start_time = 43.23,
	end_time = 46.21,
	start_function = function(me)
	    slide.gif = "apps/talk/images/small_slide2.gif";
	end,
	run_function = function(me)
	end,
	end_function = function(me)
	    slide.clear();
	end
    },
    {
	name = "Coz I don't know what to do",
	start_time = 46.21,
	end_time = 50.52,
	start_function = function(me)
	    slide.gif = "apps/talk/images/small_slide3.gif";
	end,
	run_function = function(me)
	end,
	end_function = function(me)
	    slide.clear();
	end
    },
    {
	name = "Oh brother I can't believe it's true",
	start_time = 51.98,
	end_time = 57.63,
	start_function = function(me)
	    slide.gif = "apps/talk/images/small_slide4.gif";
	end,
	run_function = function(me)
	end,
	end_function = function(me)
	    slide.clear();
	end
    },
    test
}
