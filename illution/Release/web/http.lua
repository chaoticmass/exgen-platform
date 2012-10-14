
logo = surface:new();
logo.parent = bg;
logo.gif = "glowerk/backgrounds/illution_logo.gif";
logo.top = bg.height - logo.height;
logo.left = bg.width - logo.width;	

port = 2602;
h = listen(port, "");
echo("Listening on port "..port..".\n");
i = 0;
while 1 == 1 do
    echo("Waiting for a connection...\n");
    c = accept(h);
    i = i + 1;
    setvar("sockfd", "count", i);
    setvar("sockfd", getvar("sockfd", "count"), c);
    echo("i: "..i.."\n");
    branch("web/request.lua");
end
close(h);