
surface = {};
function surface:new(index)
    local object = {};
    local daters = {};

    if (type(index) == "number") then
	daters.surface = index;
    else
	daters.surface = create_surface(16, 16);    
	surface_clear(daters.surface);
    end
    
    daters.on_draw = function () end;
    daters.on_mouse_down = function () end;
    daters.on_mouse_up = function () end;
    daters.on_mouse_click = function () end;
    daters.on_keypress = function () end;
    set_surface_lua(daters.surface);
    
    daters.margin_left = 0;
    daters.margin_right = 0;
    daters.margin_top = 0;
    daters.margin_bottom = 0;
    
    daters.tab_count = 0;
    
    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "width" then
		return get_surface_width(daters.surface);
	    elseif key == "height" then
		return get_surface_height(daters.surface);
	    elseif key == "x" then
		return get_surface_x(daters.surface);
	    elseif key == "y" then
		return get_surface_y(daters.surface);
	    elseif key == "x_offset" then
		return get_surface_x_offset(daters.surface);
	    elseif key == "y_offset" then
		return get_surface_y_offset(daters.surface);

	    elseif key == "position_absolute" then
		return get_surface_position_absolute(daters.surface);
	    elseif key == "top" then
		return get_surface_y(daters.surface);
	    elseif key == "left" then
		return get_surface_x(daters.surface);
	    elseif key == "z" then
		return get_surface_z(daters.surface);
	    elseif key == "parent" then
		return daters.parent;
		--return get_surface_parent(daters.surface);
	    elseif key == "font" then
		return  daters.font;
	    elseif key == "backcolor" then
		return get_surface_backcolor(daters.surface);
	    elseif key == "forecolor" then
		return get_surface_forecolor(daters.surface);
	    elseif key == "transcolor" then
		return get_surface_transcolor(daters.surface);
	    elseif key == "alpha" then
		return get_surface_alpha(daters.surface);
	    elseif key == "visible" then
		--echo("Get visible["..daters.surface.."] = "..get_surface_visible(daters.surface).."\n");
		return get_surface_visible(daters.surface);
	    elseif key == "rotation" then
		return get_surface_rotation(daters.surface);
	    elseif key == "on_draw" then
		return daters.on_draw;
	    elseif key == "noevents" then
		return get_surface_noevents(daters.surface);		
	    elseif key == "can_have_focus" then
		return get_surface_can_have_focus(daters.surface);
	    elseif key == "tab_index" then
		return get_surface_tab_index(daters.surface);		
	    elseif key == "dirty" then
		return get_surface_dirty(daters.surface);		
	    else 
		return daters[key];
	    end
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    if key == "width" then
		set_surface_dirty(daters.surface);
		if (type(value) == "number") then
		    if value <= 1 then
			if (type(object.parent) == "table") then
			    if type(object.parent.width) == "number" then
		    		value = object.parent.width * value;
				--echo("working with percents bitch".. value .."\n");
			    end
			end
		    end
		end
		set_surface_width(daters.surface, value);
		if (daters.visible) then
		    daters.on_draw();
		end
	    elseif key == "height" then
		set_surface_dirty(daters.surface);
		if (type(value) == "number") then
		    if value <= 1 then
			if (type(object.parent) == "table") then
			    if type(object.parent.height) == "number" then
				value = object.parent.height * value;
				--echo("working with percents bitch".. value .."\n");
			    end
			end
		    end
		end
		set_surface_height(daters.surface, value);
		if (daters.visible) then
		    daters.on_draw();
		end		
	    elseif key == "x" then
	    	set_surface_dirty(daters.surface);
		set_surface_x(daters.surface, value);
	    elseif key == "y" then
	    	set_surface_dirty(daters.surface);
		set_surface_y(daters.surface, value);
	    elseif key == "x_offset" then
	    	--set_surface_dirty(daters.surface);
		set_surface_x_offset(daters.surface, value);
	    elseif key == "y_offset" then
	    	--set_surface_dirty(daters.surface);
		set_surface_y_offset(daters.surface, value);
	    elseif key == "position_absolute" then
	    	--set_surface_dirty(daters.surface);
		set_surface_position_absolute(daters.surface, value);

	    elseif key == "top" then
	    	set_surface_dirty(daters.surface);
		set_surface_y(daters.surface, value);
	    elseif key == "left" then
	    	set_surface_dirty(daters.surface);
		set_surface_x(daters.surface, value);
	    elseif key == "z" then
	    	set_surface_dirty(daters.surface);
		set_surface_z(daters.surface, value);
	    elseif key == "font" then
		set_surface_dirty(daters.surface);
		daters.font = value;
		set_surface_font(daters.surface, value.surface);
	    elseif key == "parent" then
		daters.parent = value;
		if (type(daters.parent) == "table") then
		    daters.z = daters.parent.z;
		    daters.tab_index = daters.parent.tab_count + 1;
		    daters.parent.tab_count = daters.parent.tab_count + 1;
		    set_surface_tab_index(daters.surface, daters.tab_index);
		    set_surface_parent(daters.surface, value.surface);
		end
		--echo("Tab Index: "..daters.tab_index.."\n");
