
hidden = {};
function hidden:new()
    local object = {};
    local daters = {};
    
    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "width" then
		return daters.width;
	    else 
		return daters[key];
	    end
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    if key == "width" then
		daters[key] = value;
	    else
		daters[key] = value;
	    end
	end	
    }
    );

        
    return object;
end

