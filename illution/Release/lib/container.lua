container = {};
function container:new()
    local the_object = {};
    local daters = {};
    div = daters;
    div.top = 0;
    div.left = 0;
    div.width = 0;
    div.height = 0;
    div.last_top = 0;
    div.last_left = 0;
    div.name = "";
    div.children = {};
    div.child_count = 0;

    
    setmetatable(the_object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "width" then
		return daters.surface.width;
	    else 
		return div[key];
	    end
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    if key == "width" then
		div.width = value;
	    else
		div[key] = value;
	    end
	end	
    }
    );
    
    div.add_child = function(child)
	echo("\n\nADD CHILD\n");
	echo("child.name = ".. child.name .."\n");
	echo("my.name = ".. div.name .."\n");
        div.children[div.child_count] = child;
	div.child_count = div.child_count + 1;	
    end
    
    div.arrange_children = function()
	echo("arranging ".. div.name .."\n");
	o_y = 0;
	o_x = 0;
	div = daters.surface;
	div.last_z = div.z;
	div.last_top = div.margin_top;
	div.last_left = div.margin_left;
	div.last_bottom = div.height - div.margin_bottom;
	div.last_right = div.width - div.margin_right;
	for key, object in pairs(div.children) do
	    echo("child: "..key .."\n");
	    echo("my width: ".. div.width .."\n");
	    echo("\n");

	    if object.height == 0 then
		if object.height_percent ~= 0 then
		    object.height = object.height_percent * (div.height - div.margin_top - div.margin_bottom);
		    div.last_top = 0;
		end
	    end
	    if object.width == 0 then
		if object.width_percent ~= 0 then
		    object.width = object.width_percent * (div.width - div.margin_left - div.margin_right);
		    div.last_left = 0;
		end
	    end

	    if object.top == 0 then
		object.top = o_y + div.last_top;
		div.last_top = div.last_top + object.height;
	    end
	    if object.left == 0 then
		object.left = o_x + div.last_left;
		div.last_left = div.last_left + object.width;
	    end
	    if type(object.set_dirty) == 'function' then
		---object.z = 9999;		
		object.set_dirty();
		echo("object drawing ".. object.name .."\n");
		
		--object.on_draw();

	    end
	    object.z = div.last_z;
	    div.last_z = div.last_z + 1;
	    print(object.top, object.left, object.width, object.height, object.z,  "\n");
	end
	echo("done arranging ".. div.name .."\n");
    end
    
    div.on_resize = function()

	if type(div.parent.arrange_children) == 'function' then
	    div.parent.arrange_children();
	else
	    if div.width == 0 then
		if div.height == 0 then
		    div.width = div.parent.width - div.parent.margin_left - div.parent.margin_right;	
		    div.height = div.parent.height - div.parent.margin_top - div.parent.margin_bottom;
		end
	    end
	    if div.left == 0 then
		if div.top == 0 then
		    div.left = div.parent.margin_left;
		    div.top = div.parent.margin_top;
		end
	    end
	end
    end
        
    return the_object;
end

