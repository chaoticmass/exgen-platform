fps = 60;
frame_duration = 1 / fps;

slide = surface:new();
slide.left = 0;
slide.visible = 0;
slide.backcolor = makecol(255, 255, 255);
slide.width = 640;
slide.height = 480;
slide.gif = "apps/talk/images/face_blank.gif";

word = surface:new();
word.width = 32;
word.height = 16;
word.visible = 0;
word.print(0, 0, "talk");
wc = {};

--ball_texture = surface:new();
--ball_texture.gif = "apps/talk/images/face.gif";
--slide.clear();

globals = {};

start = 0;


script = {
    {
	name = "scene_1",
	start_time = 0,
	end_time = 20.00,
	start_function = function(me)
	    globals.world = worldspace_class:new();
	    world = globals.world;
	    
	    world.ground = world.create_plane();
	    
	    
	    cube1 = entity_class:new();
	    globals.cube1 = cube1;
	    cube1.texture = slide;
	    cube1.model = model_class:new();
	    create_cube(cube1.model, 16, 16, 64, 0, 0, 0, makecol(255, 255, 255));
	    cube1.x = 0;
	    cube1.y = 0;
	    cube1.z = 2;
	    cube1.r_x = 64;
	    cube1.r_y = 64;
	    cube1.r_z = 128;
	    cube1.scale = .015;
	    
	    for i=0,9,1 do
		x, y, z, u, v, c = cube1.model.get_face_data(i, 0);
		cube1.model.set_face_data(i, 0, x, y, z, 0, 0, c);
		
		x, y, z, u, v, c = cube1.model.get_face_data(i, 1);
		cube1.model.set_face_data(i, 1, x, y, z, 0, 0, c);
		
		x, y, z, u, v, c = cube1.model.get_face_data(i, 2);
		cube1.model.set_face_data(i, 2, x, y, z, 0, 0, c);
	    end
	    
	    camera.x = -1.15;
	    camera.y = 0;
	    camera.z = 2;
	    camera.pitch = 0;
	    camera.heading = 0;
	    
	    --cube1.body = body_class:new(world);
	    --cube1.body.set_rotation(cube1.r_x, cube1.r_y, cube1.r_z);
	    --cube1.body.set_position(cube1.x, cube1.y, cube1.z);
	    
	    
	end,
	run_function = function(me)
	    camera.update();
	    --cube1.x, cube1.y, cube1.z = cube1.body.get_position();
	    --cube1.set_rotation(cube1.body.get_rotation());
	    
	    cube1.update();
	    
	    if math.mod(me.current_frame, 2) == 0 then
		cube1.render(camera);
	    else 
		cube1.render_wireframe(camera);
	    end
	    
	    
	    --world.simulate_step(frame_duration);
	    --echo("render\n");
	end,
	end_function = function(me)
	    --world.destroy();
	    --cube1.destroy;
	    --camera.destroy();
	end
    },
    {
	name = "zoom_out",
	start_time = 3,
	end_time = 10,
	start_function = function(me)
	    camera.tween_x = create_tween(camera.x, camera.x + 15, me.total_frames);
	end,
	run_function = function(me)
	    camera.x = run_tween(camera.tween_x);
	end,
	end_function = function(me)
	end
    },

    {
	name = "change_face",
	start_time = 2,
	end_time = 2.5,
	start_function = function(me)
	    slide.gif = "apps/talk/images/face_blink.gif";
	end,
	run_function = function(me)
	end,	    
	end_function = function(me)
	    slide.gif = "apps/talk/images/face.gif";
	end
    },


    {
	name = "move_cam",
	start_time = 7.5,
	end_time = 10,
	start_function = function(me)
	    camera.tween_y = create_tween(camera.y, camera.y - 15, me.total_frames);
	    camera.tween_z = create_tween(camera.z, camera.z - 10, me.total_frames);
	end,
	run_function = function(me)
	    camera.y = run_tween(camera.tween_y);
	    camera.z = run_tween(camera.tween_z);
	    camera.turn_to(globals.cube1);
	    --camera.heading = camera.heading - 1;
	end,
	end_function = function(me)
	end
    },

    {
	name = "move_cube",
	start_time = 8.5,
	end_time = 12,
	start_function = function(me)
	    globals.cube1.tween_x = create_tween(globals.cube1.x, globals.cube1.x + 15, me.total_frames);
	end,
	run_function = function(me)
	    globals.cube1.x = run_tween(globals.cube1.tween_x);
	end,
	end_function = function(me)
	    slide.gif = "apps/talk/images/face_sing.gif";
	end
    },    
    
--[[
    {
	name = "",
	start_time = 0,
	end_time = 0,
	start_function = function(me)
	end,
	run_function = function(me)
	end,
	end_function = function(me)
	end
    },
    ]]
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