--[[		echo(type(daters.parent).."\n");
		if type(daters.parent) == "nil" then
		    echo("daters.parent == nil is true\n");
		else
		    echo("daters.parent == nil is false\n");
		    echo(daters.parent.margin_top.."\n");
		    
		    if type(daters.parent.margin_top) == "nil" then
		    else
			if type(daters.top) == "nil" then
			    daters.top = 0;
			end
			echo("margin_top bitch\n");
			daters.top = daters.parent.margin_top + daters.top;
			echo(daters.top.."\n");
		    end
		end
]]
	    	set_surface_dirty(daters.surface);
--		set_surface_dirty(daters.parent.surface);
		
	    elseif key == "backcolor" then
		set_surface_backcolor(daters.surface, value);
	    elseif key == "forecolor" then
		set_surface_forecolor(daters.surface, value);
	    elseif key == "transcolor" then
		set_surface_dirty(daters.surface);
		set_surface_transcolor(daters.surface, value);
	    elseif key == "alpha" then
		set_surface_dirty(daters.surface);
		if (type(value) == "string") then
		    value = tonumber(value);
		end
		if (tonumber(value) > 255) then
		    value = 255;
		end
		if (tonumber(value) < 0) then
		    value = 0;
		end
		set_surface_alpha(daters.surface, value);
	    elseif key == "transparent" then
		set_surface_dirty(daters.surface);
		set_surface_transparent(daters.surface, value);
	    elseif key == "rotation" then
		set_surface_dirty(daters.surface);
		set_surface_rotation(daters.surface, value);
	    elseif key == "visible" then
		--echo("Set visible["..daters.surface.."] = "..value.."\n");
		set_surface_dirty(daters.surface);
		set_surface_visible(daters.surface, value);
	    elseif key == "on_draw" then
		daters.on_draw = value;
		set_surface_on_draw(daters.surface, value);
		if (daters.visible) then
		    daters.on_draw();
		end
	    elseif key == "on_got_focus" then
		daters.on_got_focus = value;
		set_surface_on_got_focus(daters.surface, value);		
	    elseif key == "on_lost_focus" then
		daters.on_lost_focus = value;
		set_surface_on_lost_focus(daters.surface, value);
	    elseif key == "on_keypress" then
		daters.on_keypress = value;
		set_surface_on_keypress(daters.surface, value);

	    elseif key == "on_mouse_down" then
		daters.on_mouse_down = value;
		set_surface_on_mouse_down(daters.surface, value);

	    elseif key == "on_mouse_up" then
		daters.on_mouse_up = value;
		set_surface_on_mouse_up(daters.surface, value);

	    elseif key == "on_mouse_click" then
		echo(daters.surface..": MOUSE CLICKA REGISTER\n");
		daters.on_mouse_click = value;
		set_surface_on_mouse_click(daters.surface, value);
	    elseif key == "always_on_top" then
		daters.always_on_top = value;
		set_surface_always_on_top(daters.surface, value);
	    elseif key == "bmp" then
		surface_load_bmp(daters.surface, value);
		daters.bmp = value;
	    elseif key == "jpg" then
		surface_load_jpg(daters.surface, value);
		daters.jpg = value;
	    elseif key == "gif" then
		set_surface_transparent(daters.surface, 1);
		surface_load_gif(daters.surface, value);
		daters.gif = value;
	    elseif key == "animated_gif" then
		set_surface_transparent(daters.surface, 1);
		surface_load_animated_gif(daters.surface, value);
		daters.animated_gif = value;	    
	    elseif key == "noevents" then
		set_surface_noevents(daters.surface, value);
	    elseif key == "can_have_focus" then
		set_surface_can_have_focus(daters.surface, value);	
	    elseif key == "tab_index" then
		if (get_surface_tab_index(daters.surface) == -1) then
		    set_surface_tab_index(daters.surface, value);
		    if (value == 0) then
		        daters.set_focus();
		    end
		else    
		    set_surface_tab_index(daters.surface, value);
		end 
		
	    elseif key == "dirty" then
		if (value == 1) then
		    set_surface_dirty(daters.surface);
		else 
		    set_surface_dirty(daters.surface, value);
		end
	    else
		daters[key] = value;
	    end
	    daters[key] = value;
	end	
    }
    );

    daters.on_keypress = function (keycode, keyval)
	if (keyval == 13) then
	    if (type(daters.on_return) == 'function') then
		daters.on_return();
	    end
	elseif (keyval == 9) then
	    if (type(daters.on_tab) == 'function') then
		daters.on_tab();
	    end
	    next_focus();
	elseif (keyval == 27) then
	    if (type(daters.on_escape) == 'function') then
		daters.on_escape();
	    end	    
	end
    end

    daters.fullscreen = function()
	object.width = screen_width();
	object.height = screen_height();
    end

    daters.has_focus = function()
	return surface_has_focus(daters.surface);
    end

    daters.text_length = function(text)
	retval = surface_get_text_length(daters.surface, text);
	return retval;
    end

    daters.text_height = function()
	return surface_get_text_height(daters.surface);
    end
	
    daters.save_bmp = function(file) 
	surface_save_bmp(daters.surface, file);
    end
    daters.save_jpg = function(file) 
	if (type(file) == "nil") then
	    return surface_save_jpg(daters.surface);
	else
	    surface_save_jpg(daters.surface, file);
	end
    end

    daters.gradient = function (color1, color2, border) 
	border = border or 1;
	gradient(daters.surface, 0, 0, get_surface_width(daters.surface), get_surface_height(daters.surface), color1, color2, border);
    end

    daters.draw_gradient = function (x1, y1, x2, y2, color1, color2, border) 
	border = border or 1;
	gradient(daters.surface, x1, y1, x2, y2, color1, color2, border);
    end
    
    daters.clear = function ()
