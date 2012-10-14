inputbox = {};
function inputbox:new()
    local object = {};
    local daters = {};
    daters.surface = surface:new();
    daters.surface.width = 80;
    daters.surface.height = 20;
    daters.surface.backcolor = 0; --makecol(255, 255, 255);
    daters.surface.forecolor = 0; --makecol(255, 255, 255);
    daters.surface.clear();
    daters.text = "";
    daters.value = daters.text;
    daters.startpos = 0;
    daters.textwidth = 0;
    daters.label = "";
    daters.surface.multi_line = 0;
    

    
    daters.surface.on_draw = function () 	
	daters.surface.clear()
	daters.surface.gradient(makecol(255, 255, 255), makecol(200, 200, 200));
	if (daters.textwidth > daters.surface.width) then
	    xoffset = daters.textwidth - daters.surface.width + 6;
	    xoffset = xoffset - 5;
	    yoffset = 0 + 6;
	else
	    xoffset = 0 - 5;
	    yoffset = 0 + 6;
	end
	x = 3;
	y = 3;
	if (daters.surface.multi_line == 1) then
	    if (daters.surface.password) then
	        surface_print_wordwrap(x, y, daters.surface.surface, string.rep(daters.surface.password, string.len(daters.text)), 0);	    
	    else
	        surface_print_wordwrap(x, y, daters.surface.surface, daters.text, 0);	    
	    end	    
	else
	    if (daters.surface.password) then
	        daters.surface.print(0 - xoffset, yoffset - 0, string.rep(daters.surface.password, string.len(daters.text)));
	        --surface_print_wordwrap(x, y, daters.surface.surface, string.rep(daters.surface.password, string.len(daters.text)), 0);	    
	    else
	        daters.surface.print(0 - xoffset, yoffset - 0, daters.text);
	        --surface_print_wordwrap(x, y, daters.surface.surface, daters.text, 0);	    
	    end	    
	end

	if (daters.surface.has_focus() == 1) then
	    daters.surface.rect(1, 1, daters.surface.width - 2, daters.surface.height - 2);
	end

    end
    
    daters.surface.on_got_focus = function ()
	daters.surface.set_dirty();
	daters.surface.on_draw();    
    end
    daters.surface.on_lost_focus = function ()
	daters.surface.clear();
	daters.surface.on_draw();
	if (type(daters.surface.parent) == "table") then
	    daters.surface.parent.set_dirty();
	end
	
    end

    daters.surface.on_mouse_click = function (button, x, y)
	daters.surface.set_focus();
	daters.surface.set_dirty();
	
    end
    
    daters.surface.on_keypress = function (keycode, keyval)
	--echo("key pressed!".. daters.surface.multi_line .."\n");
	
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
		--echo("setting text "..value.." oldvalue "..daters.text.."\n");
		daters.text = value;
		
		if (value ~= "") then
		    daters.textwidth = daters.surface.text_length(value);
		else
		    daters.textwidth = 0;
		end
		set_surface_dirty(daters.surface.surface);		
		daters.surface.on_draw();
	        if (type(daters.surface.on_change) == 'function') then
		    daters.surface.on_change();
		end	    
	    elseif key == "value" then
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

    object.can_have_focus = 1;        
    return object;
end

textbox = inputbox;
password = inputbox;