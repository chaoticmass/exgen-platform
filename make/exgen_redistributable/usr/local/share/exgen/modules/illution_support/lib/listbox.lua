listbox = {};
function listbox:new()
    local object = {};
    local daters = {};
    daters.surface = surface:new();
    daters.surface.width = 80;
    daters.surface.height = 20;
    daters.surface.row_height = 20;
    daters.surface.row_spacing = 4;
    daters.surface.rows = 1;
    daters.surface.backcolor = makecol(200, 200, 200);
    daters.surface.forecolor = makecol(255, 255, 255);
    daters.surface.enable_scrollbar = 1;
    daters.surface.enable_selection_highlight = 1;

    daters.surface.clear();
    daters.text = "";
    daters.value = daters.text;
    daters.startpos = 0;
    daters.textwidth = 0;
    daters.label = "";
    daters.surface.multi_line = 0;

    daters.my_vscroll = scrollbar:new();
    daters.surface.selected = 1;

    daters.surface.on_draw = function () 		
	daters.surface.clear()
	
	daters.surface.gradient(daters.surface.forecolor, daters.surface.backcolor);

	if (type(daters.my_vscroll) == "table") then
	    if (type(daters.surface.list) == "table") then
		list_height = ((table.getn(daters.surface.list)+1) * (daters.surface.row_height + daters.surface.row_spacing))-daters.surface.row_spacing;
--		echo(table.getn(daters.surface.list).."\n");
--		echo(((daters.surface.row_height + daters.surface.row_spacing))-daters.surface.row_spacing.."\n");
--		echo("List Height: "..list_height..", Height: "..daters.surface.height.." Row Height: "..daters.surface.row_height.." Scroll Value: ".. daters.my_vscroll.value .."\n");
		if (list_height > daters.surface.height) then
		    if daters.surface.enable_scrollbar == 1 then
			daters.my_vscroll.visible = 1;
		    end
		    --echo("my_vscroll: "..daters.my_vscroll.visible.."\n");
		    daters.my_vscroll.min = 0;
		    daters.my_vscroll.max = list_height;
		else 
		    daters.my_vscroll.visible = 0;
		end
	    end
	end
	
	if (type(daters.surface.list) == "table") then
	    index = 0;
	    for key, value in pairs(daters.surface.list) do
		xoffset = 4;
		yoffset = (index * (daters.surface.row_height+daters.surface.row_spacing)) + 4;
		if (yoffset+daters.surface.row_height - daters.my_vscroll.value < daters.surface.height) then
		    if index + 1 == daters.surface.selected then
			if (daters.surface.enable_selection_highlight == 1) then
			    daters.surface.draw_gradient(0, index * (daters.surface.row_height+daters.surface.row_spacing) - daters.my_vscroll.value, 
			    daters.surface.width, yoffset + daters.surface.row_height+1 - daters.my_vscroll.value,
			    getvar("colors", "select-forecolor"), getvar("colors", "select-backcolor"));
			end
		    end
		    daters.surface.print(xoffset, yoffset - (daters.my_vscroll.value), value);
		end
		index = index + 1;
	    end
	end
	

	if (daters.surface.has_focus() == 1) then
	    daters.surface.rect(1, 1, daters.surface.width - 2, daters.surface.height - 2);
	end
    end
    
    daters.surface.on_got_focus = function ()
	daters.surface.set_dirty();
	--daters.surface.on_draw();    
    end
    daters.surface.on_lost_focus = function ()
	daters.surface.clear();
	--daters.surface.on_draw();
	if (type(daters.surface.parent) == "table") then
	    --daters.surface.parent.set_dirty();
	end
	
    end

    daters.surface.on_mouse_click = function (button, x, y)
	daters.surface.set_focus();
	daters.surface.set_dirty();
    end


    daters.surface.on_mouse_up = function (button, x, y)
--	if daters.surface.parent.visible ~= 0 then
	
	daters.surface.set_focus();
	daters.surface.set_dirty();
	x = x - daters.surface.left;
	y = y - daters.surface.top;
	daters.last_selected = daters.surface.selected;
	if (type(daters.surface.list) == "table") then
	    index = 0;
	    for key, value in pairs(daters.surface.list) do
		xoffset = 4;
		yoffset = (index * (daters.surface.row_height+daters.surface.row_spacing)) + 4;
		if (yoffset+daters.surface.row_height - daters.my_vscroll.value < daters.surface.height) then
		    x1 = 0;
		    y1 = index * (daters.surface.row_height+daters.surface.row_spacing) - daters.my_vscroll.value;
		    x2 = daters.surface.width
		    y2 = yoffset + daters.surface.row_height+1 - daters.my_vscroll.value;
		    if x > x1 then
		        if x < x2 then
		    	    if y > y1 then
				if y < y2 then
				    daters.surface.selected = index + 1;
				    if daters.surface.selected ~= daters.last_selected then
					if (type(daters.surface.on_change) == "function") then
					    daters.surface.on_change();
					end
				    end
				end
			    end
			end
		    end
		end
		index = index + 1;
	    end
	end
    end
    
    daters.surface.on_keypress = function (keycode, keyval)
	--echo("key pressed!".. daters.surface.multi_line .."\n");
