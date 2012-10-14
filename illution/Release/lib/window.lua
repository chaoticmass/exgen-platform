window = {};
function window:new()
    local object = {};
    local daters = {};
    
    daters.surface = surface:new();
    daters.surface.visible = 0;
    daters.surface.width = 320;
    daters.surface.height = 240;
    daters.surface.titlebar_height = 24;
    daters.surface.no_titlebar = 0;
    daters.surface.resizable = 1;
    daters.surface.rotate = 0;
    daters.surface.active = 0;
    

    daters.close = button:new();
    daters.close.position_absolute = 1;
    daters.close.parent = daters.surface;
    daters.close.width = daters.surface.titlebar_height;
    daters.close.height = daters.surface.titlebar_height - 3;
    daters.close.top = 3;
    daters.close.left = daters.surface.width - daters.close.width - 3;
    daters.close.visible = 0;

    daters.surface.z = 99999;
    daters.surface.caption = "Window";    
    daters.surface.alpha = 0;
    daters.fade_in = 1;
    daters.fade_out = 1;
    daters.tick = timer();
    daters.surface.margin_left = 3;
    daters.surface.margin_right = 3;
    daters.surface.margin_top = 32;
    daters.surface.margin_bottom = 3;
    daters.surface.bare = 0;
    daters.surface.grow = 0;
    daters.surface.draggable = 1;
    daters.surface.z = 9999;
    daters.surface.current_top = 0;
    daters.surface.current_left = 0;
    
    daters.children = {};
    daters.child_count = 0;
    
    daters.vscroll = scrollbar:new();    
    daters.vscroll.visible = 0;

    daters.surface.add_child = function(child)
	daters.children[daters.child_count] = child;
    end
    
    setvar("windows", "active", daters.surface.surface);
    
    daters.surface.next_child = function(child)
	found = 0;
	for key, object in pairs(daters.children) do
	    if found ~= 0 then
		return object;
	    end 
	    if object == child then
		--echo("found child\n");
		found = key;
	    end
	end
    end
    
    daters.surface.align_center = function(child)
	retval = (daters.surface.width * 0.5) - (child.width * 0.5);
	return retval;
    end

    daters.surface.align_top = function(child)
	retval = daters.surface.margin_top + daters.surface.current_top;
	daters.surface.current_top = daters.surface.current_top + child.height + 5;
	daters.vscroll.max = daters.surface.current_top - object.inside_height;
	if (daters.vscroll.max > daters.surface.height) then
	    daters.vscroll.visible = 1;
	else 
	    daters.vscroll.visible = 0;
	end
	--echo("align top = "..retval);
	--echo("\n");
	return retval;
    end

    daters.surface.align_left = function(child)
	retval = daters.surface.margin_left + daters.surface.current_left;
	daters.surface.current_left = daters.surface.current_left + child.width;
	return retval;
    end

    daters.surface.align_clear_left = function()
	daters.surface.current_left = 0;
	daters.vscroll.max = daters.surface.current_top;
    end

    daters.surface.align_clear_top = function()
	daters.surface.current_top = 0;
	daters.vscroll.max = daters.surface.current_top;
    end

    daters.surface.align_clear = function()
	daters.surface.current_top = 0;
	daters.surface.current_left = 0;
	daters.vscroll.max = daters.surface.current_top;
    end

    daters.surface.on_lost_focus = function()
	shutdown();
    end
    
    daters.surface.on_draw = function () 
	if (type(daters.surface.before_draw) == 'function') then
	    daters.surface.before_draw();
	end	

	if (daters.surface.rotate ~= 0) then
	    daters.surface.rotation = daters.surface.rotation + 1;
	end
	if (daters.surface.visible == 0) then
--	    echo("not drawing, invisible\n");
	    return 0;
	end
	
	if (type(daters.vscroll) == "table") then
	    if (daters.surface.current_top > daters.surface.height - (daters.surface.margin_top + daters.surface.margin_bottom)) then
		--daters.vscroll.visible =1;
	    end
	end
	if (daters.surface.bare ~= 1) then
	    daters.surface.gradient(makecol(190, 190, 190), makecol(128, 128, 128)); 
	else 
	    daters.surface.clear();
	end
