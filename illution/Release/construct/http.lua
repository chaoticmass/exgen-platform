
port = 2600;
h = listen(port, "");
echo("Listening on port "..port..".\n");
--while 1 == 1 do
    c = accept(h);
    echo("Connection accepted.\n");

    header = [[
HTTP/1.1 200 OK
Server: exgen 
Accept-Ranges: bytes
Connection: close
Content-Type: multipart/x-mixed-replace; boundary=-BoundaryString

]]

    msg = [[-BoundaryString
Content-type: image/jpeg

]]
    send(c, header);
    while 1 == 1 do
	latest_frame = getvar("frames", "latest")
	echo("Latest frame: "..latest_frame.."\n");
	send(c, msg);
	send(c, openfile(latest_frame));
	while latest_frame == getvar("frames", "latest") do
	end
	start_time = timer();
	while timer() - start_time < .1 do
	end
    end
    close(c);
--end
close(h);