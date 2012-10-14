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
main.surface.backcolor = makecol(255, 255, 255);
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

ball_model = model_class:new('apps/spacegame/planet.3ds');

ball_texture = surface:new();
ball_texture.bmp = 'apps/spacegame/earth.bmp';
--ball_texture.gif = '/root/latex/Talk/main_document1.gif';
ball_texture.visible = 0;

ball_count = 1;
ball = {};
for i=1,ball_count,1 do
    ball[i] = entity_class:new();
    ball[i].model = ball_model;
    ball[i].texture = ball_texture;
    ball[i].x = math.random(0,0.5);
    ball[i].y = i*.5;
    ball[i].z = 3 * i;
    ball[i].r_x = 64;
    ball[i].r_y = 64;
    ball[i].r_z = 128;
    ball[i].scale = .015;
end

main.x = -20;
main.y = 0;
main.z = 10;
main.pitch = 0;
main.heading = 0;

ws = worldspace_class:new();
ws.ground = ws.create_plane();

for i=1,ball_count,1 do
    ball[i].body = body_class:new(ws);
    ball[i].body.set_rotation(ball[i].r_x, ball[i].r_y, ball[i].r_z);
    ball[i].body.set_position(ball[i].x, ball[i].y, ball[i].z);
end

fps = 600;
frame_duration = 1/fps;

main.loop = 0;
main.i = 0;
main.start = timer();

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
    --main.surface.save_bmp('/root/projects/c/illution/Release/apps/frames/frame_'.. (1000 + main.i) ..'.bmp');
end


set_root_surface(main.surface.surface);
render_loop();
