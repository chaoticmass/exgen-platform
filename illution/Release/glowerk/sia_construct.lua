include("lib/render3d.lua");
include("lib/libode.lua");
include("lib/latex.lua");
include("construct/sia_logo.lua");
include("construct/indiebridge_logo.lua");
include("lib/serialize.lua");


set_window_title("Construct");

-- Start the HTTP server.
branch("construct/http.lua");

title_font = surface:new();
title_font.visible = 0;
title_font.bmp = "fonts/black_small_monospace.bmp";

body_font = surface:new();
body_font.visible = 0;
body_font.bmp = "fonts/black_micro_light.bmp";


main = camera_class:new();
main.surface.width = 63*2;
main.surface.height = 34*2;
main.surface.top = 0;
main.surface.left = 0;
main.surface.backcolor = makecol(128, 128, 128);
main.surface.visible = 0;
main.surface.z = 9999;
main.surface.clear();
main.surface.set_focus();
main.initialize();

screen = surface:new(0);
mouse = mouse_cursor:new();




--[[test = surface:new();
test.font = title_font;
test.text = "Hello.";
test.height = test.text_height();
test.width =  test.text_length(test.text);
test.backcolor = makecol(128,128,128);
test.clear();
test.print(0, 0, test.text);]]
--test.bmp = "images/destrado.bmp";
--test.width = test.width * .25;
--test.height = test.height * .25;


bg = surface:new();
bg.width = screen_width();
bg.height = screen_height();
bg.gradient(getvar("desktop", "forecolor"), getvar("desktop", "backcolor"));
bg.jpg = "images/r_colors1.jpg";


tex = latex_interface:new();
tex.source_head = [[
\\documentclass{report}
\\usepackage{tikz}
\\usepackage{fancybox}
\\usepackage[pdftex,dvips,papersize={16.2cm,8.8cm},vmargin={0cm,0cm},hmargin={0cm,0cm}]{geometry}
\\usepackage[T1]{fontenc}
\\font\\ttfarvoreg arvoreg at32pt
\\pdfpagewidth 16.2cm
\\pdfpageheight 8.8cm
\\pagestyle{empty}
\\begin{document}

\\noindent
\\fbox{
\\begin{minipage}[t][8.5cm]{15.75cm}
]]
tex.source_foot = [[
\\end{minipage}
}
\\end{document}
]]
tex.fonts = 'arvoreg';

print_window = window:new();
print_window.bare = 1;
print_window.backcolor = makecol(255, 255, 255);

print_window.height = 340 + print_window.margin_top + print_window.margin_bottom;
print_window.width = 630 + print_window.margin_left + print_window.margin_right;

print_output = surface:new();
print_output.parent = print_window;
print_output.top = print_window.margin_top;
print_output.left = print_window.margin_left;


viewer = window:new();
viewer.visible = 1;
grid = surface:new();
grid.parent = viewer;
grid.top = viewer.margin_top;
grid.left = viewer.margin_left;
grid.font = body_font;
grid.height = 340;
grid.width = 630;
grid.backcolor = makecol(255, 255, 255);
grid.clear();
viewer.width = grid.width + viewer.margin_left + viewer.margin_right;
viewer.height = grid.height + viewer.margin_top + viewer.margin_bottom;

model_count = 4;
model = {};
model[1] = model_class:new();
model[2] = model_class:new();
model[3] = model_class:new();
model[4] = model_class:new();
model[1].index = 1;
model[2].index = 2;
model[3].index = 3;
model[4].index = 4;
create_sia_logo_s(model[1], 0);
create_sia_logo_i(model[2], 0);
create_sia_logo_a(model[3], 0);
--create_sia_logo(model[4]);
create_indiebridge_logo(model[4]);

entity_count = 4;
entity = {};
for i=1,entity_count,1 do
    entity[i] = entity_class:new();
    entity[i].model = model[1];
    entity[i].x = 25 - (math.random(50));
    entity[i].y = 25 - (math.random(50));
    entity[i].z = 10 + (math.random(3)) + (25-entity[i].x);
    entity[i].r_x = math.random(64);
    entity[i].r_y = math.random(64);
    entity[i].r_z = math.random(64);
    entity[i].scale = 0.05;
    entity[i].visible = 1;
