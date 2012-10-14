

sel = window:new();
sel.close.visible = 0;
sel.font = title_font;
sel.caption = "Selection:";
sel.draggable = 0;
sel.top = menubar.top + menubar.height;
sel.left = viewer.left + viewer.width;
sel.height = viewer.height / 4;
sel.width = screen_width() - sel.left;
sel.update_list = function()
    list.list = {};
    for segment_index, segment in ipairs(glowire.segments) do
	list.list[table.getn(list.list)+1] = "Segment "..segment_index.." ("..segment.name..")";
	segment.list_index = table.getn(list.list);
	if (segment_index == glowire.current_segment) then
	    if (glowire.current_point == 0) then
		list.selected = segment.list_index;
--		glowire.current_point = 1;
--		segment.current_point = 1;
	    end
	end
	for point_index, point in ipairs(segment.points) do
    	    list.list[table.getn(list.list)+1] = "   Point "..point_index.." ("..point.name..")";
	    point.list_index = table.getn(list.list);
	    if (glowire.current_segment ~= 0) then
		if (point_index == glowire.segments[glowire.current_segment].current_point) then
		    if (glowire.current_segment == segment_index) then
			list.selected = point.list_index;
		    end
		end
	    end
	end
    end
    --list.selected = table.getn(list.list);
    edit.update_properties();
    list.set_dirty();
end


sel.before_draw = function ()
    list.set_dirty();
    button1.set_dirty();
    button2.set_dirty();
    button3.set_dirty();
    button4.set_dirty();
end

button1 = button:new();
button1.parent = sel;
button1.font = body_font;
button1.caption = "New Segment";
button1.top = sel.align_top(button1);
button1.left = sel.align_left(button1);
button1.on_mouse_up = function(button, x, y)
    glowire:add_segment();
    sel.update_list();
end

button2 = button:new();
button2.parent = sel;
button2.top = button1.top;
button2.font = body_font;
button2.caption = "Del Segment";
button2.left = sel.align_left(button2);
button2.on_mouse_up = function(button, x, y)
    for i = glowire.current_segment, table.getn(glowire.segments) - 1, 1 do
	glowire.segments[i] = glowire.segments[i+1];
    end
    glowire.current_segment = 1;
    --glowire.segments[table.getn(glowire.segments)] = nil;
    table.remove(glowire.segments, table.getn(glowire.segments));
    glowire.current_segment = table.getn(glowire.segments);
    if (glowire.current_segment > 0) then
	glowire.current_point = glowire.segments[glowire.current_segment].current_point;
    end
    sel.update_list();
end


button3 = button:new();
button3.parent = sel;
button3.top = button1.top;
button3.font = body_font;
button3.caption = "New Point";
button3.left = sel.align_left(button3);
button3.on_mouse_up = function(button, x, y)
    glowire.segments[glowire.current_segment]:add_point(0, 0);
    sel.update_list();
end

button4 = button:new();
button4.parent = sel;
button4.top = button1.top;
button4.font = body_font;
button4.caption = "Del Point";
button4.left = sel.align_left(button4);
button4.on_mouse_up = function(button, x, y)
    if (glowire.current_segment > 0) then
	for i = glowire.current_point, table.getn(glowire.segments[glowire.current_segment].points) - 1, 1 do
	    glowire.segments[glowire.current_segment].points[i] = glowire.segments[glowire.current_segment].points[i+1];
	end
	--glowire.segments[glowire.current_segment].points[table.getn(glowire.segments[glowire.current_segment].points)] = nil;
	table.remove(glowire.segments[glowire.current_segment].points, table.getn(glowire.segments[glowire.current_segment].points));
	--glowire.current_segment = 0;
	glowire.current_point = table.getn(glowire.segments[glowire.current_segment].points);
	glowire.segments[glowire.current_segment].current_point = glowire.current_point;
	sel.update_list();
    end
end


