listbox = {};
function listbox:new()
    local object = {};
    local daters = {};
    daters.surface = surface:new();
    daters.surface.width = 60;
    daters.surface.height = 20;
    daters.surface.backcolor = makecol(255, 255, 255);
    daters.icons = {};
    daters.surface.clear();
    daters.surface.arrange_vertical = 1;
    daters.surface.arrange_auto = 1;
    daters.surface.icon_width = 128;
    daters.surface.icon_height = 32;
    daters.surface.icon_count = 0;
    daters.last_index = 0;
    daters.last_click = timer();
    daters.dragging = 0;
    daters.surface.scroll_position = 0;
    daters.surface.scroll_min = 0;    
    daters.surface.scroll_max = 0;    
    daters.surface.images = {};
    daters.surface.selected = -1;
    daters.surface.selectedcolor = makecol(200,200,255);
    daters.surface.unselectedcolor = makecol(255, 255, 255);
    
    vscroll = scrollbar:new();
--    vscroll.parent = daters.surface;
    vscroll.visible = 0;
    vscroll.max = daters.surface.scroll_max;
    vscroll.size = 20;
    vscroll.value = 0;
    
    vscroll.on_change = function ()
	daters.surface.scroll_position = 0 - vscroll.value;
    end
    
    vscroll.on_mouse_up = function()
	daters.surface.set_dirty();
	daters.surface.on_redraw();
    end
    
    daters.get_icon_from_xy = function (x, y)

	if (daters.surface.arrange_vertical ~= 0) then
	    o_x = 0;
	    o_y = 0 - daters.surface.scroll_position;
	else
	    o_x = 0;
	    o_y = 0 - daters.surface.scroll_position;
	end
	x = o_x + x - daters.surface.left;
	y = o_y + y - daters.surface.top;

	for index, icon in pairs(daters.icons) do 
	    if (x > icon.left) then
		if (x < daters.surface.width) then
		    if (y > icon.top) then
			if (y < icon.top + icon.height) then
			    echo("returning "..index.."\n");
			    return index;
			end
		    end
		end
	    end
	end
	return 0;
    end
    
    daters.surface.on_redraw = function () 
	start = timer();
	o_x = 0;
	o_y = daters.surface.scroll_position;
		
	--echo(daters.surface.dirty.."\n");	
	daters.surface.clear();
	s_h = daters.surface.height;
	--if (type(icon.surface) == "number") then
	if (icon.surface > 0) then
	    for index, icon in pairs(daters.icons) do 
		if (icon.top + o_y + daters.surface.icon_height > 0) then
		    if (index == daters.surface.selected) then
			daters.surface.forecolor = daters.surface.selectedcolor;
			daters.surface.rect_fill(0, o_y + icon.top, o_x + icon.left + x + daters.surface.width, o_y + icon.top + 32); 
			daters.surface.forecolor = daters.surface.unselectedcolor;
		    end
		
		    if (daters.surface.font) then
			caption_width = daters.surface.text_length(icon.caption);
			x = 32; 
			daters.surface.print(o_x + icon.left + x, o_y + icon.top + 16, icon.caption);
		    end
		    surface_transparent_blit(icon.surface, daters.surface.surface, o_x + icon.left, o_y + icon.top);
		end
	    end
	else
	    for index, icon in pairs(daters.icons) do 
		if (icon.top + o_y + daters.surface.icon_height > 0) then
		    if (index == daters.surface.selected) then
			daters.surface.forecolor = daters.surface.selectedcolor;
			daters.surface.rect_fill(0, o_y + icon.top, o_x + icon.left + x + daters.surface.width, o_y + icon.top + daters.surface.text_height()); 
			daters.surface.forecolor = daters.surface.unselectedcolor;
		    end
		
		    if (daters.surface.font) then		    
	    		caption_width = daters.surface.text_length(icon.caption);			
			x = 0; 
			daters.surface.print(o_x + icon.left + x, o_y + icon.top, icon.caption);
		    end
		    --surface_transparent_blit(icon.surface, daters.surface.surface, o_x + icon.left + ((daters.surface.icon_width / 2) - (16)), o_y + icon.top);
		end
	    end	    
	end
    end
    
    daters.on_icon_activate = function (index)
	if (type(daters.surface.on_icon_activate) == "function") then
	    daters.surface.on_icon_activate(index, daters.icons[index]);
	end	
	if (type(daters.icons[index].callback) == "function") then
	    daters.icons[index].callback(daters.icons[index].data);
	end
    end
    
    daters.on_icon_select = function(index)
	daters.surface.selected = index;
	if (type(daters.surface.on_icon_select) == "function") then
	    daters.surface.on_icon_select(index, daters.icons[index]);
	end		
	
	daters.surface.on_redraw();
	daters.surface.set_dirty();
    end
    
    daters.surface.on_mouse_up = function (button, x, y)
	index = daters.get_icon_from_xy(x, y);
	if (index == daters.last_index) then
	    if (timer() - daters.last_click < .25) then
		if (index > 0) then
		    daters.on_icon_activate(index);
		end
	    end
	end
	daters.on_icon_select(index);
		
	daters.last_index = index;
	daters.last_click = timer();
	daters.dragging = 0;
    end

    daters.surface.on_mouse_down = function (button, x, y)
	--[[if (daters.dragging == 0) then
	    index = daters.get_icon_from_xy(x, y);
	    --daters.icons[index].dragging = 1;
	    daters.dragging = index;
	    echo("dragging "..index.."\n");
	else
	    daters.icons[daters.dragging].left = x;
	    daters.icons[daters.dragging].top = y;
	    daters.surface.set_dirty();
	end]]
	
    end
    
        
    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "width" then
		return daters.surface.width;
	    elseif key == "icons" then
		return daters.icons;
	    else 
		return daters.surface[key];
	    end
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    if key == "width" then
		daters.surface.width = value;
		vscroll.left = daters.surface.width - vscroll.width;
		daters.surface.clear();
	    elseif key == "height" then
		daters.surface.height = value;
		vscroll.height = value;
		daters.surface.clear();
	    elseif key == "parent" then
		daters.surface.parent = value;
		vscroll.parent = value;
		vscroll.left = daters.surface.left + daters.surface.width;
		vscroll.top = daters.surface.top + vscroll.top;
		daters.surface.clear();

	    else
		daters.surface[key] = value;
	    end
	    
	end	
    }
    );
