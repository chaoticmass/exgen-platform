--[[

shows how to create a fullscreen application for the best graphic performance

]]
initialize_video(320, 240);
include("lib/render3d.lua");

win = window:new();
win.width = 320;
win.height = 240;
--win.top = math.random(0, screen_height() );
--win.left = math.random(0, screen_width() );

main = camera_class:new();
main.surface.z = 9999;
main.surface.parent = win;
main.surface.width = 320 - win.margin_left - win.margin_left;
main.surface.height = 240 - win.margin_top - 3;
main.surface.top = win.margin_top;
main.surface.left = win.margin_left;
main.surface.backcolor = makecol(0, 0, 0);
main.surface.z = 9999;
main.surface.clear();
main.surface.set_focus();
--main.surface.parent = window1;
main.initialize();


--ball_model = model_class:new('apps/spacegame/planet.3ds');
ball_model = model_class:new();

earth_texture = surface:new();
--earth_texture.jpg = 'images/earth4.jpg';
earth_texture.jpg = 'images/earth.jpg';
earth_texture.visible = 0;

earth_radius = 5;
create_sphere(ball_model, earth_radius*2,16, 0, 0, 0, earth_texture);

ball = entity_class:new();
ball.model = ball_model;
ball.texture = earth_texture;
ball.x = 0;
ball.y = 0;
ball.z = 0;
ball.r_z = 0;
ball.r_y = 0;
ball.r_x = 16.26;
ball.scale = 1;
    


main.x = 0;
main.y = 20;
main.z = 0;
main.pitch = 0;
main.heading = 180;

frames = 0;
fps = 0;
last_time = timer()
main.surface.on_draw = function()
    frames = frames + 1;
    
    main.surface.visible = 0;
    main.update();
    main.surface.clear();

    ball.update();
    ball.render(main);
    ball.r_z = ball.r_z + (main.y*.005);
--    ball.r_x = ball.r_x + .1;
    main.surface.set_dirty();
--    main.heading = main.heading + 1;
    if main.y > earth_radius * 1.125 then
	main.y = main.y - .01;
    end
    main.surface.print(0, 0, "X: "..main.x.." Y: "..main.y.." Z: "..main.z);
    main.surface.print(0, 10, "Heading: "..main.heading.." x_y: "..mouse_xy().." Pitch: "..main.pitch);
    main.surface.print(0, 20, "FPS: "..(fps));

    if (timer() - last_time > 1) then
	fps = frames;
	frames = 0;
	last_time = timer();
    end
    

    main.surface.visible = 1;
end

win.after_draw = function()
    main.surface.set_focus();
end

win.during_drag = function()
    main.surface.visible = 0;
end
win.after_drag = function()
    main.surface.z = 9999;
    main.surface.visible = 1;
end
win.visible = 1;
echo("hey\n");
render_loop();