--[[	
	set_surface_dirty(daters.surface.surface);	
	if (keyval == 8) then
	    object.text = string.sub(object.text, 0, string.len(object.text) - 1);
	elseif (keyval == 13) then
	    if (daters.surface.multi_line == 1) then
		object.text = object.text .. string.char(10);	    
	    else 
	        if (type(daters.surface.on_return) == 'function') then
		    daters.surface.on_return();
		end	    
	    end
	elseif (keyval == 9) then
	    if (type(daters.surface.on_return) == 'function') then
		daters.surface.on_return();
	    end
	    index = next_focus();
	elseif (keyval == 27) then
	    if (type(daters.surface.on_escape) == 'function') then
		daters.surface.on_escape();
	    end	    
	elseif ((keyval > 31) and (keyval < 127)) then
	    object.text = object.text .. string.char(keyval);
	end]]
    end
    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "width" then
		return daters.surface.width;
	    elseif key == "text" then
		return daters.text;
	    elseif key == "startpos" then
		return daters.startpos;
	    elseif key == "nextline" then
		return daters.nextline;
	    else 
		return daters.surface[key];
	    end
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    --echo("setting "..key.."\n");
	    if key == "text" then
		--echo("setting text "..value.." oldvalue "..daters.text.."\n");
		daters.text = value;
		
		if (value ~= "") then
		    daters.textwidth = daters.surface.text_length(value);
		else
		    daters.textwidth = 0;
		end
		set_surface_dirty(daters.surface.surface);		
		daters.surface.on_draw();
	    elseif key == "value" then
		daters.text = value;
		if (value ~= "") then
		    daters.textwidth = daters.surface.text_length(value);
		else
		    daters.textwidth = 0;
		end
		--set_surface_dirty(daters.surface.surface);		
		--daters.surface.on_draw();
	    elseif key == "font" then
		daters.surface.font = value;
		daters.surface.row_height = daters.surface.text_height();
	    elseif key == "list" then
		daters.surface.list = value;
	    elseif key == "height" then
		daters.surface.height = value;
		daters.my_vscroll.height = value;
		--daters.surface.height = daters.surface.rows * daters.surface.row_height;
	    elseif key == "width" then
		daters.my_vscroll.left = value - daters.my_vscroll.width + 3;
		daters.my_vscroll.top = daters.surface.top;
		daters.my_vscroll.height = daters.surface.height;
		daters.surface.width = value;
	    elseif key == "rows" then
		daters.surface.rows = value;
		daters.surface.height = value * (daters.surface.row_height + daters.surface.row_spacing)+4;
		daters.my_vscroll.height = daters.surface.height;
	    elseif key == "selected" then
		daters.surface.selected = value;
		ypos =  (value * (daters.surface.row_height+daters.surface.row_spacing)) - 4 - daters.surface.row_height;
		if (ypos - daters.my_vscroll.value + daters.surface.row_height > daters.surface.height - 4) then
		    --daters.my_vscroll.value = ypos;
		    daters.my_vscroll.value = (value * (daters.surface.row_height+daters.surface.row_spacing)) - 4 - daters.surface.height + daters.surface.row_height; 
		end
		if (ypos - daters.my_vscroll.value < 0) then
		    daters.my_vscroll.value = (value * (daters.surface.row_height+daters.surface.row_spacing)) - 4 - daters.surface.row_height;
		end
		--echo(ypos..", ".. daters.my_vscroll.value .."\n");
	    elseif key == "parent" then
		daters.surface.parent = value;
		daters.my_vscroll.parent = value;
	    else
		
		if type(value) == "string" then
		    --echo(key.." = "..value.."\n");
		end
		daters.surface[key] = value;
	    end
	end	
    }
    );

    --daters.my_vscroll.visible = 0;
    daters.my_vscroll.width = 16;
    daters.my_vscroll.size = (daters.surface.height - daters.surface.margin_top - daters.surface.margin_bottom) / 10;
    daters.my_vscroll.max = (daters.surface.height - daters.surface.margin_top - daters.surface.margin_bottom);
    
    daters.my_vscroll.on_change = function ()
	---echo("scroll click\n");
	daters.surface.set_dirty();
    end
    
    daters.surface.value_exists = function(test_value) 
	exists = 0;
	for k, v in pairs(daters.surface.list) do
	    echo("test: "..test_value.." == "..v.."\n");
	    if test_value == v then
		exists = 1;
		return exists;
	    end
	end
	return exists;
    end
    
    object.can_have_focus = 1;        
    return object;
end

