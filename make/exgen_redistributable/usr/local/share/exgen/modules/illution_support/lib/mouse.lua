
mouse_cursor = {};
function mouse_cursor:new()
    local object = {};
    local daters = {};
    daters.surface = surface:new();
    daters.surface.width = 16;
    daters.surface.height = 16;
    daters.surface.transparent = 1;
    daters.surface.backcolor = makecol(255, 0, 255);
    daters.surface.forecolor = makecol(0, 0, 0);
    daters.surface.clear();
    daters.surface.always_on_top = 1;
    daters.surface.bmp = "system/images/cursor.bmp";
    daters.surface.z = 9999;
    
    
    daters.surface.on_draw = function () 
	x, y = mouse_xy();
	mouse.top = y;
	mouse.left = x;
	mouse.set_dirty();
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
	    else
		daters.surface[key] = value;
	    end
	end	
    }
    );
    
    return object;
end