list = listbox:new();
list.parent = sel;
list.font = body_font;
list.top = sel.align_top(list);
list.left = sel.margin_left;
list.height = sel.height - sel.margin_top - sel.margin_bottom - 22;
list.width = sel.width - sel.margin_left - sel.margin_right;
list.on_change = function()
    edit.update_properties();
    if (selected_type == "point") then
	glowire.new_point = 1;
    end
end


edit = window:new();
edit.draggable = 0;
edit.close.visible = 0;
edit.font = title_font;
edit.caption = "Properties";
edit.width = sel.width;
edit.top = sel.top + sel.height+1;
edit.height = viewer.height - edit.top+viewer.top;
edit.left = sel.left;
edit.before_draw = function()
    edit.name.set_dirty();
    edit.name_label.set_dirty();
    edit.thickness.set_dirty();
    edit.thickness_label.set_dirty();
    edit.colorr.set_dirty();
    edit.colorr_label.set_dirty();
    edit.colorg.set_dirty();
    edit.colorg_label.set_dirty();
    edit.colorb.set_dirty();
    edit.colorb_label.set_dirty();

end

edit.name_label = label:new();
edit.name_label.parent = edit;
edit.name_label.top = edit.margin_top;
edit.name_label.left = edit.margin_left;
edit.name_label.font = body_font;
edit.name_label.caption = "Name: ";
edit.name_label.transparent = 1;
edit.name_label.backcolor = makecol(255, 0, 255);
edit.name = textbox:new();
edit.name.parent = edit;
edit.name.top = edit.margin_top;
edit.name.left = edit.margin_left+ (edit.width/3);
edit.name.font = body_font;
edit.name_label.height = edit.name.height;
edit.name.width = edit.width - edit.name.left - edit.margin_right;

edit.thickness_label = label:new();
edit.thickness_label.parent = edit;
edit.thickness_label.top = edit.name_label.height+edit.name_label.top+4;
edit.thickness_label.left = edit.margin_left;
edit.thickness_label.font = body_font;
edit.thickness_label.caption = "Thickness: ";
edit.thickness_label.transparent = 1;
edit.thickness_label.backcolor = makecol(255, 0, 255);

edit.thickness = textbox:new();
edit.thickness.parent = edit;
edit.thickness.top = edit.thickness_label.top;
edit.thickness.left = edit.margin_left+ (edit.width/3);
edit.thickness.font = body_font;
edit.thickness_label.height = edit.thickness.height;
edit.thickness.width = edit.width - edit.thickness.left - edit.margin_right;
edit.thickness.visible = 0;
edit.thickness_label.visible = 0;

edit.colorr_label = label:new();
edit.colorr_label.parent = edit;
edit.colorr_label.top = edit.thickness_label.height+edit.thickness_label.top+4;
edit.colorr_label.left = edit.margin_left;
edit.colorr_label.font = body_font;
edit.colorr_label.caption = "Red: ";
edit.colorr_label.transparent = 1;
edit.colorr_label.backcolor = makecol(255, 0, 255);
edit.colorr = textbox:new();
edit.colorr.parent = edit;
edit.colorr.top = edit.colorr_label.top;
edit.colorr.left = edit.margin_left+ (edit.width/3);
edit.colorr.font = body_font;
edit.colorr_label.height = edit.colorr.height;
edit.colorr.width = edit.width - edit.colorr.left - edit.margin_right;
edit.colorr.visible = 0;
edit.colorr_label.visible = 0;