end

--[[entity[1].model = model[1];
entity[1].scale = 0.05;
entity[1].x = 0;
entity[1].y = 6.5;
entity[1].z = 7.9;
entity[1].r_x = 0;
entity[1].r_y = 64;
entity[1].r_z = 64;]]

-- s1
entity[1].model = model[3];
entity[1].scale = 0.05;
entity[1].x = 0;
entity[1].y = 6.7800064086914;
entity[1].z = 8.0900039672852;
entity[1].r_x = -82.699310302734;
entity[1].r_y = 64;
entity[1].r_z = -64;
-- s2
entity[2].model = model[3];
entity[2].scale = 0.05;
entity[2].x = 0;
entity[2].y = 7.2199935913086; -- smaller = to right
entity[2].z = 7.4199957847595; -- smaller = down
entity[2].r_x = -82.199317932129;
entity[2].r_y = 64;
entity[2].r_z = 64;

-- i
entity[3].model = model[2];
entity[3].scale = 0.05;
entity[3].x = 0;
entity[3].y = 6.05; -- smaller = to right
entity[3].z = 7.7; -- smaller = down
entity[3].r_x = 0;
entity[3].r_y = 64;
entity[3].r_z = 64;

-- a
entity[4].model = model[3];
entity[4].scale = 0.05;
entity[4].x = 0;
entity[4].y = 5.45; -- smaller = to right
entity[4].z = 7.7; -- smaller = down
entity[4].r_x = 0;
entity[4].r_y = 64;
entity[4].r_z = 64;
--[[
entity[5].model = model[4];
entity[5].scale = 0.05;
entity[5].x = 0;
entity[5].y = 8;
entity[5].z = 7.5;
entity[5].r_x = 0;
entity[5].r_y = 64;
entity[5].r_z = 64;
]]
main.x = -1.91;
main.y = 6.42;
main.z = 7.78;
main.pitch = 0;
main.heading = 0;

ws = worldspace_class:new();
ws.ground = ws.create_plane();

for i=1,entity_count,1 do
    entity[i].body = body_class:new(ws);
    entity[i].body.set_rotation(entity[i].r_x, entity[i].r_y, entity[i].r_z);
    entity[i].body.set_position(entity[i].x, entity[i].y, entity[i].z);
end

fps = 100;
frame_duration = 1/fps;

main.surface.alpha = 128;
--main.surface.rotation = 0;

main.loop = 0;
main.i = 0;
main.start = timer();


reset_entities = function() 
    for i=1,entity_count,1 do
	entity[i].model = model[2];
	entity[i].texture = page_texture;
	entity[i].x = 25 - (math.random(50));
	entity[i].y = 25 - (math.random(50));
	entity[i].z = 10 + (math.random(3)) + (25-entity[i].x);
	entity[i].r_x = math.random(64);
	entity[i].r_y = math.random(64);
	entity[i].r_z = math.random(64);
	entity[i].scale = 0.05;
    end
-- s1
entity[1].model = model[1];
entity[1].scale = 0.05;
entity[1].x = 0;
entity[1].y = 6.5;
entity[1].z = 7.9;
entity[1].r_x = 0;
entity[1].r_y = 64;
entity[1].r_z = -64;
-- s2
entity[2].model = model[1];
entity[2].scale = 0.05;
entity[2].x = 0;
entity[2].y = 7.5; -- smaller = to right
entity[2].z = 7.6; -- smaller = down
entity[2].r_x = 0;
entity[2].r_y = 64;
entity[2].r_z = 64;

-- i
entity[3].model = model[2];
entity[3].scale = 0.05;
entity[3].x = 0;
entity[3].y = 6.05; -- smaller = to right
entity[3].z = 7.7; -- smaller = down
entity[3].r_x = 0;
entity[3].r_y = 64;
entity[3].r_z = 64;

