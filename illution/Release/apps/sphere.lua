--[[

shows how to create a fullscreen application for the best graphic performance

]]

include("lib/render3d.lua");

win = window:new();
win.width = 640;
win.height = 480;
--win.top = math.random(0, screen_height() );
--win.left = math.random(0, screen_width() );

main = camera_class:new();
main.surface.z = 9999;
main.surface.parent = win;
main.surface.width = 640 - win.margin_left - win.margin_left;
main.surface.height = 480 - win.margin_top - 3;
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

ball_texture = surface:new();
ball_texture.jpg = 'images/logo_512x512.jpg';
ball_texture.visible = 0;

topo_texture = surface:new();
topo_texture.jpg = 'images/earth_topo.jpg';
topo_texture.visible = 0;

earth_texture = surface:new();
earth_texture.jpg = 'images/earth3.jpg';
earth_texture.visible = 0;

create_sphere(ball_model, 10,32, 0, 0, 0, earth_texture);

ball = entity_class:new();
ball.model = ball_model;
ball.texture = earth_texture;
ball.x = 0;
ball.y = 0;
ball.z = 0;
ball.r_z = 0;
ball.r_y = 0;
ball.r_x = 0;
ball.scale = 1;
    


main.x = 0;
main.y = 10;
main.z = 0;
main.pitch = 0;
main.heading = 180;


main.surface.on_draw = function()
    main.surface.visible = 0;
    main.update();
    main.surface.clear();

    ball.update();
    ball.render(main);
    ball.r_z = ball.r_z + .01;
--    ball.r_x = ball.r_x + .1;
    main.surface.set_dirty();
--    main.heading = main.heading + 1;
    main.surface.print(0, 0, "X: "..main.x.." Y: "..main.y.." Z: "..main.z);
    main.surface.print(0, 10, "Heading: "..main.heading.." x_y: "..mouse_xy().." Pitch: "..main.pitch);
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
