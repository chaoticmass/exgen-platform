
image = {};
function image:new()
    local object = {};
    local daters = {};
    daters.surface = surface:new();

    daters.surface.on_mouse_up = function (button, x, y) 
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
	    elseif key == "img" then
	    else
		daters.surface[key] = value;
	    end
	end	
    }
    );

    daters.surface.load_image = function()
	echo("LOAD IMAGE\n\n\n");
	if (daters.surface.hostname) then
	    if (tostring(daters.surface.image_name) == '') then
		daters.surface.image_file = "system/tmp/".. tick() ..".gif";
	    else
		daters.surface.image_file = "system/tmp/".. daters.surface.image_name;
	    end
	    print(daters.surface.hostname, daters.surface.src, daters.surface.image_file, "\n\n");
	    download_file(daters.surface.hostname, daters.surface.src, daters.surface.options, daters.surface.image_file);
	    daters.surface.gif = daters.surface.image_file;
	else
	    
	end	
    end
        
    return object;
end