--	set_surface_dirty(daters.surface);
	surface_clear(daters.surface);
    end
        
    daters.print = function (x, y, text)
--	set_surface_dirty(daters.surface);
	surface_print(daters.surface, x, y, text);
    end

    daters.get_pixel = function (x1, y1)
	return surface_get_pixel(daters.surface, x1, y1);
    end

    daters.thick_line = function (x1, y1, x2, y2, thickness)
	surface_thick_line(daters.surface, x1, y1, x2, y2, thickness);
    end

    daters.line = function (x1, y1, x2, y2)
	surface_line(daters.surface, x1, y1, x2, y2);
    end

    daters.rect = function (x1, y1, x2, y2)
	surface_rect(daters.surface, x1, y1, x2, y2);
    end

    daters.rect_fill = function (x1, y1, x2, y2)
	surface_rect_fill(daters.surface, x1, y1, x2, y2);
    end
    
    daters.set_focus = function ()
	set_focus(daters.surface);
    end
    
    daters.blit = function (dest, source_x, source_y, dest_x, dest_y, width, height)
	if (type(source_x) == "nil") then
	    surface_blit(daters.surface, dest.surface);
	else 
	    surface_blit(daters.surface, dest.surface, source_x, source_y, dest_x, dest_y, width, height)
	end
    end
    daters.transparent_blit = function (dest, dest_x, dest_y)
	if (type(dest_x) == "nil") then
	    surface_transparent_blit(daters.surface, dest.surface);
	else 
	    surface_transparent_blit(daters.surface, dest.surface, dest_x, dest_y)
	end
    end

    daters.set_dirty = function ()
	set_surface_dirty(daters.surface);
    end
    
    daters.destroy = function ()
	destroy_surface(daters.surface);
	set_surface_dirty(daters.parent);
	daters = nil;
	--return nil;
    end
    
    daters.dump = function ()
	echo("dump\n");
	for key, value in pairs(daters) do 
	    echo("Key: ");
	    echo(key);
	    echo(" Value: ");
	    echo(value);
	    echo("\n");
	end
    end
    
    object.dirty = 1;
    return object;
end

