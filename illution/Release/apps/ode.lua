include("lib/render3d.lua");
include("lib/libode.lua");

o_x = 0;
o_y = 0;
window1 = window:new();
window1.top = 0;
window1.left = 0;
window1.width = screen_width();
window1.height = screen_height();
window1.visible = 1;
window1.draggable = 0;


earth = surface:new();
earth.bmp = "images/earth_small.bmp";
earth.top = window1.margin_top;
earth.left = window1.align_center(earth);
earth.parent = window1;
earth.z = 999;

topo = surface:new();
topo.visible = 0;
topo.bmp = "images/earth_topo_small.bmp";

texture = surface:new();
texture.visible = 0;
texture.bmp = "images/earth_small.bmp";


earth.on_mouse_click = function(button, x, y)
    echo(x..", "..y.." click\n");
    o_x = x * (texture.width / earth.width);
    o_y = y * (texture.height / earth.height);
    window1.close.on_click();
end


earth.set_focus();
--render_loop(window1.surface);

echo(o_x..", "..o_y.."\n");

window1 = window:new();
window1.top = 0;
window1.left = 0;
window1.width = screen_width();
window1.height = screen_height();
window1.visible = 1;

main = camera_class:new();
main.surface.width = window1.inside_width;
main.surface.height = window1.inside_height;
main.surface.top = 32;
main.surface.left = 3;
main.surface.backcolor = makecol(255, 255, 255);
main.surface.z = 9999;
main.surface.clear();
main.surface.parent = window1;
main.initialize();


--ground_model = model_class:new('/root/projects/exgen/spacegame/models/planet.3ds');
ground_model = model_class:new();
ground_model.w = 20;
ground_model.h = 20;
ground_model.texture = texture;
echo(ground_model.w * ground_model.h * 2 .."\n");
total = 0;
for i = 1, ground_model.w * ground_model.h * 2, 1 do
	echo(".");
    	ground_model.add_face(); --	 x,	y,	z
	total = total + 1;
end

echo(o_x..", "..o_y.."\n");

o_x = 0;
o_y = 0;
x = 0;
y = 0;
fc = 0;
sz = 32;
step_x = texture.width / ground_model.w;
step_y = texture.height / ground_model.h;
echo("step: "..step_x ..", "..step_y.."\n");



w = ground_model.w;
h = ground_model.h;

radius = 10;

lat = 0 - radius; --y
lon = 0; --x

lat_step = (radius * 2) / h;
lon_step = 6.28318531 / w; --((radius * 2) * 3.1415) / w;
sz = sz;

main.surface.forecolor = makecol(0, 0, 0);
for y=0, h-1, 1  do
    for x=0, w-1, 1  do
	tx = o_x + (x * step_x);
	ty = o_y + (y * step_y);
	z_tl = topo.get_pixel(tx, ty);
	z_tr = topo.get_pixel(tx+step_x, ty);
	z_bl = topo.get_pixel(tx, ty+step_y);
	z_br = topo.get_pixel(tx+step_x, ty+step_y);
	
	z_tl, g, b = get_rgb(z_tl);
	z_tr, g, b = get_rgb(z_tr);
	z_bl, g, b = get_rgb(z_bl);
	z_br, g, b = get_rgb(z_br);
	
	echo("tx:"..tx..", ty:"..ty.."\n");

	c_tl = texture.get_pixel(tx, ty);
	c_tr = texture.get_pixel(tx+step_x, ty);
	c_bl = texture.get_pixel(tx, ty+step_y);
	c_br = texture.get_pixel(tx+step_x, ty+step_y);
	
	lat = (0 - radius) + (y * lat_step);
	lon = x * lon_step;
	
	x3 = math.cos(lon) * radius;
	z3 = math.sin(lon) * radius;

	lat2 = (0 - radius) + ((y + 1) * lat_step);
	lon2 = (x + 1) * lon_step;
	
	x4 = math.cos(lon2) * radius;
	z4 = math.sin(lon2) * radius;
	
	echo ("x: ".. x ..", lon:".. lon..", x3: ".. x3 .."\n");
	echo ("y: ".. y ..", lat:".. lat..", z3: ".. z3 .."\n");
	--main.surface.rect_fill(x3, z3, x3, z3);
	
	--tl
	ground_model.set_face_data(fc, 0, x3,	z3, 	lat, tx, ty, c_tl);
	--bl
	ground_model.set_face_data(fc, 1, x3, 	z3, 	lat2, tx, ty+step_y, c_bl);
	--br
	ground_model.set_face_data(fc, 2, x4, 	z4, 	lat2, tx+step_x, ty+step_y, c_br);
	fc = fc + 1;	
	
	--tl
	ground_model.set_face_data(fc, 0, x3,	z3, 	lat, tx, ty, c_tl);
	--tr
	ground_model.set_face_data(fc, 1, x4,	z4, 	lat, tx+step_x, ty, c_tr);
	--br
	ground_model.set_face_data(fc, 2, x4,	z4, 	lat2, tx+step_x, ty+step_y, c_br);
	fc = fc + 1;	

	echo(fc .. " out of "..total.."\n");
    end
end
ground = entity_class:new();
ground.model = ground_model;
ground.texture = texture;
ground.x = 100;
ground.y = 0;
ground.z = 0;
ground.r_y = 0;
ground.scale = 1;



main.x = 5;
main.y = 120;
main.z = 80;
main.pitch = 0;
main.heading = 0;



ws = worldspace_class:new();
ws.ground = ws.create_plane();

body1 = body_class:new(ws);
body1.set_position(ground.x, ground.y, ground.z);



window1.on_keypress = function(keycode, keyval)
    --echo("Keycode: ".. keycode .." Keyval: ".. keyval .."\n");
    echo(main.x .. ", ".. main.y ..", ".. main.z .."\n");
    speed = 10;
    if (keycode == 84) then
	-- up
	main.pitch = main.pitch - speed;
    elseif (keycode == 85) then
	-- down
	main.pitch = main.pitch + speed;
    elseif (keycode == 83) then
	-- right
	main.heading = main.heading + speed;
    elseif (keycode == 82) then
	-- left
	main.heading = main.heading - speed;
    elseif (keycode == 1) then
	main.x = main.x + speed;
    elseif (keycode == 26) then
	main.x = main.x - speed;
    elseif (keycode == 19) then
	main.y = main.y + speed;
    elseif (keycode == 24) then
	main.y = main.y - speed;
    elseif (keycode == 4) then
	main.z = main.z + speed;
    elseif (keycode == 3) then
	main.z = main.z - speed;

    end
end

main.loop = 0;
main.i = 0;
main.start = timer();
main.surface.on_draw = function()
    main.update();    
    
    --ground.r_z = ground.r_z + 1;
    ground.update();

    ground.render(main);

    main.surface.set_dirty();
end

window1.set_focus();
--set_root_surface(window1.surface);
render_loop(window1.surface);

