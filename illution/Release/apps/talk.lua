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
--main.surface.backcolor = makecol(255, 255, 255);
main.surface.backcolor = makecol(0, 0, 0);
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

slide = surface:new();
--slide.parent = main.surface;
--slide.gif = '/root/latex/Talk/main_document1.gif';
slide.gif = 'images/unaccept.gif';
--slide.clear();
slide.visible = 0;

ball_texture = surface:new();
ball_texture.bmp = 'images/earth.bmp';
--ball_texture.gif = '/root/latex/Talk/main_document1.gif';
ball_texture.visible = 0;

cube_texture = surface:new();
--ball_texture.bmp = '/root/projects/exgen/spacegame/textures/earth.bmp';
cube_texture.gif = 'images/main_document1.gif';
cube_texture.visible = 0;


cube_width = cube_texture.width;
cube_height = cube_texture.height;

ball_model = model_class:new('/root/projects/exgen/spacegame/models/planet.3ds');
cube = model_class:new();
--create_cube(cube, cube_width, cube_height, 64, 0, 0, 0, 0);

icon = surface:new();
icon.parent = main.surface;
icon.visible = 1;
--icon.gif = "images/bubble.gif";
icon.top = 100;
icon.width = 32;
icon.height = 16;
icon.print(0, 0, "talk");
create_model_from_image(cube, icon, 1, 16);


ball_count = 10;
ball = {};
for i=1,ball_count,1 do
    ball[i] = entity_class:new();
    ball[i].model = ball_model;
    ball[i].texture = ball_texture;
    ball[i].x = math.random(5);
    ball[i].y = 0; --math.random(5);
    ball[i].z = 1 * i;
    ball[i].r_x = 64;
    ball[i].r_y = 64;
    ball[i].r_z = 128;
    ball[i].scale = .015;
end

ball[9].model = cube;
ball[9].texture = cube_texture;
ball[9].x = 0;

main.x = -15;
main.y = 10;
main.z = 0;
main.pitch = 0;
main.heading = 0;

ws = worldspace_class:new();
ws.ground = ws.create_plane();

for i=1,ball_count,1 do
    ball[i].body = body_class:new(ws);
    ball[i].body.set_rotation(ball[i].r_x, ball[i].r_y, ball[i].r_z);
    ball[i].body.set_position(ball[i].x, ball[i].y, ball[i].z);
end

fps = 60;
frame_duration = 1/fps;


main.dist = 25;
main.slide = 0;

main.loop = 0;
main.i = 0;
main.start = timer();
main.surface.on_draw = function()
    if (main.i < 3000) then
    main.update();    
    
    --surface_blit(slide.surface, main.surface.surface);
    --, 0, 0, 0, 0, slide.width, slide.height
    
    for i=1,ball_count,1 do
	
	if i == 9 then	
	    
	    ball[i].r_y = ball[i].r_y + .5;
	    ball[i].update();	
	    --ball[i].render_gouraud(main);
	    ball[i].render(main);
	else
	    ball[i].x, ball[i].y, ball[i].z = ball[i].body.get_position();
	    ball[i].update();	
	    ball[i].set_rotation(ball[i].body.get_rotation());
	    ball[i].render(main);
	    --ball[i].render_gouraud(main);
	end
    end
    if (main.dist > 2.5) then
	--main.dist = main.dist - .05;

    else
	if (main.slide == 0) then
	    main.slide = main.slide + 1;
	    cube_texture.gif = 'images/main_document2.gif';
	end    
    end
    main.x = ball[9].x - main.dist;
    main.y = ball[9].y + main.dist;
    main.z = ball[9].z;


    main.look_at(ball[9]);
    --main.heading = main.heading + 1;
    
    ws.simulate_step(frame_duration);
    
    main.i = main.i + 1;
    
    
    main.surface.set_dirty();
    --main.surface.save_bmp('/root/projects/c/illution/Release/apps/frames/frame_'.. (1000 + main.i) ..'.bmp');
    else 
	
	echo(timer() - main.start.."\n");
    end
end


set_root_surface(main.surface.surface);
render_loop(main.surface.surface);