--	daters.surface.gradient(makecol(190, 190, 190), makecol(128, 128, 128)); 
	if (daters.surface.no_titlebar == 0) then 
	    if (daters.surface.font) then
		daters.surface.titlebar_height = daters.surface.text_height() + 4;
		daters.surface.margin_top = daters.surface.titlebar_height + 8;
		daters.close.height = daters.surface.titlebar_height - 3;
	    end
	    --echo("Active Window: "..getvar("windows", "active")..", This Window: "..daters.surface.surface.."\n");
	    --if (tonumber(getvar("windows", "active")) == daters.surface.surface) then
		daters.surface.draw_gradient(3, 3, daters.surface.width - 3, daters.surface.titlebar_height, makecol(190, 190, 250), makecol(128, 128, 250)); 	
	    --else
	--	daters.surface.draw_gradient(3, 3, daters.surface.width - 3, daters.surface.titlebar_height, makecol(190, 190, 190), makecol(128, 128, 128));
	--    end
	end
	    if (daters.surface.font) then
		y = 3 + (daters.surface.titlebar_height / 2) - (daters.surface.text_height() / 2)
		daters.surface.print(6, y, daters.surface.caption);
	    end

--	if (daters.fade_in == 1) then
	    --[[if (daters.surface.alpha <= 0) then
		daters.fade_in = 0;
	    end	    
	    daters.surface.alpha = daters.surface.alpha - (getvar("windows", "fade-out-speed") * (timer() - daters.tick));
	    daters.surface.set_dirty();
	    daters.tick = timer();]]
--[[		echo(daters.surface.surface.."visible? "..daters.surface.visible.."\n");
		echo("fading in\n")
		daters.tick = timer();	    
	    	--while (daters.surface.alpha > 0) do
	    	    echo("	fading "..daters.surface.alpha.."\n");
		    daters.surface.alpha = daters.surface.alpha - (getvar("windows", "fade-out-speed") * (timer() - daters.tick));
		    daters.surface.set_dirty();
		    daters.tick = timer();	    
		    --render(daters.surface.surface);
		    --vsync();
		    if (daters.surface.alpha <= 0) then
			daters.fade_in = 0;
		    end
		--end
		echo("done fading in\n")
	end]]
--[[	if (daters.fade_out == 1) then	    
	    daters.surface.alpha = daters.surface.alpha + (256 * (timer() - daters.tick));
	    daters.surface.set_dirty();
	    daters.tick = timer();
	    if (daters.surface.alpha >= 255) then
		daters.fade_out = 0;
		if (daters.destroy_atfer_fade_out == 1) then
		    --daters.surface.destroy();
		end
	    end	    
	end]]
	if (type(daters.surface.on_redraw) == 'function') then
	    daters.surface.on_redraw();
	end
	if (type(daters.surface.after_draw) == 'function') then
	    daters.surface.after_draw();
	end	

    end

    daters.surface.on_keypress = function(keycode, keyscan) 
	--echo(keycode..' '..keyscan.."\n");
    end
    
    daters.surface.on_mouse_down = function(button, x, y) 
	if (daters.surface.draggable == 1) then
	    mouse_x, mouse_y = mouse_xy();
	    if (daters.dragging ~= 1) then
		daters.drag_x = x - daters.surface.left;
		daters.drag_y = y - daters.surface.top;
		daters.surface.alpha = getvar("windows", "lost-focus-alpha");
		daters.dragging = 1;
		daters.drag_mode = 0;
		if (daters.drag_x > daters.surface.width - 12) then
		    if (daters.drag_x <= daters.surface.width) then
			daters.drag_mode = 1
		    end
		end
		if (daters.drag_y > daters.surface.height - 12) then
		    if (daters.drag_y <= daters.surface.height) then
			daters.drag_mode = 2
		    end
		end

	    end
	    if (daters.drag_mode == 0) then
		daters.surface.left = x - daters.drag_x;
		daters.surface.top = y - daters.drag_y;
	    elseif (daters.drag_mode == 1) then
		surface_rect(0, daters.surface.x, daters.surface.y, x, daters.surface.y + daters.surface.height);
		daters.surface.new_width = tostring(x - daters.surface.left);
	    elseif (daters.drag_mode == 2) then
		surface_rect(0, object.x, object.y, object.x + object.width, y);
		daters.surface.new_height = tostring(y - object.top);
	    end
	    daters.surface.z = 9999;	
	    if (type(daters.surface.during_drag) == "function") then
		daters.surface.during_drag();
	    end
	end
    end
    
    daters.surface.on_mouse_up = function(button, x, y)
	setvar("windows", "active", daters.surface.surface);
	--echo("Active Window: "..getvar("windows", "active").."\n");
	if (button == 1) then
	    if (daters.surface.rotate ~= 0) then
		daters.surface.rotate = 0;
	    else
		--daters.surface.rotate = 1;
	    end
	end

	if (tonumber(daters.drag_mode) == 1) then
	    object.width = daters.surface.new_width;
	elseif (tonumber(daters.drag_mode) == 2) then
	    --echo("New Height: ".. tostring(object.new_height));
	    object.height = daters.surface.new_height;
	end

	daters.dragging = 0;
	daters.surface.alpha = getvar("windows", "got-focus-alpha");
	if (type(daters.surface.after_drag) == "function") then
	    daters.surface.after_drag();
	end
	if (type(daters.surface.after_mouse_up) == "function") then
	    daters.surface.after_mouse_up(button, x, y);
	end

    end
    
    daters.close.on_click = function(button, x, y) 
