
multi_line_inputbox = {};
function multi_line_inputbox:new()
    local object = {};
    local daters = {};
    daters.surface = surface:new();
    daters.surface.width = 60;
    daters.surface.height = 20;
    daters.surface.backcolor = 0; --makecol(255, 255, 255);
    daters.surface.forecolor = 0; --makecol(255, 255, 255);
    daters.surface.clear();
    daters.text = "";
    daters.startpos = 0;
    daters.textwidth = 0;
--    daters.vscroll = scrollbar:new();
    
    daters.surface.on_draw = function () 
	daters.surface.clear()
	if (daters.textwidth > daters.surface.width) then
	    xoffset = daters.textwidth - daters.surface.width + 3;
	else
	    xoffset = 0;
	end
	if (daters.surface.has_focus() == 1) then
	    --daters.surface.print(0 - xoffset, 0, daters.text.."|");
	    surface_print_wordwrap(0, 0, daters.surface.surface, daters.text, 0);
	else
	    --daters.surface.print(0, 0, daters.text);
	    surface_print_wordwrap(0, 0, daters.surface.surface, daters.text, 0);
	end
    end

    daters.surface.on_mouse_click = function ()
	daters.surface.set_focus();
    end
    
    daters.surface.on_keypress = function (keycode, keyval)
	--echo("key pressed!".. keyval .."\n");
	set_surface_dirty(daters.surface.surface);	
	if (keyval == 8) then
	    object.text = string.sub(object.text, 0, string.len(object.text) - 1);
	elseif (keyval == 13) then
	    object.text = object.text .. string.char(10);
	elseif (keyval == 9) then
	    if (type(daters.surface.on_return) == 'function') then
		daters.surface.on_return();
	    end
	elseif (keyval == 27) then
	    if (type(daters.surface.on_escape) == 'function') then
		daters.surface.on_escape();
	    end	    
	elseif ((keyval > 31) and (keyval < 127)) then
	    object.text = object.text .. string.char(keyval);
	end
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
		daters.text = value;
		if (value ~= "") then
		    daters.textwidth = daters.surface.text_length(value);
		else
		    daters.textwidth = 0;
		end
		--set_surface_dirty(daters.surface.surface);		
		--daters.surface.on_draw();
	    elseif key == "nextline" then
		daters.nextline = value;
	    elseif key == "startpos" then
		--echo("new start\n");
		daters.startpos = value;
		set_surface_dirty(daters.surface.surface);		
		daters.surface.on_draw();		
	    elseif key == "width" then
		daters.surface.width = value;
	    else
		daters.surface[key] = value;
	    end
	end	
    }
    );

        
    return object;
end