--[[
    daters.surface.on_keypress = function (keycode, keyval)
	--echo("key pressed!".. keyval .."\n");
	set_surface_dirty(daters.surface.surface);	
	if (keyval == 13) then
	    if (type(daters.surface.on_click) == 'function') then
		daters.surface.on_click();
	    end
	elseif (keyval == 9) then
	    if (type(daters.surface.on_tab) == 'function') then
		daters.surface.on_tab();
	    end
	elseif (keyval == 27) then
	    if (type(daters.surface.on_escape) == 'function') then
		daters.surface.on_escape();
	    end	    
	end
    end
]]

    daters.surface.arrange_icons = function()
	o_x = 6;
	o_y = 6;
	x = 0;
	y = 0;
	for index, icon in pairs(daters.icons) do 
	    icon.top = o_y + y;
	    icon.left = o_x + x;
	    if (icon.image) then
		icon.height = daters.surface.icon_height;
		y = y + icon.height;
	    else 
		if (daters.surface.font) then
		    icon.height = daters.surface.text_height();
		    y = y + icon.height;
		end
	    end
	    daters.surface.scroll_max = y;
	end
	vscroll.max = daters.surface.scroll_max;
	if (vscroll.max > daters.surface.height) then
	    vscroll.visible = 1;
	else
	    vscroll.visible = 0;
	end
	daters.surface.set_dirty();
	vscroll.left = daters.surface.left + daters.surface.width;
	vscroll.top = daters.surface.top;
	
    end
    
    daters.surface.remove_icon = function(index)
	if (daters.surface.icon_count > 0) then
	    table.remove(daters.icons, index);
	end
	daters.surface.icon_count = daters.surface.icon_count - 1;
	if (daters.surface.icon_count < 0) then
	    daters.surface.icon_count = 0;
	end
    end
    
    daters.surface.clear_icons = function()
	daters.icons = {};
	daters.surface.icon_count = 0;
    end
    
    daters.surface.add_icon = function (caption, image, callback, data)
	echo("adding "..tostring(caption));
	
	icon = {};
	icon.top = 0;
	icon.left = 0;
	if (type(image) ~= "nil") then
	    icon.surface = image.surface;
	else 
	    icon.surface = 0;
	end
	icon.caption = caption;
	icon.data = data;
	icon.callback = callback;
	
	table.insert(daters.icons, icon);
	
	if (daters.surface.arrange_auto == 1) then
	    daters.surface.arrange_icons();
	end	
	daters.surface.icon_count = daters.surface.icon_count + 1;
	echo("done\n");
    end
        
    return object;
end