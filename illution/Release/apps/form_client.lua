include("lib/form.lua");


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

next_form = "scan_upc";
setvar("form", "next_form", next_form)
setvar("form", "post", "");
setvar("form", "id", "");
while next_form ~= "" do
run_string([[
    include("lib/form.lua");
    font1 = surface:new();
    font1.visible = 0;
    font1.bmp = 'fonts/black_micro_computer.bmp';

    elements = load_form(getvar("form", "next_form"), getvar("form", "id"));
    if (type(elements.request.next_form) == "string") then
	next_form = elements.request.next_form;
	setvar("form", "next_form", next_form);
    end
    echo("Next form "..next_form.."\n");
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
    post = post .. "id=" .. tostring(elements.request.id) .. "&";
    post = post .. "dataset=" .. tostring(elements.request.dataset) .. "&";    
    echo("Echoing POST:"..post.."\n");
    setvar("form", "post", post);
    setvar("form", "id", elements.request.id);
    
    echo("\n Posting Data...\n");
    data = download_string("65.66.245.90", "/db/form_action.php", post);
    echo("It worked\n\n");
    echo(data.."\n");
    
    ]]);
    next_form = getvar("form", "next_form", next_form);
    echo("Next form: "..next_form.."\n");
    echo("id: ");
    echo(getvar("form", "id"));
    echo("\n");
end