-- a
entity[4].model = model[3];
entity[4].scale = 0.05;
entity[4].x = 0;
entity[4].y = 5.45; -- smaller = to right
entity[4].z = 7.7; -- smaller = down
entity[4].r_x = 0;
entity[4].r_y = 64;
entity[4].r_z = 64;
    
    for i=1,entity_count,1 do
	entity[i].body.set_rotation(entity[i].r_x, entity[i].r_y, entity[i].r_z);
	entity[i].body.set_position(entity[i].x, entity[i].y, entity[i].z);
    end
end

global_on_keypress = function(keycode, keyval)
    echo("Keycode: "..keycode..", Keyval: "..keyval.."\n");

    amount = .01;
    

    if (keycode == 58) then
	shutdown();
    end

    if (keycode == 74) then
	if (entity[selected_entity].visible == 1) then
	    entity[selected_entity].visible = 0;
	else
	    entity[selected_entity].visible = 1;
	end
    end

    if (keycode == 79) then
	main.pitch = main.pitch - 1;
    elseif (keycode == 78) then
	main.pitch = main.pitch + 1;
    end
    if (keycode == 77) then
	main.heading = main.heading - 1;
    elseif (keycode == 81) then
	main.heading = main.heading + 1;
    end


    if (keycode == 84) then
	main.x = main.x + amount;
    elseif (keycode == 85) then
	main.x = main.x - amount;
    end

    if (keycode == 82) then
	main.y = main.y + amount;
    elseif (keycode == 83) then
	main.y = main.y - amount;
    end
    if (keycode == 1) then
	main.z = main.z + amount;
    elseif (keycode == 26) then
	main.z = main.z - amount;
    end


--[[
    if (keycode == 47) then 
	if (model_window.visible == 0) then
	    model_window.visible = 1;
	else 
	    model_window.visible = 0;
	end
    end
]]
    if (keycode == 48) then 
	if (prop_window.visible == 0) then
	    prop_window.visible = 1;
	else 
	    prop_window.visible = 0;
	end
    end