edit.colorg_label = label:new();
edit.colorg_label.parent = edit;
edit.colorg_label.top = edit.colorr_label.height+edit.colorr_label.top+4;
edit.colorg_label.left = edit.margin_left;
edit.colorg_label.font = body_font;
edit.colorg_label.caption = "Green: ";
edit.colorg_label.transparent = 1;
edit.colorg_label.backcolor = makecol(255, 0, 255);
edit.colorg = textbox:new();
edit.colorg.parent = edit;
edit.colorg.top = edit.colorg_label.top;
edit.colorg.left = edit.margin_left+ (edit.width/3);
edit.colorg.font = body_font;
edit.colorg_label.height = edit.colorg.height;
edit.colorg.width = edit.width - edit.colorg.left - edit.margin_right;
edit.colorg.visible = 0;
edit.colorg_label.visible = 0;

edit.colorb_label = label:new();
edit.colorb_label.parent = edit;
edit.colorb_label.top = edit.colorg_label.height+edit.colorg_label.top+4;
edit.colorb_label.left = edit.margin_left;
edit.colorb_label.font = body_font;
edit.colorb_label.caption = "Blue: ";
edit.colorb_label.transparent = 1;
edit.colorb_label.backcolor = makecol(255, 0, 255);
edit.colorb = textbox:new();
edit.colorb.parent = edit;
edit.colorb.top = edit.colorb_label.top;
edit.colorb.left = edit.margin_left+ (edit.width/3);
edit.colorb.font = body_font;
edit.colorb_label.height = edit.colorb.height;
edit.colorb.width = edit.width - edit.colorb.left - edit.margin_right;
edit.colorb.visible = 0;
edit.colorb_label.visible = 0;

selected_type = "";
selected_index = 0;
selected_segment = {};
selected_point = {};

calculate_segment_length = function(s)
    for point_index, point in ipairs(s.points) do
	
    end
end

