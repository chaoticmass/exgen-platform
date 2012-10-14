
application = {};
function application:new()
    local object = {};
    local daters = {};	
    
    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "dingdong" then
		return 0;
	    else 
		return daters[key];
	    end
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    if key == "dingdong" then
	    else
		daters[key] = value;
	    end
	    daters[key] = value;
	end	
    }
    );
    daters.dump = function ()
	echo("dump\n");
	for key, value in pairs(daters) do 
	    echo("Key: ");
	    echo(key);
	    echo(" Value: ");
	    echo(value);
	    echo("\n");
	end
    end
    
    

    return object;
end

app = application:new();

