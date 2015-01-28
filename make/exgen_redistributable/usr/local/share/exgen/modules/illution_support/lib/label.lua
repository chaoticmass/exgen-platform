
label = {};
function label:new()
    local object = {};
    local daters = {};
    daters.surface = surface:new();
    daters.caption = "Label";
    daters.surface.width = 60;
    daters.surface.height = 20;
    daters.surface.forecolor = getvar("colors", "widget-forecolor");
    daters.surface.backcolor = getvar("colors", "widget-backcolor");    
    
    daters.surface.on_draw = function () 
	daters.surface.clear();
	if (daters.surface.font) then
	    daters.surface.print(6, daters.surface.height - 3 - daters.surface.text_height(), daters.caption);
	end
    end
    
    daters.surface.on_mouse_down = function (button, x, y) 
	daters.down = 1;
	daters.surface.set_dirty();
	--daters.surface.on_draw();
    end

    daters.surface.on_mouse_up = function (button, x, y) 
	daters.down = 0;	
	daters.surface.set_dirty();
	if (daters.surface.on_click) then
	    daters.surface.on_click();
	end
    end
    
    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "width" then
		return daters.surface.width;
	    else 
		return daters.surface[key];
	    end
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    if key == "width" then
		daters.surface.width = value;
	    elseif key == "caption" then
		daters.caption = value;
		if (daters.surface.font) then
		    daters.surface.height = daters.surface.text_height() + 6;
		    daters.surface.width = daters.surface.text_length(value) + 12;
		end
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
	    next_focus();
	elseif (keyval == 27) then
	    if (type(daters.surface.on_escape) == 'function') then
		daters.surface.on_escape();
	    end	    
	end
    end
        
    return object;
end

