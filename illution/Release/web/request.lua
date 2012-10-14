    echo("-i: "..getvar("sockfd", "count").."\n");
    c = getvar("sockfd", getvar("sockfd", "count"));
    echo("Connection accepted.\n");

    request = recv(c);
--    request = [[GET /?caption=Da HTTP/1.1
--]]
    echo(request.."\n\n");
    pos = string.find(request, "\n", 1, 1);
    request = string.sub(request, 1, pos);
    if (string.sub(request, 1, 4) == "GET ") then
	pos = string.find(request, " HTTP/", 1, 1);
	request = string.sub(request, 5, pos-1);
	echo("Request: "..request.."\n");
	file_pos = string.find(request, "?", 1, 1);
	if (type(file_pos) ~= "nil") then
	    file = string.sub(request, 1, file_pos -1);
	    query = string.sub(request, file_pos+1, -1);
	else
	    file = string.sub(request, 1, -1);
	    query = "";
	end
	
	echo("File: "..file.."\nQuery: "..query.."\n");
	if (query ~= "") then
	    query = query .. "&";
	    last_pos = 1;
	    pos = string.find(query, "&", 1, 1);
	    index = 0;
	    arguments = {};
	    while (type(pos) ~= "nil") do
		index = index + 1;
		line = string.sub(query, last_pos, pos-1);
		equal_pos = string.find(line, "=", 1, 1);
		if (type(equal_pos) == "nil") then
		    variable_name = line;
		    variable_value = "";
		else
		    variable_name = string.sub(line, 1, equal_pos-1);
		    variable_value = string.sub(line, equal_pos+1, -1);
		end
		--[[arguments[index] = {};
		arguments[index].name = variable_name;
		arguments[index].value = variable_value;
		echo("Argument["..index.."].name = "..arguments[index].name.."\n");
		echo("Argument["..index.."].value = "..arguments[index].value.."\n\n");]]
		setvar("query", variable_name, variable_value);
		echo(variable_name.." = "..variable_value.."\n");
		last_pos = pos+1;
		pos = string.find(query, "&", last_pos+1, 1);
	    end
	end
    end


    header = [[
HTTP/1.1 200 OK
Server: exgen 
Accept-Ranges: bytes
Connection: close
Content-Type: text/html;

]]


    send(c, header);
    send(c, [[
Hey
<form action="" method=POST>
</form>
]]);
    close(c);
