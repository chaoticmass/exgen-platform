
mod_load("illution");

lua_init_script = [[
include("lib/application.lua");
include("lib/gui.lua");
include("lib/term.lua");
include("lib/db.lua");
]];
setvar("configuration", "lua-init-script", lua_init_script);
reinit();

set_debug_mode(0);

--initialize_video(800, 600);
-- 34x63
--laptop
--initialize_video(1280, 800);
--initialize_video(1280, 800-56);
--initialize_video(1280, 720);

--desktop
--initialize_video(1280, 1024-56);
--initialize_video(320, 240);

--littleiron
initialize_video(1024, 768);
--initialize_video(640,480);
--initialize_video(240, 320);

include("system/config/system_config.lua");
include("system/config/colors.lua");
include("system/config/desktop_config.lua");
include("lib/render3d.lua");
include("lib/libode.lua");
include("lib/latex.lua");
include("lib/serialize.lua");

mouse = mouse_cursor:new();

function round(num, idp)
  local mult = 10^(idp or 0)
  return math.floor(num * mult + 0.5) / mult
end

VERSION_BASE = "0.1";
VERSION_SUFFIX = "A";
VERSION = VERSION_BASE .. VERSION_SUFFIX;
set_window_title("Glowerk Project Designer v."..VERSION);

-- Settings
-- Paper Size
paper_width = 279.4 - 20;  -- mm, 11in (minus page margins)
paper_height = 215.9 - 20; -- mm  8.5in
canvas_width = paper_width - 10;
canvas_height = paper_height - 10;

-- End Settings




title_font = surface:new();
title_font.visible = 0;
title_font.bmp = "fonts/black_small_monospace.bmp";

body_font = surface:new();
body_font.visible = 0;
body_font.bmp = "fonts/black_micro_light.bmp";

-- intro
include("glowerk/intro.lua");


screen = surface:new(0);