edit.update_properties = function()
--    echo("Update Properties, 
    for segment_index, segment in ipairs(glowire.segments) do
	if segment.list_index == list.selected then
	    echo("Segment Selected\n");
	    selected_type = "segment";
	    selected_index = segment_index;
	    selected_segment = segment;
	    calculate_segment_length(segment);
	    break;
	end
	for point_index, point in ipairs(segment.points) do
	    if point.list_index == list.selected then
		echo("Point Selected\n");
		selected_type = "point";
		selected_index = point_index;
		selected_point = point;
		glowire.current_segment = segment_index;
		break;
	    end
	end
    end

    if (selected_type == "segment") then
	glowire.current_segment = selected_index;
	edit.name.text = selected_segment.name;
	edit.thickness.text = selected_segment.thickness;
	edit.colorr.text = selected_segment.red;
	edit.colorg.text = selected_segment.green;
	edit.colorb.text = selected_segment.blue;
	edit.thickness.visible = 1;
	edit.thickness_label.visible = 1;
	edit.colorr.visible = 1;
	edit.colorr_label.visible = 1;
	edit.colorg.visible = 1;
	edit.colorg_label.visible = 1;
	edit.colorb.visible = 1;
	edit.colorb_label.visible = 1;
	--edit.segment_length = selected_segment.length;
    elseif (selected_type == "point") then
	glowire.current_point = selected_index;
	edit.name.text = selected_point.name;
	edit.thickness.visible = 0;
	edit.thickness_label.visible = 0;
	edit.colorr.visible = 0;
	edit.colorr_label.visible = 0;
	edit.colorg.visible = 0;
	edit.colorg_label.visible = 0;
	edit.colorb.visible = 0;
	edit.colorb_label.visible = 0;
    end
    echo("Selected Segment: "..glowire.current_segment..", Point: "..glowire.current_point.."\n");
    edit.set_dirty();
    viewer.before_draw();
end

edit.name.on_return = function()
    if (selected_type == "segment") then
	selected_segment.name = edit.name.text;
    elseif (selected_type == "point") then
	selected_point.name = edit.name.text;
    end
    sel.update_list();
end

edit.thickness.on_return = function()
    selected_segment.thickness = edit.thickness.text;
    viewer.before_draw();
end

edit.colorr.on_change = function()
    if (type(tonumber(edit.colorr.text)) ~= "nil") then
	if tonumber(edit.colorr.text) >= 0 then
	    if tonumber(edit.colorr.text) <= 1 then
		selected_segment.red = tonumber(edit.colorr.text);
	    end
        end
    end
    viewer.before_draw();
end

edit.colorg.on_change = function()
    if (type(tonumber(edit.colorg.text)) ~= "nil") then
	if tonumber(edit.colorg.text) >= 0 then
	    if tonumber(edit.colorg.text) <= 1 then
		selected_segment.green = tonumber(edit.colorg.text);
	    end
        end
    end
    viewer.before_draw();
end

edit.colorb.on_change = function()
    if (type(tonumber(edit.colorb.text)) ~= "nil") then
	if tonumber(edit.colorb.text) >= 0 then
	    if tonumber(edit.colorb.text) <= 1 then
		selected_segment.blue = tonumber(edit.colorb.text);
	    end
        end
    end
    viewer.before_draw();
end

menubar.before_draw = function()
    mnufile.set_dirty();
    mnuedit.set_dirty();
    edit_menu.set_dirty();
    file_menu.set_dirty();
end

mnufile = label:new();
mnufile.parent = menubar;
mnufile.font = body_font;
mnufile.top = menubar.margin_top;
mnufile.left = menubar.margin_left;
mnufile.backcolor = makecol(255, 0, 255);
mnufile.transparent = 1;
mnufile.caption = "File";
mnufile.on_mouse_up = function(button, x, y)
    if (file_menu.visible == 1) then
	file_menu.visible = 0;
    else 
	file_menu.z = 9999;
        file_menu.set_focus();
	file_menu.visible = 1;
	edit_menu.visible = 0;
    end
end

mnuedit = label:new();
mnuedit.parent = menubar;
mnuedit.font = body_font;
mnuedit.top = menubar.margin_top;
mnuedit.left = menubar.margin_left + mnufile.width + mnufile.left;
mnuedit.backcolor = makecol(255, 0, 255);
mnuedit.transparent = 1;
mnuedit.caption = "Edit";
mnuedit.on_mouse_up = function(button, x, y) 
    if (edit_menu.visible == 1) then
	edit_menu.visible = 0;
    else
	edit_menu.z = 9999;
        edit_menu.set_focus();
        edit_menu.visible = 1;
        file_menu.visible = 0;
    end
end


file_menu = listbox:new();
file_menu.visible = 0;
file_menu.enable_scrollbar = 0;
file_menu.enable_selection_highlight = 0;
file_menu.font = body_font;
file_menu.list = {"New", "", "Save", "Save As", "Open", "", "Print", "", "Exit"};
file_menu.parent = screen;
file_menu.forecolor = makecol(190, 190, 190);
file_menu.backcolor = makecol(128, 128, 128);
file_menu.top = menubar.top + menubar.height;
file_menu.left = mnufile.left;
file_menu.z = 9999;
file_menu.height = (table.getn(file_menu.list)+1) * (file_menu.text_height() + 3);
file_menu.width = 90;
file_menu.on_mouse_click = function()
    if (file_menu.list[file_menu.selected] == "New") then
	glowire.file_name = "";
	file_name.mode = "save";
	file_name.caption = "New Project";
	glowire.name = "Untitled Project";
	glowire.segments = {};
	glowire.current_segment = 0;
	glowire.current_point = 0;
	grid.on_draw();
	reinitialize_glowire();
	logo.gif = nil;
	logo.clear();
	save_project();
    elseif (file_menu.list[file_menu.selected] == "Save") then
	save_project();
    elseif (file_menu.list[file_menu.selected] == "Save As") then
	file_name.mode = "save";
	file_name.caption = "Save As";
	file_name.visible = 1;
	file_name.z = 9999;
	file_name.textbox.set_focus();
    elseif (file_menu.list[file_menu.selected] == "Open") then
	file_name.mode = "open";
	file_name.caption = "Open Project";
	file_name.visible = 1;
	file_name.z = 9999;
	file_name.textbox.set_focus();
    elseif (file_menu.list[file_menu.selected] == "Print") then
	if (glowire.file_name == "") then
	    save_project();
	else
	    update_print_source();
	    tex.output_format = 'pdf';
	    tex.create_document();
	    tex.download_pdf();
	    id = tex.id;
--	    tex.destroy_document();
	    os.rename(id..".pdf", glowire.file_name..".pdf");
	    os.execute("xpdf "..glowire.file_name..".pdf");
	end
    elseif (file_menu.list[file_menu.selected] == "Exit") then
	stop_render();
    end
    file_menu.visible = 0;
end

file_name = window:new();
file_name.close.visible = 0;
file_name.visible = 0;
file_name.font = title_font;
file_name.width = 320;
file_name.height = 200;
file_name.top = (screen_height() * 0.5) - (file_name.height * 0.5);
file_name.left = (screen_width() * 0.5) - (file_name.width * 0.5);
file_name.caption = "Save As";
file_name.label = label:new();
file_name.label.parent = file_name;
file_name.label.font = body_font;
file_name.label.top = file_name.margin_top;
file_name.label.left = file_name.margin_left;
file_name.label.backcolor = makecol(255, 0, 255);
file_name.label.transparent = 1;
file_name.label.caption = "File Name: ";

file_name.textbox = textbox:new();
file_name.textbox.parent = file_name;
file_name.textbox.font = body_font;
file_name.textbox.top = file_name.margin_top;
file_name.textbox.left = file_name.label.width + file_name.margin_left;
file_name.textbox.width = file_name.width - file_name.margin_right - file_name.textbox.left;
file_name.textbox.backcolor = makecol(255, 0, 255);
file_name.textbox.transparent = 1;
file_name.textbox.text = glowire.file_name;

file_name.button = button:new();
file_name.button.parent = file_name;
file_name.button.font = body_font;
file_name.button.caption = "OK";
file_name.button.top = file_name.textbox.top + file_name.textbox.height + 8;
file_name.button.left = (file_name.width * 0.5) - (file_name.button.width * 0.5);
file_name.height = file_name.button.top + file_name.button.height+8;

file_name.button.on_click = function(button, x, y) 
    file_name.visible = 0;
    file_name.z = -1;
    glowire.file_name = file_name.textbox.text;
    if (file_name.mode == "save") then
	save_project();
    elseif (file_name.mode == "open") then
	load_project(glowire.file_name);
    end
end

function file_name.before_draw()
    file_name.textbox.set_dirty()
    file_name.label.set_dirty();
    file_name.button.set_dirty();
end

edit_menu = listbox:new();
edit_menu.visible = 0;
edit_menu.enable_scrollbar = 0;
edit_menu.enable_selection_highlight = 0;
edit_menu.font = body_font;
edit_menu.list = {"Project Properties", "Background Image"};
edit_menu.parent = screen;
edit_menu.forecolor = makecol(190, 190, 190);
edit_menu.backcolor = makecol(128, 128, 128);
edit_menu.top = menubar.top + menubar.height;
edit_menu.left = mnuedit.left;
edit_menu.z = 9999;
edit_menu.height = (table.getn(edit_menu.list)+1) * (edit_menu.text_height() + 3);
edit_menu.width = 120;
edit_menu.on_mouse_click = function()
    if (edit_menu.list[edit_menu.selected] == "Background Image") then
	edit_bg.visible = 1;
	edit_bg.z = 9999;
    elseif (edit_menu.list[edit_menu.selected] == "Project Properties") then
	project.visible = 1;
	project.z = 9999;
    end
    edit_menu.visible = 0;
end

edit_bg = window:new();
edit_bg.font = title_font;
edit_bg.visible = 0;
edit_bg.caption = "Background Image";
edit_bg.height = 170;
edit_bg.left = (screen_width() / 2) - (edit_bg.width / 2);
edit_bg.top = (screen_height() / 2) - (edit_bg.height / 2);
edit_bg.close.on_mouse_up = function()
    edit_bg.visible = 0;
    edit_bg.z = -1;
end
edit_bg.before_draw = function()
    edit_bg.label1.set_dirty();
    edit_bg.list1.set_dirty();
    edit_bg.button1.set_dirty();
end
edit_bg.label1 = label:new();
edit_bg.label1.parent = edit_bg;
edit_bg.label1.font = body_font;
edit_bg.label1.top = edit_bg.margin_top;
edit_bg.label1.left = edit_bg.margin_left;
edit_bg.label1.caption = "Image File:";
edit_bg.label1.transparent = 1;
edit_bg.label1.backcolor = makecol(255, 0, 255);

edit_bg.list1 = listbox:new();
edit_bg.list1.parent = edit_bg;
edit_bg.list1.font = body_font;
edit_bg.list1.top = edit_bg.margin_top;
edit_bg.list1.left = edit_bg.margin_left + edit_bg.label1.width;
edit_bg.list1.width = edit_bg.width - edit_bg.list1.left-6;
edit_bg.list1.height = 100;
edit_bg.list1.list = list_files("glowerk/backgrounds/");

edit_bg.button1 = button:new();
edit_bg.button1.parent = edit_bg;
edit_bg.button1.font = body_font;
edit_bg.button1.caption = "Apply";
edit_bg.button1.top = edit_bg.list1.top + edit_bg.list1.height + 6;
edit_bg.button1.left = (edit_bg.width / 2) - (edit_bg.button1.width / 2)

edit_bg.button1.on_mouse_click = function()
    if edit_bg.visible == 1 then
	echo("\n\nSelecting Image: "..edit_bg.list1.list[edit_bg.list1.selected].."\n");
	logo.gif = "glowerk/backgrounds/"..edit_bg.list1.list[edit_bg.list1.selected];
	edit_bg.visible = 0;
	edit_bg.z = -1;
        viewer.before_draw();
    end
end

project = window:new();
project.font = title_font;
project.caption = "Project Properties";
project.top = (screen_height() * 0.5) - (project.height * 0.5);
project.left = (screen_width() * 0.5) - (project.width * 0.5);

i = 1;
project.label = {};
project.textbox = {};
project.label[i] = label:new();
project.label[i].parent = project;
project.label[i].font = body_font;
project.label[i].top = project.margin_top;
project.label[i].left = project.margin_left+6;
project.label[i].caption = "Title:";
project.label[i].backcolor = makecol(255, 0, 255);
project.label[i].transparent = 1;
project.textbox[i] = textbox:new();
project.textbox[i].parent = project;
project.textbox[i].font = body_font;
project.textbox[i].text = glowire.name;
project.textbox[i].width = (project.width / 3)*2-6;
project.textbox[i].left = project.width - project.textbox[i].width - project.margin_left - project.margin_right;
project.label[i].height = project.textbox[i].height;
project.textbox[i].top = project.label[i].top;
project.control_count = i;

project.close.on_click = function()
    project.visible = 0;
end

project.submit = button:new();
project.submit.parent = project;
project.submit.font = body_font;
project.submit.caption = "Submit";
project.submit.top = project.height - project.submit.height - 6;
project.submit.left = (project.width * 0.5) - (project.submit.width * 0.5)

project.before_draw = function()
    for i = 1, project.control_count, 1 do
	project.label[i].set_dirty();
	project.textbox[i].set_dirty();
    end
    project.submit.set_dirty();
end

project.submit.on_click = function()

    glowire.name = project.textbox[1].text;
    menubar.caption = glowire.name.." ("..glowire.file_name..")"..menubar.title;
    menubar.set_dirty();

    project.visible = 0;
end

project.visible = 0;