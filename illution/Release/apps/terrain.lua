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

mouse = mouse_cursor:new();
mouse.parent = main.surface;

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

--ball_model = model_class:new('apps/spacegame/planet.3ds');
ball_model = model_class:new();
create_cube(ball_model, 256, 256, 1, 0, 0, 0, makecol(128,0, 0));

ball_texture = surface:new();
ball_texture.bmp = 'apps/spacegame/earth.bmp';
ball_texture.visible = 0;

topo_texture = surface:new();
topo_texture.jpg = 'images/earth_topo.jpg';
topo_texture.visible = 0;

earth_texture = surface:new();
earth_texture.jpg = 'images/earth.jpg';
earth_texture.visible = 0;

ball_count = 3;
ball = {};
for i=1,ball_count,1 do
    ball[i] = entity_class:new();
    ball[i].model = ball_model;
    ball[i].texture = ball_texture;
    ball[i].x = -55;
    ball[i].y = 55;
    ball[i].z = 5+((i-1)*2);
    ball[i].r_x = 64;
    ball[i].r_y = 64;
    ball[i].r_z = 128;
    ball[i].scale = 1;
end

ground_model = model_class:new();
--create_model_from_image(ground_model, topo_texture, 1, 64, 0);
echo("Create 3D Rectangle\n");
x = 1024;
y = 768
x1 = x;
x2 = x+10;
y1 = y;
y2 = y+10;
ox = 0;
oy = 0;
w = 1;
h = 1;
sx = 1;
sy = 1;
sz = (1/255)*1;
step = 1;
for x=x1,x2-1,step do
    ox = ox + step;
    oy = 0;
    for y=y1,y2-1,step do
	oy = oy + step;
	echo("X: "..x.." Y: "..y.."\n");

	ca1 = topo_texture.get_pixel(x, y);
	ca2 = earth_texture.get_pixel(x, y);
	ga = get_rgb(ca1);
	ga = 0 - (ga*sz);

	cb1 = topo_texture.get_pixel(x+1, y);
	cb2 = earth_texture.get_pixel(x+1, y);
	gb = get_rgb(cb1);
	gb = 0 - (gb*sz);
	
	cc1 = topo_texture.get_pixel(x, y+1);
	cc2 = earth_texture.get_pixel(x, y+1);
	gc = get_rgb(cc1);
	gc = 0 - (gc*sz);

	cd1 = topo_texture.get_pixel(x+1, y+1);
	cd2 = earth_texture.get_pixel(x+1, y+1);
	gd = get_rgb(cd1);
	gd = 0 - (gd*sz);


	fc = ground_model.face_count;
	ground_model.add_face();
	ground_model.add_face();
	echo("G: "..ga.."\n");
	echo("FC: "..fc.."\n");
				-- face, i,    x,    y, z, u, v, c
	ground_model.set_face_data(fc+0, 0, (ox-w)*sx, (oy-h)*sy, ga, 0, 0, ca2);
	ground_model.set_face_data(fc+0, 1, (ox+w)*sx, (oy-h)*sy, gb, 0, 0, cb2);
	ground_model.set_face_data(fc+0, 2, (ox+w)*sx, (oy+h)*sy, gd, 0, 0, cd2);

	ground_model.set_face_data(fc+1,0, (ox+w)*sx, (oy+h)*sy, gd, 0, 0, cd2);
	ground_model.set_face_data(fc+1,1, (ox-w)*sx, (oy+h)*sy, gc, 0, 0, cc2);
	ground_model.set_face_data(fc+1,2, (ox-w)*sx, (oy-h)*sy, ga, 0, 0, ca2);

    end
end

ground = entity_class:new();
ground.model = ground_model;
ground.texture = topo_texture;
ground.x = 0;
ground.y = 0;
ground.z = 0;
ground.r_x = 0;
ground.r_y = 0;
ground.r_z = 128;
ground.scale = 1;



main.x = -75;
main.y = 55;
main.z = 10;
main.pitch = 0;
main.heading = 0;

ws = worldspace_class:new();
ws.ground = ws.create_plane();

for i=1,ball_count,1 do
    ball[i].body = body_class:new(ws, create_body(ws.ws, "box", .5, .5, .5, 1));
    ball[i].body.set_rotation(ball[i].r_x, ball[i].r_y, ball[i].r_z);
    ball[i].body.set_position(ball[i].x, ball[i].y, ball[i].z);
end

ground.body = body_class:new(ws, create_body(ws.ws, "box", .5, .5, .5, 1));
--ground.body = body_class:new(ws);
ground.body.set_rotation(ground.r_x, ground.r_y, ground.r_z);
ground.body.set_position(ground.x, ground.y, ground.z);


fps = 120;
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

    ground.x, ground.y, ground.z = ground.body.get_position();
    ground.set_rotation(ground.body.get_rotation());
    ground.update();
    ground.render_gouraud(main);
    
    ws.simulate_step(frame_duration);
    
    main.i = main.i + 1;
    main.surface.set_dirty();
end


set_root_surface(main.surface.surface);
render_loop();
