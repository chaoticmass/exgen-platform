port = 2601;
h = listen(port, "");
echo("Listening on port "..port..".\n");



    screen = surface:new(0);
    screen.i = 0;
    screen.save_jpg("frame"..screen.i..".jpg");
    
    screen.on_draw = function()
	echo("\n\nON DRAW\n\n");
	if (screen.i ~= screen.last_frame) then
	    screen.save_jpg("frame"..screen.i..".jpg");
	    screen.last_frame = screen.i;
	end
	screen.set_dirty();	
    end
    screen.on_draw();
    screen.set_dirty();




while 1 == 1 do
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
	l_i = screen.i;
	file = openfile("frame"..l_i..".jpg");
	if (file ~= nil) then
	    echo("Sending Frame...\n");
	    send(c, msg);
	    send(c, file);
	    start_time = timer();
	    while timer() - start_time < .1 do
	    end
	    screen.i = screen.i + 1;
	end
    end
    close(c);
    end
close(h);