include("lib/form.lua");
font1 = surface:new();
font1.visible = 0;
font1.bmp = 'fonts/small.bmp';


dump = function(thing)
    for key, value in pairs(thing) do
	echo("Key: ");
	echo(key);

	echo(" Type: ");
	echo(type(value));

	if (type(value) == "table") then
	    echo("\n");	
	    dump(value);
	else
	    echo(" Value: ");
	    echo(value);
	    echo("\n");
	end
    end
end

elements = load_form("login");

post = "";
for key, value in pairs(elements) do
    if (type(value) == "table") then
	if (type(value.db) == "string") then
	    if (type(value.text) == "string") then
		post = post .. value.db .. "=" .. value.text .."&";
	    elseif (type(value.value) == "string") then
		post = post .. value.db .. "=" .. value.value .."&";
	    end
	end
    end
end
echo(post.."\n");

data = download_string("65.66.245.90", "/db/form_action.php", post);
echo(data.."\n");