--[[
    if (keycode == 49) then 
	if (anim_window.visible == 0) then
	    anim_window.visible = 1;
	else 
	    anim_window.visible = 0;
	end
    end


    if (keycode == 50) then 
	if (term_window.visible == 0) then
	    term_window.visible = 1;
	else 
	    term_window.visible = 0;
	end
    end
]]
    -- F5
    if (keycode == 51) then
	tex.output_format = 'jpg';
	tex.create_document();
	tex.download_page();
	print_output.jpg = tex.pages[1];
	--echo(tex.pages[1].."\n");
	tex.destroy_document();
	print_window.width = print_output.width + print_window.margin_left + print_window.margin_right;
	print_window.height = print_output.height + print_window.margin_top + print_window.margin_bottom;
    end
    -- F6
    if (keycode == 52) then
	tex.create_document();
	tex.download_pdf();
	id = tex.id;
	tex.destroy_document();
    end
    
    -- F8
    if (keycode == 54) then
	--save("global", _G);
	save("entity", entity);
	savefile( "source.tex", tex.source);
	echo(tex.source);
    
	text = "";
	text = text .. "\nCamera:\n";
	text = text .. "	X:	"..main.x.."\n";
	text = text .. "	Y:	"..main.y.."\n";
	text = text .. "	Z:	"..main.z.."\n";
	text = text .. "	Pitch:	"..main.pitch.."\n";
	text = text .. "	Heading:	"..main.heading.."\n";
	
	for i=1,entity_count,1 do
	    if type(entity[i].body.body) ~= "nil" then
		if (entity[i].visible == 1) then
		    text = text .. "\nEntity "..i..":\n";
		    text = text .. "	Model:	"..entity[i].model.index.."\n";
		    text = text .. "	X:	"..entity[i].x.."\n";
		    text = text .. "	Y:	"..entity[i].y.."\n";
		    text = text .. "	Z:	"..entity[i].z.."\n";
		    text = text .. "	RX:	"..entity[i].r_x.."\n";
		    text = text .. "	RY:	"..entity[i].r_y.."\n";
		    text = text .. "	RZ:	"..entity[i].r_z.."\n";
		    text = text .. "	Scale:	"..entity[i].scale.."\n";
		end
	    end
	end
	savefile("save.txt", text);
    end


    -- -
    if (keycode == 62) then
	main.surface.width = screen_width();
	main.surface.height = screen_height();
	main.initialize();
    end
    -- +
    if (keycode == 61) then
	main.surface.width = 63*2;
	main.surface.height = 34*2;
	main.initialize();
    end

    if (keyval == 44) then
    	entity_controls = 0;
	vertex_controls = 1;    
	selected_face = 0;
	selected_model = selected_model - 1;
	if (selected_model < 1) then
	    selected_model = model_count;
	end
    end
    if (keyval == 46) then
    	entity_controls = 0;
	vertex_controls = 1;

	selected_face = 0;
	selected_model = selected_model + 1;
	if (selected_model > model_count) then
	    selected_model = 1;
	end
    end

    if (keyval == 60) then
	entity_controls = 1;
	vertex_controls = 0;
	selected_entity = selected_entity - 1;
	if (selected_entity < 1) then
	    selected_entity = entity_count;
	end
    end
    if (keyval == 62) then
	entity_controls = 1;
	vertex_controls = 0;
	selected_entity = selected_entity + 1;
	if (selected_entity > entity_count) then
	    selected_entity = 1;
	end
    end
    
    if (keycode == 68) then
	selected_face = selected_face - 1;
	if (selected_face < 0) then
	    selected_face = model[selected_model].face_count;
	end
    end
    if (keycode == 69) then
	selected_face = selected_face + 1;
	if (selected_face > model[selected_model].face_count) then
	    selected_face = 0;
	end
    end
    if (keycode == 65) then
	selected_vertex = selected_vertex - 1;
	if (selected_vertex < 0) then
	    selected_vertex = 2;
	end
    end
    if (keycode == 66) then
	selected_vertex = selected_vertex + 1;
	if (selected_vertex > 2) then
	    selected_vertex = 0;
	end
    end

    if (vertex_controls == 1) then
    if (keycode == 41) then
	x, y, z, u, v, c = model[selected_model].get_face_data(selected_face, selected_vertex);
	x = x - 0.1;
	model[selected_model].set_face_data(selected_face, selected_vertex, x, y, z, u, v, c);
    end
    if (keycode == 43) then
	x, y, z, u, v, c = model[selected_model].get_face_data(selected_face, selected_vertex);
	x = x + 0.1;
	model[selected_model].set_face_data(selected_face, selected_vertex, x, y, z, u, v, c);
    end
    if (keycode == 45) then
	x, y, z, u, v, c = model[selected_model].get_face_data(selected_face, selected_vertex);
	y = y - 0.1;
	model[selected_model].set_face_data(selected_face, selected_vertex, x, y, z, u, v, c);
    end
    if (keycode == 39) then
	x, y, z, u, v, c = model[selected_model].get_face_data(selected_face, selected_vertex);
	y = y + 0.1;
	model[selected_model].set_face_data(selected_face, selected_vertex, x, y, z, u, v, c);
    end
    end
    
    if (entity_controls == 1) then
	if (keycode == 41) then
	    entity[selected_entity].y = entity[selected_entity].y + 0.01;
	    entity[selected_entity].body.set_rotation(entity[selected_entity].r_x, entity[selected_entity].r_y, entity[selected_entity].r_z);
	    entity[selected_entity].body.set_position(entity[selected_entity].x, entity[selected_entity].y, entity[selected_entity].z);
	end
	if (keycode == 43) then
	    entity[selected_entity].y = entity[selected_entity].y - 0.01;
	    entity[selected_entity].body.set_rotation(entity[selected_entity].r_x, entity[selected_entity].r_y, entity[selected_entity].r_z);
	    entity[selected_entity].body.set_position(entity[selected_entity].x, entity[selected_entity].y, entity[selected_entity].z);
	end
	if (keycode == 45) then
	    entity[selected_entity].z = entity[selected_entity].z + 0.01;
	    entity[selected_entity].body.set_rotation(entity[selected_entity].r_x, entity[selected_entity].r_y, entity[selected_entity].r_z);
	    entity[selected_entity].body.set_position(entity[selected_entity].x, entity[selected_entity].y, entity[selected_entity].z);
	end
	if (keycode == 39) then
	    entity[selected_entity].z = entity[selected_entity].z - 0.01;
	    entity[selected_entity].body.set_rotation(entity[selected_entity].r_x, entity[selected_entity].r_y, entity[selected_entity].r_z);
	    entity[selected_entity].body.set_position(entity[selected_entity].x, entity[selected_entity].y, entity[selected_entity].z);
	end
	if (keycode == 44) then
	    entity[selected_entity].r_x = entity[selected_entity].r_x + 0.1;
	    entity[selected_entity].body.set_rotation(entity[selected_entity].r_x, entity[selected_entity].r_y, entity[selected_entity].r_z);
	    entity[selected_entity].body.set_position(entity[selected_entity].x, entity[selected_entity].y, entity[selected_entity].z);
	end
	if (keycode == 46) then
	    entity[selected_entity].r_x = entity[selected_entity].r_x - 0.1;
	    entity[selected_entity].body.set_rotation(entity[selected_entity].r_x, entity[selected_entity].r_y, entity[selected_entity].r_z);
	    entity[selected_entity].body.set_position(entity[selected_entity].x, entity[selected_entity].y, entity[selected_entity].z);
	end

	if (keycode == 38) then
	    entity[selected_entity].r_y = entity[selected_entity].r_y + 0.1;
	    entity[selected_entity].body.set_rotation(entity[selected_entity].r_x, entity[selected_entity].r_y, entity[selected_entity].r_z);
	    entity[selected_entity].body.set_position(entity[selected_entity].x, entity[selected_entity].y, entity[selected_entity].z);
	end
	if (keycode == 40) then
	    entity[selected_entity].r_y = entity[selected_entity].r_y - 0.1;
	    entity[selected_entity].body.set_rotation(entity[selected_entity].r_x, entity[selected_entity].r_y, entity[selected_entity].r_z);
	    entity[selected_entity].body.set_position(entity[selected_entity].x, entity[selected_entity].y, entity[selected_entity].z);
	end

	if (keycode == 89) then
	    entity[selected_entity].scale = entity[selected_entity].scale + 0.0005;
	    entity[selected_entity].body.set_rotation(entity[selected_entity].r_x, entity[selected_entity].r_y, entity[selected_entity].r_z);
	    entity[selected_entity].body.set_position(entity[selected_entity].x, entity[selected_entity].y, entity[selected_entity].z);
	end
	if (keycode == 88) then
	    entity[selected_entity].scale = entity[selected_entity].scale - 0.0005;
	    entity[selected_entity].body.set_rotation(entity[selected_entity].r_x, entity[selected_entity].r_y, entity[selected_entity].r_z);
	    entity[selected_entity].body.set_position(entity[selected_entity].x, entity[selected_entity].y, entity[selected_entity].z);
	end

	if (keycode == 86) then
	    current_model = entity[selected_entity].model.index;
	    current_model = current_model - 1;
	    if (current_model < 1) then
		current_model = model_count;
	    end
	    entity[selected_entity].model = model[current_model];
	end
	if (keycode == 87) then
	    current_model = entity[selected_entity].model.index;
	    current_model = current_model + 1;
	    if (current_model > model_count) then
		current_model = 1;
	    end
	    entity[selected_entity].model = model[current_model];
	end


    end

    echo("Model: "..selected_model..", Faces: "..model[selected_model].face_count.."\n");
    if (keycode == 28) then
	reset_entities();
    end
    
    if (keycode == 7) then
	if (grid_visible == 0) then
	    grid_visible = 1;
	else
	    grid_visible = 0;
	end
    end
