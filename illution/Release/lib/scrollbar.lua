scrollbar = {};
function scrollbar:new()
    local object = {};
    local daters = {};
    daters.surface = surface:new();
    daters.surface.width = 32;
    local scroll = daters.surface;
    scroll.vertical = 1;
    scroll.visible = 0;
    scroll.size = 10;
    scroll.max = 100;
    scroll.last_value = 0;
    scroll.can_have_focus = 1;
    scroll.position = 0;
    scroll.width = 16;
    
    scroll.on_draw = function () 
	if scroll.visible ~= 0 then
	    col1 = makecol(190, 190, 190);
	    col2 = makecol(100, 100, 100);
	    
	    scroll.size = (scroll.height / scroll.max) * scroll.height;
	    scroll.clear()
	    --scroll.gradient(col2, col2);
	    --scroll.position = ((scroll.height - scroll.size) / scroll.max) * scroll.value;
	    y1 = scroll.position;
	    y2 = scroll.position + scroll.size;

	    --scroll.forecolor = makecol(128, 0, 0);
	    --scroll.backcolor = makecol(128, 0, 0);
	    --scroll.rect(0, y1, scroll.width, y2);
	    scroll.forecolor = col1;
	    scroll.rect_fill(0, 0, scroll.width, scroll.height);
	    scroll.forecolor = col2;
	    scroll.rect(0, 0, scroll.width - 1, scroll.height - 1);
	    --gradient(scroll.surface, 0, y1, scroll.width, y2, col1, col2);
	    scroll.rect_fill(0, y1, scroll.width, y2);

	    scroll.hitbox_x1 = 0;
	    scroll.hitbox_y1 = y1;	    
	    scroll.hitbox_x2 = scroll.width;
	    scroll.hitbox_y2 = y2;
	    if scroll.value ~= scroll.last_value then
		if type(scroll.on_change) == 'function' then
		    scroll.on_change();
		end
		--echo("S: "..scroll.value.." L: "..scroll.last_value.."\n");
		scroll.last_value = scroll.value;
	    end
	    
	end
    end

    scroll.on_mouse_up = function(button, x, y)
	scroll.dragging = 0;
    end
    
    scroll.on_mouse_down = function (button, x, y)
	x = x - scroll.left;
	y = y - scroll.top;
--[[	if (x > scroll.hitbox_x1) then
	    if (x < scroll.hitbox_x2) then
		if (y > scroll.hitbox_y1) then
		    if (y < scroll.hitbox_y2) then
			echo("drag\n\n");
			if (scroll.dragging ~= 1) then
			    scroll.dragging = 1;
			end
		    end
		end
	    end
	end]]
	
	-- calculate position of scroll button
	if scroll.vertical == 1 then
	    scroll.position = y - (scroll.size/2);
	else
	    
	end
	-- keep it in bounds
	if scroll.position > scroll.height - scroll.size then
	    scroll.position = scroll.height - scroll.size;
	end
	if scroll.position < 0 then
	    scroll.position = 0;
	end

	-- calculate value of scroll
	if scroll.vertical == 1 then
	    --scroll.value = (scroll.max / (scroll.height - scroll.size)) * (scroll.position);
	    scroll.value = ((scroll.max / scroll.height) * scroll.position);
	    --scroll.value = scroll.position;
	
	    --echo(scroll.max.." / "..(scroll.height - scroll.size).." * "..scroll.position.." = "..scroll.value.."\n");
	else
	    
	end
	-- keep it in bounds
	if scroll.value > scroll.max then
	    scroll.value = scroll.max;
	end 
	if scroll.value < scroll.min then
	    scroll.value = scroll.min;
	end
	if scroll.has_focus ~= true then
	    --echo("setting focus\n");
	    --scroll.set_focus();
	end
	--scroll.on_draw();
	scroll.set_dirty();
    end
    scroll.on_keypress = function (keycode, keyval)
	--echo(keycode.."\n");
	set_surface_dirty(scroll.surface);	
	if (keycode == 8) then
	elseif (keycode == 84) then
	    -- up
	    scroll.value = scroll.value - scroll.size;
	    scroll.set_dirty();
	elseif (keycode == 85) then
	    -- down
	    scroll.value = scroll.value + scroll.size;
	    scroll.set_dirty();
	elseif (keycode == 80) then
	    -- page-up
	    scroll.value = scroll.value - scroll.size;
	    scroll.set_dirty();
	elseif (keycode == 81) then
	    -- page-down
	    scroll.value = scroll.value + scroll.size;
	    scroll.set_dirty();

	elseif (keycode == 78) then
	    -- home
	    scroll.value = scroll.min;
	    scroll.set_dirty();
	elseif (keycode == 79) then
	    -- end
	    scroll.value = scroll.max;
	    scroll.set_dirty();

	elseif (keycode == 83) then
	    -- left
	    scroll.value = scroll.value - scroll.size;
	    scroll.set_dirty();
	elseif (keycode == 58) then
	    -- right
	    scroll.value = scroll.value + scroll.size;
	    scroll.set_dirty();
	end
	if scroll.value > scroll.max then
	    scroll.value = scroll.max;
	end 
	if scroll.value < scroll.min then
	    scroll.value = scroll.min;
	end
	
    end
    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "width" then
		return scroll.width;
	    else 
		return scroll[key];
	    end
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    --echo("setting "..key.."\n");
	    if key == "value" then
		scroll.value = value;
		scroll.on_draw();
	    elseif key == "parent" then
		scroll.parent = value;
		--scroll.width = 16;
		scroll.height = 20;
		--scroll.backcolor = getvar("colors", "widget-backcolor"); --makecol(255, 255, 255);
		--scroll.forecolor = getvar("colors", "widget-forecolor") --makecol(255, 255, 255);
		
		scroll.min = 0;
		scroll.max = 100;
		scroll.size = 10;
		scroll.value = 1;
		scroll.last_value = 1;
		
		if (scroll.vertical) then
		    scroll.top = scroll.parent.margin_top;
		    scroll.left = scroll.parent.width - scroll.width - scroll.parent.margin_right;
		    scroll.height = scroll.parent.height - scroll.top - scroll.parent.margin_bottom;
		end

		scroll.on_draw();
		scroll.z = 9999;
		--scroll.visible = 1;
		
	    else
		scroll[key] = value;
	    end
	end	
    }
    );

    return object;
end

