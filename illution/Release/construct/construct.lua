include("lib/render3d.lua");
include("lib/libode.lua");
include("lib/latex.lua");
include("construct/sia_logo.lua");
--include("construct/indiebridge_logo.lua");
include("lib/serialize.lua");

set_window_title("Construct");

-- Settings
-- Paper Size
paper_width = 279.4;  -- mm, 11in
paper_height = 215.9; -- mm  8.5in
canvas_width = paper_width - 10;
canvas_height = paper_height - 10;

-- End Settings

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
--bg.jpg = "images/r_colors1.jpg";

latex_font = "felix";
tex = latex_interface:new();
tex.source_head = [[
\\documentclass{report}
\\usepackage{tikz}
\\usepackage{fancybox}
\\usepackage[pdftex,dvips,papersize={]]..paper_width..[[mm,]]..paper_height..[[mm},vmargin={0cm,0cm},hmargin={0cm,0cm}]{geometry}
\\usepackage[T1]{fontenc}
\\font\\ttf]]..latex_font..[[ ]]..latex_font..[[ at32pt
\\pdfpagewidth ]]..paper_width..[[mm
\\pdfpageheight ]]..paper_height..[[mm
\\pagestyle{empty}
\\begin{document}

\\noindent
\\fbox{
\\begin{minipage}[t][]]..canvas_height..[[mm]{]]..canvas_width..[[mm}
]]

tex.source_foot = [[
\\end{minipage}
}
\\end{document}
]]

tex.fonts = latex_font;

print_window = window:new();
print_window.bare = 1;
print_window.backcolor = makecol(255, 255, 255);


print_output = surface:new();
print_output.parent = print_window;
print_output.top = print_window.margin_top;
print_output.left = print_window.margin_left;

logo = surface:new();
--include("construct/glowerk.lua");




viewer = window:new();
viewer.visible = 1;
grid = surface:new();
grid.parent = viewer;
grid.top = viewer.margin_top;
grid.left = viewer.margin_left;
grid.font = body_font;
grid.height = canvas_height*4;
grid.width = canvas_width*4;
grid.backcolor = makecol(128, 128, 128);
grid.z = logo.z + 1;
grid.clear();
viewer.width = grid.width + viewer.margin_left + viewer.margin_right;
viewer.height = grid.height + viewer.margin_top + viewer.margin_bottom;
print_window.height = grid.height + print_window.margin_top + print_window.margin_bottom;
print_window.width = grid.width + print_window.margin_left + print_window.margin_right;
print_window.left = viewer.width;


logo.parent = viewer;
logo.left = ((viewer.width - viewer.margin_left - viewer.margin_right) * 0.5) - (logo.width * 0.5);
logo.top = viewer.margin_top;
grid.alpha = 128;

model_count = 1;
model = {};
model[1] = model_class:new();
create_sia_logo_a(model[1], 0);
model[1].index = 1;

entity_count = 1;
entity = {};
entity[1] = entity_class:new();
entity[1].visible = 1;

-- s1
entity[1].model = model[1];
entity[1].scale = 0.05;
entity[1].x = 0;
entity[1].y = 6.7800064086914;
entity[1].z = 8.0900039672852;
entity[1].r_x = -82.699310302734;
entity[1].r_y = 64;
entity[1].r_z = -64;

main.x = -1.91;
main.y = 6.42;
main.z = 7.78;
main.pitch = 0;
main.heading = 0;

fps = 100;
frame_duration = 1/fps;

main.loop = 0;
main.i = 0;
main.start = timer();

include("construct/io.lua");

main.surface.on_draw = function()
    main.update();    
    for i=1,entity_count,1 do
	if type(entity[i]) ~= "nil" then
	    if (entity[i].visible == 1) then
		entity[i].update();
		entity[i].render_gouraud(main);
		entity[i].plot(main);
	    end
	end
    end
    
--    page.update();
--    page.render(main);
    
--    ws.simulate_step(frame_duration);
--    main.i = main.i + 1;
    main.surface.set_dirty();
    
--    screen.save_jpg('/root/render/'..string.format("%04d", main.i)..'_frame.jpg');
--    setvar("frames", "latest", '/root/render/'..string.format("%04d", main.i)..'_frame.jpg');
    
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
print_scale = 1/8;
latex_width = canvas_width * 0.1;
latex_height = canvas_height * 0.1; -- canvas_height (mm) to latex_height (cm)
latex_scale_x = print_scale;
latex_scale_y = print_scale;
cwx = grid.width / (latex_width / (latex_scale_x * 4));
cwy = grid.height / (latex_height / (latex_scale_y * 4));

latex_precision = 0;


grid_source = "";
grid_source = "\\\\draw[xstep="..((1 * latex_scale_x)*4)..", ystep="..((1 * latex_scale_y)*4)..", color=gray]\n";
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
    grid.clear();
    --surface_stretch_blit(main.surface.surface, grid.surface, 0, 0, main.surface.width, main.surface.height, 0, 0, grid.width, grid.height);


    source = "\\\\begin{tikzpicture}\n";
    
    source = source .. "\\\\filldraw[line width=.1pt,color=black]\n (0, 0) -- (0.0, 0.01) -- (0.01, 0.01) -- cycle;\n";

    if (grid_visible == 1) then
	grid.forecolor = makecol(0,0,0);
	for x = 0, grid.width, cwx do
	    grid.line(x, 0, x, grid.height);
	end
	for y = 0, grid.height, cwy do
    	    grid.line(0, y, grid.width, y);
	end
	grid.forecolor = makecol(255,255,255);
    end
    
    for i=1,entity_count,1 do
	if type(entity[i]) ~= "nil" then
	if (entity[i].visible == 1) then
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
    source = ""..source;
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
