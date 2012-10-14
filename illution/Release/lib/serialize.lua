
function basicSerialize (o)
    if type(o) == "number" then
	return tostring(o)
    elseif type(o) == "string" then   -- assume it is a string
	return string.format("%q", o)
    else
	echo ("it's a something else\n");
	return "woa";
    end
end

function save (output, name, value, saved, step)
    step = step or 0;
    saved = saved or {}       -- initial value
    --io.write(string.rep("  ", step), name, " = ")
    output = output .. string.rep("  ", step) .. name .. " = ";
    if type(value) == "number" or type(value) == "string" then
	--io.write(string.rep("  ", step).. basicSerialize(value), "\n")
	output = output .. string.rep("  ", step) .. basicSerialize(value).. "\n";
    elseif type(value) == "table" then
	if saved[value] then    -- value already saved?
    	    --io.write(saved[value], "\n")  -- use its previous name
    	    output = output .. saved[value].. "\n";
	else
    	    saved[value] = name   -- save name for next time
	    --io.write("{}\n")     -- create a new table
	    output = output .. "{}\n";
	    if (type(value["self"]) == "table") then
		for k,v in pairs(value["self"]) do      -- save its fields
        	    local fieldname = string.format("%s[%s]", name,
                                            basicSerialize(k))
		    step = step + 1;
		    output = save(output, fieldname, v, saved, step)
		    step = step - 1;
		end
	    end
	    for k,v in pairs(value) do      -- save its fields
        	local fieldname = string.format("%s[%s]", name,
                                            basicSerialize(k))
		step = step + 1;
		output = save(output, fieldname, v, saved, step)
		step = step - 1;
	    end
    	end
    else
	--error("cannot save a " .. type(value))
	--io.write("function\n");
	--io.write("{}\n");
	output = output .. "{}\n";
    end
    return output;
end

function serialize(value)
    retval = ""
    retval = save(retval, "value", value);
    return retval;
end