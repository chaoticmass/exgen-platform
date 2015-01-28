initialize_video(1280, 760);

draw_color = makecol(0, 0, 0);
scratch_color = makecol(128, 128, 128);

l = surface:new();
l.top = 0;
l.left = 0;
l.width = screen_width();
l.height = screen_height();
l.backcolor = makecol(255, 255, 255);
l.forecolor = scratch_color;
l.clear();

origin_x = 320;
origin_y = 240;
a_b_length = 100;

points = {};
points.a = {x = origin_x, y = origin_y};
points.b = {x = points.a.x+a_b_length, y = points.a.y};

-- line a - b
-- 0 is 3-o-clock, 64 is noon
l.forecolor = draw_color;
l.line(points.a.x, points.a.y, points.b.x, points.b.y);
do_arc(points.a.x, points.a.y, 0, 64, a_b_length, function(x, y) 
    l.forecolor = scratch_color;
    l.put_pixel(x, y); 
end);
l.print(points.a.x, points.a.y, "a");
l.print(points.b.x, points.b.y, "b");

do_arc(points.b.x, points.b.y, 64, 128, a_b_length, function(x, y) 
    l.forecolor = scratch_color;
    if (l.get_pixel(x, y) == scratch_color) then
	echo("X: "..x.." Y: "..y.."\n");
	points.c = {};
	points.c.x = x;
	points.c.y = y;
    end
    l.put_pixel(x, y);
end);
l.print(points.c.x, points.c.y, "c");

l.forecolor = draw_color;
l.line(points.b.x, points.b.y, points.c.x, points.c.y);

-- a1 to a2 in counter-clockwise
-- 0 is 3-o-clock, 64 is noon
-- find half way between a - b, call it c1
do_arc(points.b.x, points.b.y, 64+16, 48+128, a_b_length*.555, function(x, y) 
    l.forecolor = scratch_color;
    l.put_pixel(x, y);
end);
do_arc(points.a.x, points.a.y, 256 - 48, 48, a_b_length*.555, function(x, y) 
    if (l.get_pixel(x, y) == scratch_color) then
	if (type(points.c1) == "nil") then
	    points.c1 = {};
	end
	if (type(points.c1.x1) == "nil") then
	    points.c1.x1 = x;
	    points.c1.y1 = y;
	else
	    points.c1.x2 = x;
	    points.c1.y2 = y;
	end
    end
    l.forecolor = scratch_color;
    l.put_pixel(x, y);
end);

l.line(points.c1.x1, points.c1.y1, points.c1.x2, points.c1.y2);
a_c1_length = points.c1.x1 - points.a.x;
-- find half way between a - c1, call it c2
do_arc(points.c1.x1, points.a.y, 64+16, 48+128, a_c1_length*.555, function(x, y) 
    l.forecolor = scratch_color;
    l.put_pixel(x, y);
end);
do_arc(points.a.x, points.a.y, 256-32, 48, a_c1_length*.555, function(x, y) 
    if (l.get_pixel(x, y) == scratch_color) then
	if (type(points.c2) == "nil") then
	    points.c2 = {};
	end
	if (type(points.c2.x1) == "nil") then
	    points.c2.x1 = x;
	    points.c2.y1 = y;
	else
	    points.c2.x2 = x;
	    points.c2.y2 = y;
	end
    end
    l.forecolor = scratch_color;
    l.put_pixel(x, y);
end);
l.line(points.c2.x1, points.c2.y1, points.c2.x2, points.c2.y2);

a_c2_length = points.c2.x1 - points.a.x;
-- find half way between a - c2, call it d
do_arc(points.c2.x1, points.a.y, 64, 48+128, a_c2_length*.51, function(x, y) 
    l.forecolor = scratch_color;
    l.put_pixel(x, y);
end);

do_arc(points.a.x, points.a.y, 256 - 48, 48, a_c2_length*.666, function(x, y) 
    if (l.get_pixel(x, y) == scratch_color) then
	if (type(points.d) == "nil") then
	    points.d = {};
	end
	if (type(points.d.x1) == "nil") then
	    points.d.x1 = x;
	    points.d.y1 = y;
	else
	    points.d.x2 = x;
	    points.d.y2 = y;
	end
    end
    l.forecolor = scratch_color;
    l.put_pixel(x, y);
end);
l.line(points.d.x1, points.d.y1, points.d.x2, points.d.y2);
points.d.x = points.d.x1;
points.d.y = points.a.y;
l.print(points.d.x, points.d.y, "d");

a_d_length = points.d.x - points.a.x;
do_circle(points.a.x, points.a.y, a_d_length, function(x, y)
    l.forecolor = scratch_color;
    l.put_pixel(x, y);
end);

do_line(points.a.x, points.a.y, points.c.x, points.c.y, function(x, y)
    if (l.get_pixel(x, y) == scratch_color) then
	if (type(points.e) == "nil") then
	    points.e = {};
	end
	if (type(points.e.x) == "nil") then
	    points.e.x = x;
	    points.e.y = y;
	end
    end
    l.forecolor = scratch_color;
    l.put_pixel(x, y);
end);
l.print(points.e.x, points.e.y, "e");

do_circle(points.b.x, points.b.y, a_d_length, function(x, y)
    l.forecolor = scratch_color;
    l.put_pixel(x, y);
end);

do_circle(points.c.x, points.c.y, a_d_length, function(x, y)
    l.forecolor = scratch_color;
    l.put_pixel(x, y);
end);

l.visible = 1;
render_loop();