end

set_global_on_keypress(global_on_keypress);


main.surface.on_draw = function()
    main.update();    
    for i=1,entity_count,1 do
	if type(entity[i].body.body) ~= "nil" then
	    if (entity[i].visible == 1) then
		entity[i].x, entity[i].y, entity[i].z = entity[i].body.get_position();
		entity[i].set_rotation(entity[i].body.get_rotation());
		entity[i].update();
		entity[i].render_gouraud(main);
		entity[i].plot(main);
	    end
	end
    end
    
--    page.update();
--    page.render(main);
    
--    ws.simulate_step(frame_duration);
    main.i = main.i + 1;
    main.surface.set_dirty();
    
--    screen.save_jpg('/root/render/'..string.format("%04d", main.i)..'_frame.jpg');
    setvar("frames", "latest", '/root/render/'..string.format("%04d", main.i)..'_frame.jpg');
    
--[[    if (main.i > 1000) then
	main.i = 0;
	--reset_entities();
	setvar("frames", "latest", '/root/render/'..string.format("%04d", 1)..'_frame.jpg');
    end]]
end

function round(num, idp)
  local mult = 10^(idp or 0)
  return math.floor(num * mult + 0.5) / mult
end


scale_x = 5;
scale_y = 5;
scale = 5;
print_scale = 0.5;
latex_width = 15.75;
latex_height = 8.5;
latex_scale_x = (latex_width / main.surface.width);
latex_scale_y = (latex_height / main.surface.height);
latex_precision = 0;


