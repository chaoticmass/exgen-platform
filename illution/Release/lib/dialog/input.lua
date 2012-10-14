
input_dialog = {};
function input_dialog:new()
    local object = {};
    local daters = {};
--unfinished
    window = window:new();
    daters.window = window;
    
    window.width = screen_width() / 2;
    window.height = 100;
    
    input = inputbox:new();
    daters.input = input;
    input.parent = window;
    input.top = window.align_top();
    input.width = window.width - 64;
    input.left = window.align_center();
    
    
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

