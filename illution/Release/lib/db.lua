--[[

--data = download_string("65.66.245.90", "/db/db_interface.php", "output=lua&select=*&max_results=10&dataset=inventory&get_title=");
data = download_string("65.66.245.90", "/db/db_interface.php", "output=lua&query=select 1242078774 from meta->forms get all");

include_string(data);


--echo("Record Count: "..results.record_count.."\n");
--echo(results.records[1].form_string);
--echo("\n");

form = results.records[1].form_string;
data = download_string("65.66.245.90", "/db/form_generator.php", "output=lua&query="..form);
echo(data);
echo("\n");
terminal = {};]]

client_interface = {};
function client_interface:new()
    local object = {};
    local daters = {};

    db = daters;
    db.hostname = '127.0.0.1';
    db.db_url = '/db/db_interface.php';
    db.db_query = '';
    db.post_fields = '';

    db.form_url = '/db/form_generator.php';
    db.form_query = '';
    
    
    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    return daters[key];
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    daters[key] = value;
	end	
    }
    );

    db.db_post = function()
	data = download_string(db.hostname, db.db_url, db.post_fields .."output=lua&query="..db.db_query);
--	include_string(data);	
	return data;
    end

    db.form_post = function()
	data = download_string(db.hostname, db.form_url, "output=lua&query="..db.form_query);
	return data;
    end

    return object;
end


function tprint (t, indent, done)
  done = done or {}
  indent = indent or 0
  for key, value in pairs (t) do
    print (string.rep (" ", indent)) -- indent it
    if type (value) == "table" and not done [value] then
      done [value] = true
      print (tostring (key), ":");
      tprint (value, indent + 2, done)
    else
      print (tostring (key), "=")
      print (value)
    end
  end
end