--[[	if (daters.close.visible == 1) then
	    --echo("Close button pressed\n");
	    daters.tick = timer();
	    daters.surface.set_dirty();
	    --echo(daters.fade_out.."\n");
	if (daters.fade_out) then
		while (daters.surface.alpha < 255) do
		    daters.surface.alpha = daters.surface.alpha + (getvar("windows", "fade-out-speed") * (timer() - daters.tick));
		    daters.surface.set_dirty();
		    daters.tick = timer();	    
		    --render(daters.surface.surface);
		    echo("why does this work?\n");
		    vsync();
		end
	    end
	    --daters.surface.destroy();
	    daters.surface.visible = 0;
	end]]
	if (type(daters.surface.on_close) == "function") then
	    daters.surface.on_close();
	end
    end 
    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "width" then
		return daters.surface.width;
	    elseif key == "inside_width" then
		return daters.surface.width - (daters.surface.margin_left + daters.surface.margin_right);
	    elseif key == "inside_height" then
		return daters.surface.height - (daters.surface.margin_top + daters.surface.margin_bottom);
	    elseif key == "close" then
		return daters.close;
	    elseif key == "vscroll" then
		return daters.vscroll;
	    elseif key == "self" then
		return daters;
	    else 
		return daters.surface[key];
	    end
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    if key == "width" then		
		daters.vscroll.left = value - daters.vscroll.width - daters.surface.margin_right; 
		daters.surface.width = value;
		daters.vscroll.top = daters.vscroll.parent.margin_top;
		daters.vscroll.left = daters.vscroll.parent.width - daters.vscroll.width - daters.vscroll.parent.margin_right;
		daters.vscroll.height = daters.vscroll.parent.height - daters.vscroll.top - daters.vscroll.parent.margin_bottom - 3;

		daters.close.left = daters.surface.width - daters.close.width - 3;
		if (type(daters.surface.on_resize) == "function") then
		    daters.surface.on_resize();
		end		
	    elseif key == "height" then
		daters.surface.height = value;
		--if (daters.vscroll.height ~=  daters.vscroll.parent.height) then
		daters.vscroll.height = value - daters.surface.margin_top;
		daters.vscroll.height = daters.vscroll.height - daters.surface.margin_bottom;
		--daters.vscroll.height = value - 32;		
		--echo("setting vscroll height to :" 
		--end
		if (type(daters.surface.on_resize) == "function") then
		    daters.surface.on_resize();
		end
	    elseif key == "caption" then
		daters.surface.caption = value;
		daters.surface.on_draw();

	    else
		daters.surface[key] = value;
	    end
	    daters.surface[key] = value;
	end	
    }
    );

    daters.vscroll.position_absolute = 1;
    daters.vscroll.parent = daters.surface;
    daters.vscroll.size = (daters.surface.height - daters.surface.margin_top - daters.surface.margin_bottom) / 10;
    daters.vscroll.visible = 0;
    daters.vscroll.on_change = function ()
	daters.vscroll.parent.y_offset = 0 - daters.vscroll.value;
	--daters.surface.set_dirty();
	--echo("vscroll on change\n");
    end

    daters.vscroll.on_mouse_up = function ()
	--daters.surface.set_dirty();
    end

    return object;
end
