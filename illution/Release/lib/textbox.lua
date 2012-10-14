--[[



    ALERT ALERT ALERT ALERT ALERT ALERT ALERT ALERT
    ALERT 				      ALERT
    ALERT 	THIS CODE IS NOT USED	      ALERT
    ALERT 		RIGHT NOW             ALERT
    ALERT 				      ALERT
    ALERT ALERT ALERT ALERT ALERT ALERT ALERT ALERT

]]

textbox = {};
function textbox:new()
    local object = {};
    local daters = {};
    daters.surface = surface:new();
    daters.surface.width = 60;
    daters.surface.height = 20;
    daters.surface.backcolor = makecol(255, 255, 255);
    daters.surface.forecolor = 0;
    daters.surface.clear();
    daters.text = "";
    daters.startpos = 0;
    daters.surface.on_draw = function () 
	daters.surface.clear()
	--echo("starting at ");
	--echo(daters.startpos);
	--echo("\n");
	daters.prevline = daters.nextline;
	daters.nextline = surface_print_wordwrap(daters.surface.surface, daters.text, daters.startpos);
	--echo("next line is ");
	--echo(daters.nextline);
	--echo("\n");

	--echo("prev line is ");
	--echo(daters.prevline);
	--echo("\n");

    end
    daters.surface.on_keypress = function (keycode, keyval)
	--echo("key pressed!".. keycode .."\n");
	if keycode == 84 then
	    daters.startpos = daters.prevline;
	    if daters.startpos < 0 then
		daters.startpos = 0;
	    end
	elseif keycode == 85 then
	    daters.startpos = daters.nextline;
	    if daters.startpos > string.len(daters.text) then
		daters.startpos = string.len(daters.text);
	    end
	else
	end
	set_surface_dirty(daters.surface.surface);		
	
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
		set_surface_dirty(daters.surface.surface);		
		daters.surface.on_draw();
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