grid_source = "";
grid_source = "\\\\draw[xstep="..((1 * latex_scale_x)*2)..", ystep="..((1 * latex_scale_y)*2)..", color=gray]\n";
grid_source = grid_source .. " (0,"..(0-latex_height)..") grid ("..latex_width..", 0);\n";

selected_face = 0;
selected_model = 3;
selected_vertex = 0;
selected_entity = 1;
grid_visible = 1;
entity_controls = 1;
vertex_controls = 0;

grid.on_draw = function()
    main.surface.on_draw();
    surface_stretch_blit(main.surface.surface, grid.surface, 0, 0, main.surface.width, main.surface.height, 0, 0, grid.width, grid.height);


    source = "\\\\begin{tikzpicture}\n";
    
    source = source .. "\\\\filldraw[line width=.1pt,color=black]\n (0, 0) -- (0.0, 0.01) -- (0.01, 0.01) -- cycle;\n";

    if (grid_visible == 1) then
	grid.forecolor = makecol(0,0,0);
	for x = 0, grid.width, scale*2 do
	    grid.line(x, 0, x, grid.height);
	end
	for y = 0, grid.height, scale*2 do
    	    grid.line(0, y, grid.width, y);
	end
	grid.forecolor = makecol(255,255,255);
    end
    
    for i=1,entity_count,1 do
	if type(entity[i].body.body) ~= "nil" then
	if (entity[i].visible == 1) then
	    --echo("Entity["..i.."]\n");
	    for e=0,table.getn(entity[i].projection),1 do
		--[[echo("	Face["..e.."]\n");
		echo("		X1: "..(round(entity[i].projection[e].x1, 0)*print_scale)..", Y1: "..(round(entity[i].projection[e].y1, 0)*print_scale).."\n");
		echo("		X2: "..(round(entity[i].projection[e].x2, 0)*print_scale)..", Y2: "..(round(entity[i].projection[e].y2, 0)*print_scale).."\n");
		echo("		X3: "..(round(entity[i].projection[e].x3, 0)*print_scale)..", Y3: "..(round(entity[i].projection[e].y3, 0)*print_scale).."\n");
		]]
		
		--source = source .. "\\draw[line width=1pt,top color=red!1, middle color=red!50, bottom color=red!100, color=black, shading=axis, shading angle="..entity[i].projection[e].a.."]\n";
		source = source .. "\\\\fill[line width=0.1pt, color=black]\n";
		source = source .. "(".. (round(entity[i].projection[e].x1, latex_precision)*latex_scale_x) .. ", ".. (0-round(entity[i].projection[e].y1, latex_precision)*latex_scale_y) ..") -- ";
		source = source .. "(".. (round(entity[i].projection[e].x2, latex_precision)*latex_scale_x) .. ", ".. (0-round(entity[i].projection[e].y2, latex_precision)*latex_scale_y) ..") -- ";
		source = source .. "(".. (round(entity[i].projection[e].x3, latex_precision)*latex_scale_x) .. ", ".. (0-round(entity[i].projection[e].y3, latex_precision)*latex_scale_y) ..") -- ";
		source = source .. "cycle;\n";
		
		if (e == selected_face) and (entity[i].model.index == selected_model) then
		    grid.forecolor = makecol(255, 128, 255);
		    if (selected_vertex == 2) then
			grid.rect(round(entity[i].projection[e].x3, 0)*scale, round(entity[i].projection[e].y3, 0)*scale, 
			round(entity[i].projection[e].x3+1, 0)*scale, round(entity[i].projection[e].y3+1, 0)*scale );
		    elseif (selected_vertex == 1) then
			grid.rect(round(entity[i].projection[e].x2, 0)*scale, round(entity[i].projection[e].y2, 0)*scale, 
			round(entity[i].projection[e].x2+1, 0)*scale, round(entity[i].projection[e].y2+1, 0)*scale );
		    else
		        grid.rect(round(entity[i].projection[e].x1, 0)*scale, round(entity[i].projection[e].y1, 0)*scale, 
			round(entity[i].projection[e].x1+1, 0)*scale, round(entity[i].projection[e].y1+1, 0)*scale );
		    end
		else 
		    if (i == selected_entity) then
			grid.forecolor = makecol(0,255,0);
		    else
			grid.forecolor = makecol(255, 255, 255);
		    end
		end
		grid.line(round(entity[i].projection[e].x1, 0)*scale, round(entity[i].projection[e].y1, 0)*scale, round(entity[i].projection[e].x2, 0)*scale, round(entity[i].projection[e].y2,0)*scale);
		grid.line(round(entity[i].projection[e].x2, 0)*scale, round(entity[i].projection[e].y2, 0)*scale, round(entity[i].projection[e].x3, 0)*scale, round(entity[i].projection[e].y3,0)*scale);
		grid.line(round(entity[i].projection[e].x3, 0)*scale, round(entity[i].projection[e].y3, 0)*scale, round(entity[i].projection[e].x1, 0)*scale, round(entity[i].projection[e].y1,0)*scale);
		--grid.print(entity[i].projection[e].x1 * scale, entity[i].projection[e].y1 * scale, "D: "..entity[i].projection[e].d1..", "..(round(entity[i].projection[e].x1, 0)*print_scale)..", "..(round(entity[i].projection[e].y1, 0)*print_scale));
		--grid.print(entity[i].projection[e].x1 * scale, entity[i].projection[e].y1 * scale, round(entity[i].projection[e].d1, 0));
		--grid.print(entity[i].projection[e].x2 * scale, entity[i].projection[e].y2 * scale, round(entity[i].projection[e].d2, 0));
		--grid.print(entity[i].projection[e].x3 * scale, entity[i].projection[e].y3 * scale, round(entity[i].projection[e].d3) .." A: "..round(entity[i].projection[e].a, 0));
		--grid.print(entity[i].projection[e].ox * scale, entity[i].projection[e].oy * scale, "A: "..round(entity[i].projection[e].a, 0));
		
		
		
	    end
	end
	end
    end
    
    if (grid_visible == 1) then
	source = source .. grid_source;
    end
    source = source .. "\\\\end{tikzpicture}\n";
    source = "\\\\ttfarvoreg Solution Integration Agency \\\\newline \n"..source;
    tex.source = tex.source_head..source..tex.source_foot;
    grid.set_dirty();
end

main.surface.set_focus();

--measure angle between two verticies
measure = function()
    -- what two verticies to measure between?
    vertex1_x, vertex1_y, vertex1_z, vertex1_u, vertex1_v, vertex1_c = entities[1].model.get_face_data(0, 0);
    vertex2_x, vertex2_y, vertex2_z, vertex2_u, vertex2_v, vertex2_c = entities[1].model.get_face_data(0, 1);
    
    
end

render_loop();