latex_font = "felix";
tex = latex_interface:new();
tex.preview_source_head = [[
\\documentclass[landscape]{report}
\\usepackage{tikz}
\\usepackage{longtable}
\\usepackage{fancybox}
\\usepackage{lscape}
\\usepackage[landscape,pdftex,dvips,vmargin={1cm,1cm},hmargin={1cm,1cm}]{geometry}
\\usepackage[T1]{fontenc}
%\\font\\ttf]]..latex_font..[[ ]]..latex_font..[[ at32pt
\\pagestyle{empty}
\\begin{document}

\\noindent
\\fbox{
\\begin{minipage}[t][]]..canvas_height..[[mm]{]]..canvas_width..[[mm}
]]



tex.preview_source_foot = [[
\\end{document}
]]

tex.print_source_head = [[
\\documentclass{report}
\\usepackage{tikz}
\\usepackage{longtable}
\\usepackage{fancybox}
\\usepackage{lscape}
\\usepackage[pdftex,dvips,vmargin={1cm,1cm},hmargin={1cm,1cm}]{geometry}
\\usepackage[T1]{fontenc}
%\\font\\ttf]]..latex_font..[[ ]]..latex_font..[[ at32pt
\\pagestyle{empty}
\\begin{document}

]]



tex.print_source_foot = [[
\\end{document}
]]


tex.fonts = latex_font;



menubar = window:new();
menubar.draggable = 0;
menubar.font = title_font;
menubar.top = 0;
menubar.left = 0;
menubar.height = 48;
menubar.title = " - Glowerk Project Designer v"..VERSION;
menubar.caption = "Untitled Project ()"..menubar.title;
menubar.width = screen_width();
menubar.close.on_click = function()
    shutdown();
end


viewer = window:new();
viewer.draggable = 0;
viewer.top = menubar.top + menubar.height;
viewer.no_titlebar = 1;
viewer.margin_top = 3;
viewer.visible = 1
if (screen_width() < (canvas_width*4)) then
    viewer.width = (screen_width() / 3) * 2;--canvas_width*4 + viewer.margin_left + viewer.margin_right;
else
    viewer.width = canvas_width*4 + viewer.margin_left + viewer.margin_right;
end
viewer.height = screen_height() - viewer.top; --grid.height + viewer.margin_top + viewer.margin_bottom;
viewer.noevents = 1;

print_output = surface:new();
print_output.parent = viewer;
print_output.top = viewer.margin_top;
print_output.left = viewer.margin_left;
print_output.visible = 0;

pregrid = surface:new();
pregrid.parent = viewer;
pregrid.top = 0;
pregrid.left = viewer.margin_left;
pregrid.font = body_font;
pregrid.height = canvas_height*4;
pregrid.width = viewer.width - viewer.margin_left - viewer.margin_right - viewer.vscroll.width;--canvas_width*4;
pregrid.backcolor = makecol(128, 128, 128);
pregrid.clear();

grid = surface:new();
grid.parent = pregrid;
grid.top = viewer.margin_top;
grid.left = viewer.margin_left;
grid.font = body_font;
grid.height = canvas_height*4;
grid.width = canvas_width*4;
grid.backcolor = makecol(128, 128, 128);
grid.noevents = 1;

zoom = window:new();
zoom.font = title_font;
zoom.no_titlebar = 1;
zoom.close.visible = 0;
zoom.width = 60 + 6;
zoom.height = 60 + 6;
zoom.top = viewer.top + viewer.height - zoom.height;
zoom.left = viewer.left + viewer.width - zoom.width - viewer.vscroll.width;
zoom.zoom = surface:new();
zoom.zoom.parent = zoom;
zoom.zoom.top = 3;
zoom.zoom.left = 3;
zoom.zoom.width = 60;
zoom.zoom.height = 60;
zoom.before_redraw = function()
    zoom.zoom.set_dirty();
end

--zoom.on_redraw = function()
--    surface_stretch_blit(grid.surface, zoom.surface, mx - 15, my - 15, 30, 30, 3, 3, 60, 60);
---end


logo = surface:new();

--logo.gif = "glowerk/backgrounds/e_logo.gif";
logo.backcolor = makecol(255, 0, 255);
logo.clear();
logo.visible = 0;
logo.parent = viewer;
logo.left = ((viewer.width - viewer.margin_left - viewer.margin_right) * 0.5) - (logo.width * 0.5);
logo.top = viewer.margin_top+10;
--include("glowerk/illution_logo.lua");

-- glo-wire
glowire = {};
glowire.file_name = "";
glowire.name = "Untitled Project";
glowire.new_point = 0;
glowire.segments = {};
glowire.segment_count = 0;
glowire.current_segment = 0;

function reinitialize_glowire()
    function glowire:add_segment()
	segment = {
	    current_point = 0,
	    name = "Segment "..(table.getn(self.segments) + 1),
	    points = {},
	    index = 0,
	    thickness = 5,
	    red = 0,
	    green = 1,
	    blue = 0
	};
	function segment:add_point(ax, ay)
	    point = {
		x = ax,
		y = ay,
		name = "Point "..(table.getn(self.points) + 1),
		segment_index = self.current_segment;
	    };
	    table.insert(self.points, point);
	    self.current_point = table.getn(self.points);
	end

	table.insert(self.segments, segment);
	self.current_segment = table.getn(self.segments);
	self.current_point = 0;
	self.segments[self.current_segment].index = self.current_segment;
    end
    for segment_index, segment in ipairs(glowire.segments) do
        segment.add_point = function(self, ax, ay)
	    point = {
	        x = ax,
	        y = ay,
	        name = "Point "..(table.getn(self.points) + 1),
	        segment_index = self.current_segment;
	    };
	    table.insert(self.points, point);
	    self.current_point = table.getn(self.points);
	end
    end

end

--glowire:add_segment = function()
function glowire:add_segment()
    segment = {
        current_point = 0,
        name = "Segment "..(table.getn(self.segments) + 1),
        points = {},
	index = 0,
	thickness = 5,
	red = 0,
	green = 1,
	blue = 0
    };
    function segment:add_point(ax, ay)
        point = {
	    x = ax,
	    y = ay,
	    name = "Point "..(table.getn(self.points) + 1),
	    segment_index = self.current_segment;
	};
	table.insert(self.points, point);
	self.current_point = table.getn(self.points);
    end

    table.insert(self.segments, segment);
    self.current_segment = table.getn(self.segments);
    self.current_point = 0;
    self.segments[self.current_segment].index = self.current_segment;
end

save_project = function()
    if (glowire.file_name == "") then
	file_name.mode = "save";
	file_name.visible = 1;
	file_name.z = 9999;
	file_name.textbox.set_focus();
    else 
	output = "";
        output = save(output, "glowire", glowire);
        if (type(logo.gif) ~= "nil") then
	    output = output .. "logo.gif = \""..logo.gif.."\"\n";
	else
	    output = output .. "logo.gif = nil\n";
	end
        savefile(glowire.file_name..".glo", output);
        menubar.caption = glowire.name.." ("..glowire.file_name..")"..menubar.title;
	menubar.set_dirty();
    end
end

load_project = function(file_name)
    logo.clear();
    include(file_name..".glo");
    reinitialize_glowire();
    viewer.before_draw();
    menubar.caption = glowire.name.." ("..glowire.file_name..")"..menubar.title;
    menubar.set_dirty();
    sel.update_list();
end

glowire:add_segment();

--[[glowire:add_segment();
glowire.segments[2]:add_point(10, 10);
glowire.segments[2]:add_point(20, 10);
glowire.segments[2]:add_point(20, 20);]]
-- end glo-wire

include("glowerk/io.lua");




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
grid.transparent = 1;
grid.backcolor = makecol(255, 0, 255);
pregrid.alpha = 128;
viewer.vscroll.visible = 1;

update_preview_source = function()
    source = "\\\\begin{tikzpicture}\n";
    source = source .. "\\\\filldraw[line width=.1pt,color=black]\n (0, 0) -- (0.0, 0.01) -- (0.01, 0.01) -- cycle;\n";
    for segment_index, segment in ipairs(glowire.segments) do
	source = source .. "\\\\definecolor{currentcolor}{rgb}{"..segment.red..", "..segment.green..", "..segment.blue.."}\n";
	source = source .. "\\\\draw[line width="..segment.thickness.."mm, color=currentcolor, join=round, cap=round]\n";
	segment.length = 0;
	for point_index, point in ipairs(segment.points) do
	    if (type(segment.points[point_index + 1]) ~= "nil") then
		next_point = segment.points[point_index + 1];
		thickness = segment.thickness;
		source = source .. "(".. (point.x*latex_scale_x) ..", "..0-(point.y*latex_scale_y)..") -- ".. "(".. (next_point.x*latex_scale_x) ..", "..0-(next_point.y*latex_scale_y)..") -- "
		h = point.x - next_point.x;
		v = point.y - next_point.y;
		distance = math.sqrt((h*h) + (v*v)) * print_scale;
		segment.length = segment.length + distance;
	    else
		source = source .. "(".. (point.x*latex_scale_x) ..", "..0-(point.y*latex_scale_y)..") -- ".. "(".. (point.x*latex_scale_x) ..", "..0-(point.y*latex_scale_y)..");\n"
	    end
	end
    end
    
    if (grid_visible == 1) then
	source = source .. grid_source;
    end
    source = source .. "\\\\end{tikzpicture}\\\\end{minipage}}";
    source = source .. [[ \\begin{landscape} \\section*{\\usefont{T1}{bch}{m}{n}Segments} 
    \\usefont{T1}{bch}{m}{n}
    \\begin{longtable}{|l|r|}
    ]];
    for segment_index, segment in ipairs(glowire.segments) do
	source = source .. [[ ]]..segment.name..[[ %26 ]].. round(segment.length, 2)..[[ cm \\\\]];
    end
    source = source .. [[\\end{longtable} \\end{landscape} ]];
    tex.source = tex.preview_source_head..source..tex.preview_source_foot;
end

update_print_source = function()
    source = [[
\\begin{landscape} 
\\noindent
\\fbox{
\\begin{minipage}[t][]]..canvas_height..[[mm]{]]..canvas_width..[[mm}
\\begin{tikzpicture}
    ]];
    source = source .. "\\\\filldraw[line width=.1pt,color=black]\n (0, 0) -- (0.0, 0.01) -- (0.01, 0.01) -- cycle;\n";
    for segment_index, segment in ipairs(glowire.segments) do
	source = source .. "\\\\definecolor{currentcolor}{rgb}{"..segment.red..", "..segment.green..", "..segment.blue.."}\n";
	source = source .. "\\\\draw[line width="..segment.thickness.."mm, color=currentcolor, join=round, cap=round]\n";
	segment.length = 0;
	for point_index, point in ipairs(segment.points) do
	    if (type(segment.points[point_index + 1]) ~= "nil") then
		next_point = segment.points[point_index + 1];
		thickness = segment.thickness;
		source = source .. "(".. (point.x*latex_scale_x) ..", "..0-(point.y*latex_scale_y)..") -- ".. "(".. (next_point.x*latex_scale_x) ..", "..0-(next_point.y*latex_scale_y)..") -- "
		h = point.x - next_point.x;
		v = point.y - next_point.y;
		distance = math.sqrt((h*h) + (v*v)) * print_scale;
		segment.length = segment.length + distance;
	    else
		source = source .. "(".. (point.x*latex_scale_x) ..", "..0-(point.y*latex_scale_y)..") -- ".. "(".. (point.x*latex_scale_x) ..", "..0-(point.y*latex_scale_y)..");\n"
	    end
	end
    end
    
    if (grid_visible == 1) then
	source = source .. grid_source;
    end
    source = source .. [[\\end{tikzpicture}\\end{minipage} } \\end{landscape} ]];
    source = source .. [[ \\section*{\\usefont{T1}{bch}{m}{n}Segments} 
    \\usefont{T1}{bch}{m}{n}
    \\begin{longtable}{|l|r|}
    ]];
    for segment_index, segment in ipairs(glowire.segments) do
	source = source .. [[ ]]..segment.name..[[ %26 ]].. round(segment.length, 2)..[[ cm \\\\]];
    end
    source = source .. [[\\end{longtable}]];
    tex.source = tex.print_source_head..source..tex.print_source_foot;
end


grid.on_draw = function()
--    mx, my = mouse_xy();
--    mx = mx - viewer.left;
--    my = my - viewer.top - viewer.y_offset;
    mx = 30;
    my = 30;
    if (glowire.current_segment ~= 0) then
	if (glowire.current_point ~= 0) then
	    if (glowire.current_point <= table.getn(glowire.segments[glowire.current_segment].points)) then
		mx = glowire.segments[glowire.current_segment].points[glowire.current_point].x * scale;
		my = glowire.segments[glowire.current_segment].points[glowire.current_point].y * scale;
		if my < 30 then
		    my = 31;
		end
		if my > grid.height - 30 then
		    my = grid.height - 31;
		end
	    end
	end
    end
    
    grid.clear();
    pregrid.clear();
    --surface_blit(logo.surface, grid.surface, 0, 0, (grid.width * 0.5) - (logo.width * 0.5), (grid.height * 0.5) - (logo.height * 0.5),  logo.width, logo.height);
    padding = 60;
    gh = grid.height - padding;
    gw = grid.width - padding;
    if ((gw / logo.width) * logo.height > gh) then
	surface_stretch_blit(logo.surface, grid.surface, 0, 0, logo.width, logo.height, (gw * 0.5) - (((gh / logo.height) * logo.width) * 0.5), padding*0.5, (gh / logo.height) * logo.width, gh);
    else 
	surface_stretch_blit(logo.surface, grid.surface, 0, 0, logo.width, logo.height, padding*0.5, padding*0.5+(gh * 0.5) - (((gw / logo.width) * logo.height) * 0.5), gw, (gw / logo.width) * logo.height);
    end
    --grid.forecolor = makecol(255, 255, 255);
    --grid.rect_fill(grid.width - 62, grid.height - 62, grid.width, grid.height);
    --surface_stretch_blit(grid.surface, grid.surface, mx - 15, my - 15, 30, 30, grid.width - 60, grid.height - 60, 60, 60);


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

    grid.forecolor = makecol(255, 255, 255);
    if (mx < grid.width - 30) and (my < grid.height - 30) and (mx > 30) and (my > 30) then
	grid.forecolor = makecol(0, 255, 0);
	grid.rect_fill(mx, my, mx, my);
	surface_stretch_blit(grid.surface, zoom.zoom.surface, mx - 15, my - 15, 30, 30, 3, 3, 60, 60);
        zoom.zoom.forecolor = makecol(0, 255, 0);
        --zoom.zoom.thick_line(0, 30, 60, 30, 1);
        --zoom.zoom.thick_line(30, 0, 30, 60, 1);

        if ((my >= viewer.height - 122) and (mx >= viewer.width - 122)) then
	    if (zoom.top ~= viewer.top) then
		zoom.top = viewer.top;
	    end
	else
	    if (zoom.top ~= viewer.top + viewer.height - zoom.height) then
	        zoom.top = viewer.top + viewer.height - zoom.height;
	    end
	end
    end


    for segment_index, segment in ipairs(glowire.segments) do
	for point_index, point in ipairs(segment.points) do
	    
	    grid.rect((point.x*scale) - 2, (point.y*scale) - 2, (point.x*scale) + 2, (point.y*scale) + 2);
	    if (segment_index == glowire.current_segment) then
		if (point_index == glowire.current_point) then
		    grid.forecolor = makecol(64, 64, 255);
		    grid.rect((point.x*scale) - 8, (point.y*scale) - 8, (point.x*scale) + 8, (point.y*scale) + 8);
		    grid.rect((point.x*scale) - 7, (point.y*scale) - 7, (point.x*scale) + 7, (point.y*scale) + 7);
		end
	    end
	    
	    if (type(segment.points[point_index + 1]) ~= "nil") then
		next_point = segment.points[point_index + 1];
		
		thickness = segment.thickness;
		grid.forecolor = makecol(segment.red * 255, segment.green * 255, segment.blue * 255);
		grid.thick_line(point.x*scale, point.y*scale, next_point.x*scale, next_point.y*scale, thickness*1.5);

		if (segment_index == glowire.current_segment) then
		    grid.forecolor = makecol(255 - segment.red * 255, 255-segment.green * 255, 255-segment.blue * 255);
		    grid.thick_line(point.x*scale, point.y*scale, next_point.x*scale, next_point.y*scale, thickness * .5);
		end
		
		--grid.line(point.x*scale, point.y*scale, next_point.x*scale, next_point.y*scale);
	    end
	end
    end
end

viewer.before_draw = function()
    pregrid.set_dirty();
    grid.set_dirty();
    grid.on_draw();
    print_output.set_dirty();
    viewer.vscroll.max = pregrid.height;
    viewer.vscroll.set_dirty();
--    term_window.z = 9999;
--    sel.set_dirty();
end

include("glowerk/selector.lua");

load_project("glowerk_logo");
intro_fadeout();
render_loop();