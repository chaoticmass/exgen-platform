--[[


]]

include("lib/render3d.lua");
include("lib/libode.lua");


include("apps/talk/words.lua");
include("apps/talk/script.lua");
--[[
screen = surface:new();
screen.height = screen_height();
screen.width = screen_width();
screen.top = 0;
screen.left = 0;
screen.backcolor = makecol(255, 0, 255);
screen.set_focus();
screen.clear();
screen.visible = 1;


screen.i = 0;
screen.start = timer() - start;

--screen.on_draw = function()
]]

camera = camera_class:new();
camera.surface.fullscreen();
camera.surface.visible = 1;
camera.surface.z = 9999;
camera.surface.backcolor = makecol(0, 25,0);
camera.surface.clear();
camera.surface.set_focus();
camera.initialize();

camera.start = timer() - start;
camera.surface.on_draw = function()
    camera.update();
    for i=1,table.getn(script),1 do
	if (timer() - camera.start > script[i].start_time) then
	    if (script[i].run ~= 1) then
		script[i].run = 1;
		echo(script[i].start_time..": "..script[i].name.." start.\n");

		script[i].duration = (script[i].end_time - script[i].start_time);
		script[i].total_frames = script[i].duration * fps;
		script[i].current_frame = 0;
		script[i].current_time = 0;
		
		script[i].start_function(script[i]);
		script[i].last_run = timer();
	    else 
		if (script[i].done ~= 1) then
		    if (timer() - script[i].last_run > frame_duration) then
			script[i].current_frame = script[i].current_frame + 1;
			script[i].current_time = timer() - script[i].start_time - camera.start;
			script[i].time_step = timer() - script[i].last_run;
			
			script[i].run_function(script[i]);
			script[i].last_run = timer();
		    end
		end
	    end
	end
	if (timer() - camera.start > script[i].end_time) then
	    if (script[i].done ~= 1) then
		script[i].done = 1;
		echo(script[i].end_time..": "..script[i].name.." end.\n");
		script[i].end_function(script[i]);
	    end
	end

    end

    camera.surface.set_dirty();
end


set_root_surface(camera.surface.surface);
render_loop(camera.surface.surface);
