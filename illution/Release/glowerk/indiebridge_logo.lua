
create_indiebridge_logo = function (model) 
--				  w, h, d, x, y, z
--    create_3d_rectangle(model, 64, 64, 64, 0, 0, 0, makecol(128,0,0), 0);

    d = 1;
    u = 0;
    v = 0;
    c1 = makecol(128+10,0,0);
    c2 = c1;--makecol(0,0,128);
    
    twosixfour = 268;

    a_x = 0;
    a_y = 29;
    b_x = 0;
    b_y = 34;
    c_x = twosixfour;
    c_y = 29;
    d_x = twosixfour;
    d_y = 34;
    
    e_x = 37;
    e_y = 62;
    f_x = 52.5;
    f_y = 6;
    g_x = 68;
    g_y = 62;
    
    h_x = 201;
    h_y = 62;
    i_x = 216.5;
    i_y = 6;
    j_x = 232;
    j_y = 62;
    
    -- a - c
    model.add_face();
    model.add_face();
    model.set_face_data(0, 0, a_x, a_y, 0, u, v, c1); -- a1
    model.set_face_data(0, 1, c_x, c_y, 0, u, v, c1); -- c1
    model.set_face_data(0, 2, a_x, a_y, d, u, v, c1); -- a2
    model.set_face_data(1, 0, c_x, c_y, 0, u, v, c1); -- c1
    model.set_face_data(1, 1, c_x, c_y, d, u, v, c1); -- c2
    model.set_face_data(1, 2, a_x, a_y, d, u, v, c1); -- a2

    -- c - d
    model.add_face();
    model.add_face();
    model.set_face_data(2, 0, c_x, c_y, 0, u, v, c1); -- c1
    model.set_face_data(2, 1, d_x, d_y, 0, u, v, c1); -- d1
    model.set_face_data(2, 2, c_x, c_y, d, u, v, c1); -- c2
    model.set_face_data(3, 0, d_x, d_y, 0, u, v, c1); -- d1
    model.set_face_data(3, 1, d_x, d_y, d, u, v, c1); -- d2
    model.set_face_data(3, 2, c_x, c_y, d, u, v, c1); -- c2

    -- d - b
    model.add_face();
    model.add_face();
    model.set_face_data(4, 0, d_x, d_y, 0, u, v, c1); -- d1
    model.set_face_data(4, 1, b_x, b_y, 0, u, v, c1); -- b1
    model.set_face_data(4, 2, d_x, d_y, d, u, v, c1); -- d2
    model.set_face_data(5, 0, b_x, b_y, 0, u, v, c1); -- b1
    model.set_face_data(5, 1, b_x, b_y, d, u, v, c1); -- b2
    model.set_face_data(5, 2, d_x, d_y, d, u, v, c1); -- d2

    -- b - a
    model.add_face();
    model.add_face();
    model.set_face_data(6, 0, b_x, b_y, 0, u, v, c1); -- b1
    model.set_face_data(6, 1, a_x, a_y, 0, u, v, c1); -- a1
    model.set_face_data(6, 2, b_x, b_y, d, u, v, c1); -- b2
    model.set_face_data(7, 0, a_x, a_y, 0, u, v, c1); -- a1
    model.set_face_data(7, 1, a_x, a_y, d, u, v, c1); -- a2
    model.set_face_data(7, 2, b_x, b_y, d, u, v, c1); -- b2

    -- e - f
    model.add_face();
    model.add_face();
    model.set_face_data(8, 0, e_x, e_y, 0, u, v, c1); -- e1
    model.set_face_data(8, 1, f_x, f_y, 0, u, v, c1); -- f1
    model.set_face_data(8, 2, e_x, e_y, d, u, v, c1); -- e2
    model.set_face_data(9, 0, f_x, f_y, 0, u, v, c1); -- f1
    model.set_face_data(9, 1, f_x, f_y, d, u, v, c1); -- f2
    model.set_face_data(9, 2, e_x, e_y, d, u, v, c1); -- e2

    -- f - g
    model.add_face();
    model.add_face();
    model.set_face_data(10, 0, f_x, f_y, 0, u, v, c1); -- f1
    model.set_face_data(10, 1, g_x, g_y, 0, u, v, c1); -- g1
    model.set_face_data(10, 2, f_x, f_y, d, u, v, c1); -- f2
    model.set_face_data(11, 0, g_x, g_y, 0, u, v, c1); -- g1
    model.set_face_data(11, 1, g_x, g_y, d, u, v, c1); -- g2
    model.set_face_data(11, 2, f_x, f_y, d, u, v, c1); -- f2

    -- g - e
    model.add_face();
    model.add_face();
    model.set_face_data(12, 0, g_x, g_y, 0, u, v, c1); -- g1
    model.set_face_data(12, 1, e_x, e_y, 0, u, v, c1); -- e1
    model.set_face_data(12, 2, g_x, g_y, d, u, v, c1); -- g2
    model.set_face_data(13, 0, e_x, e_y, 0, u, v, c1); -- e1
    model.set_face_data(13, 1, e_x, e_y, d, u, v, c1); -- e2
    model.set_face_data(13, 2, g_x, g_y, d, u, v, c1); -- g2


    -- h - i
    model.add_face();
    model.add_face();
    model.set_face_data(14, 0, h_x, h_y, 0, u, v, c1); -- h1
    model.set_face_data(14, 1, i_x, i_y, 0, u, v, c1); -- i1
    model.set_face_data(14, 2, h_x, h_y, d, u, v, c1); -- h2
    model.set_face_data(15, 0, i_x, i_y, 0, u, v, c1); -- i1
    model.set_face_data(15, 1, i_x, i_y, d, u, v, c1); -- i2
    model.set_face_data(15, 2, h_x, h_y, d, u, v, c1); -- h2

    -- i - j
    model.add_face();
    model.add_face();
    model.set_face_data(16, 0, i_x, i_y, 0, u, v, c1); -- i1
    model.set_face_data(16, 1, j_x, j_y, 0, u, v, c1); -- j1
    model.set_face_data(16, 2, i_x, i_y, d, u, v, c1); -- i2
    model.set_face_data(17, 0, j_x, j_y, 0, u, v, c1); -- j1
    model.set_face_data(17, 1, j_x, j_y, d, u, v, c1); -- j2
    model.set_face_data(17, 2, i_x, i_y, d, u, v, c1); -- i2

    -- j - h
    model.add_face();
    model.add_face();
    model.set_face_data(18, 0, j_x, j_y, 0, u, v, c1); -- j1
    model.set_face_data(18, 1, h_x, h_y, 0, u, v, c1); -- h1
    model.set_face_data(18, 2, j_x, j_y, d, u, v, c1); -- j2
    model.set_face_data(19, 0, h_x, h_y, 0, u, v, c1); -- h1
    model.set_face_data(19, 1, h_x, h_y, d, u, v, c1); -- h2
    model.set_face_data(19, 2, j_x, j_y, d, u, v, c1); -- j2

-- front faces
    -- acd
    model.add_face();
    model.set_face_data(20, 0, a_x, a_y, 0, u, v, c2); -- a
    model.set_face_data(20, 1, c_x, c_y, 0, u, v, c2); -- c
    model.set_face_data(20, 2, d_x, d_y, 0, u, v, c2); -- d

    -- dab
    model.add_face();
    model.set_face_data(21, 0, d_x, d_y, 0, u, v, c2); -- d
    model.set_face_data(21, 1, a_x, a_y, 0, u, v, c2); -- a
    model.set_face_data(21, 2, b_x, b_y, 0, u, v, c2); -- b
    
    -- efg
    model.add_face();
    model.set_face_data(22, 0, e_x, e_y, 0, u, v, c2); -- e
    model.set_face_data(22, 1, f_x, f_y, 0, u, v, c2); -- f
    model.set_face_data(22, 2, g_x, g_y, 0, u, v, c2); -- g
    
    -- hij
    model.add_face();
    model.set_face_data(23, 0, h_x, h_y, 0, u, v, c2); -- h
    model.set_face_data(23, 1, i_x, i_y, 0, u, v, c2); -- i
    model.set_face_data(23, 2, j_x, j_y, 0, u, v, c2); -- j

    bottom_anchor_x = 132-5;
    bottom_anchor_y = 29;
    top_anchor_x = 52.5;
    top_anchor_y = 6;
    anchor_count = 5;
    anchor_span_x = (bottom_anchor_x - top_anchor_x);
    anchor_span_y = (bottom_anchor_y - top_anchor_y);
    anchor_step_x = anchor_span_x / anchor_count;
    anchor_step_y = anchor_span_y / anchor_count;
    face_count = 23;

    for i=0,anchor_count - 1 do
	x1 = top_anchor_x;
	y1 = top_anchor_y + (anchor_step_y * i);
	x2 = bottom_anchor_x - (anchor_step_x * i);
	y2 = bottom_anchor_y;
	x3 = x1;
	y3 = y1 + 1;
	x4 = x2 - 4;
	y4 = y2;

	model.add_face();
	model.add_face();
	face_count = face_count + 1;
	model.set_face_data(face_count, 0, x1, y1, 0, u, v, c1); 
	model.set_face_data(face_count, 1, x2, y2, 0, u, v, c1);
	model.set_face_data(face_count, 2, x4, y4, 0, u, v, c1);
	face_count = face_count + 1;
	model.set_face_data(face_count, 0, x1, y1, 0, u, v, c1);
	model.set_face_data(face_count, 1, x3, y3, 0, u, v, c1);
	model.set_face_data(face_count, 2, x4, y4, 0, u, v, c1);
    
    end

    bottom_anchor_x = 132+5;
    bottom_anchor_y = 29;
    top_anchor_x = 216.5;
    top_anchor_y = 6;
    anchor_count = 5;
    anchor_span_x = (top_anchor_x - bottom_anchor_x);
    anchor_span_y = (bottom_anchor_y - top_anchor_y);
    anchor_step_x = anchor_span_x / anchor_count;
    anchor_step_y = anchor_span_y / anchor_count;

    for i=0,anchor_count - 1 do
	x1 = top_anchor_x;
	y1 = top_anchor_y + (anchor_step_y * i);
	x2 = bottom_anchor_x + (anchor_step_x * i);
	y2 = bottom_anchor_y;
	x3 = x1;
	y3 = y1 + 1;
	x4 = x2 + 4;
	y4 = y2;

	model.add_face();
	model.add_face();
	face_count = face_count + 1;
	model.set_face_data(face_count, 0, x1, y1, 0, u, v, c1); 
	model.set_face_data(face_count, 1, x2, y2, 0, u, v, c1);
	model.set_face_data(face_count, 2, x4, y4, 0, u, v, c1);
	face_count = face_count + 1;
	model.set_face_data(face_count, 0, x1, y1, 0, u, v, c1);
	model.set_face_data(face_count, 1, x3, y3, 0, u, v, c1);
	model.set_face_data(face_count, 2, x4, y4, 0, u, v, c1);
    
    end

    bottom_anchor_x = 296-5;
    bottom_anchor_y = 29;
    top_anchor_x = 216.5;
    top_anchor_y = 6;
    anchor_count = 5;
    anchor_span_x = (bottom_anchor_x - top_anchor_x);
    anchor_span_y = (bottom_anchor_y - top_anchor_y);
    anchor_step_x = anchor_span_x / anchor_count;
    anchor_step_y = anchor_span_y / anchor_count;

    for i=0,anchor_count - 1 do
	x1 = top_anchor_x;
	y1 = top_anchor_y + (anchor_step_y * i);
	x2 = bottom_anchor_x - (anchor_step_x * i);
	y2 = bottom_anchor_y;

	x3 = x1;
	y3 = y1 + 1;
	x4 = x2 - 4;
	y4 = y2;


	a = math.atan2(y2-y1, x2-x1);
	if (x2 > twosixfour) then
	    x2 = twosixfour;
	    t_x2 = x1;
	    t_y2 = y1;
	    while (t_x2 < x2) do
		t_x2 = t_x2 + math.cos(a)*1;
		t_y2 = t_y2 + math.sin(a)*1;
	    end
	    x2 = t_x2;
	    y2 = t_y2;
	    
	    y4 = y2 + math.sin(a)*4;
	    x4 = x2;

	end

	
	model.add_face();
	model.add_face();
	face_count = face_count + 1;
	model.set_face_data(face_count, 0, x1, y1, 0, u, v, c1); 
	model.set_face_data(face_count, 1, x2, y2, 0, u, v, c1);
	model.set_face_data(face_count, 2, x4, y4, 0, u, v, c1);
	face_count = face_count + 1;
	model.set_face_data(face_count, 0, x1, y1, 0, u, v, c1);
	model.set_face_data(face_count, 1, x3, y3, 0, u, v, c1);
	model.set_face_data(face_count, 2, x4, y4, 0, u, v, c1);
    
    end


    bottom_anchor_x = -27 + 5;
    bottom_anchor_y = 29;
    top_anchor_x = 52.5;
    top_anchor_y = 6;
    anchor_count = 5;
    anchor_span_x = (bottom_anchor_x - top_anchor_x);
    anchor_span_y = (bottom_anchor_y - top_anchor_y);
    anchor_step_x = anchor_span_x / anchor_count;
    anchor_step_y = anchor_span_y / anchor_count;

    for i=0,anchor_count - 1 do
	x1 = top_anchor_x;
	y1 = top_anchor_y + (anchor_step_y * i);
	x2 = bottom_anchor_x - (anchor_step_x * i);
	y2 = bottom_anchor_y;
	x3 = x1;
	y3 = y1 + 1;
	x4 = x2 - 4;
	y4 = y2;

	a = math.atan2(y2-y1, x2-x1);
	
	if (x2 < 0) then
	    x2 = 0;
	    t_x2 = x1;
	    t_y2 = y1;
	    while (t_x2 > x2) do
		t_x2 = t_x2 + math.cos(a)*1;
		t_y2 = t_y2 + math.sin(a)*1;
	    end
	    x2 = t_x2;
	    y2 = t_y2;
	    
	    y4 = y2 + math.sin(a)*4;
	    x4 = x2;
	else 
	    x3 = x1;
	    y3 = y1 + 1;
	    x4 = x2 + 4;
	    y4 = y2;
	end

	
	model.add_face();
	model.add_face();

	face_count = face_count + 1;
	model.set_face_data(face_count, 0, x1, y1, 0, u, v, c1); 
	model.set_face_data(face_count, 1, x2, y2, 0, u, v, c1);
	model.set_face_data(face_count, 2, x4, y4, 0, u, v, c1);
	face_count = face_count + 1;
	model.set_face_data(face_count, 0, x1, y1, 0, u, v, c2);
	model.set_face_data(face_count, 1, x3, y3, 0, u, v, c2);
	model.set_face_data(face_count, 2, x4, y4, 0, u, v, c2);
    
    end
    
--[[    tex = "\\newcommand{\\indiebridgelogosixtythreepx}{";
    tex = tex .. "\\begin{tikzpicture}\n";
    scale = .00054 * 63;
    x_offset = -100;
    y_offset = 0;
    line_width = ".01pt";
    for index = 1, face_count do
	x1, y1, z1, u1, v1, c1 = model.get_face_data(index, 0);
	x2, y2, z2, u2, v2, c2 = model.get_face_data(index, 1);
	x3, y3, z3, u3, v3, c3 = model.get_face_data(index, 2);
	
	tex = tex .. "\n \\filldraw[line width="..line_width..",color=orange]\n";
	tex = tex .. "("..(x_offset + (x1 * scale))..","..(y_offset + (0-y1 * scale))..") -- ";
	tex = tex .. "("..(x_offset + (x2 * scale))..","..(y_offset + (0-y2 * scale))..") -- ";
	tex = tex .. "("..(x_offset + (x3 * scale))..","..(y_offset + (0-y3 * scale))..") -- ";
	tex = tex .. "cycle;\n";
    end
    tex = tex .. "\\end{tikzpicture} }\n";
    
    savefile('/root/projects/latex/indiebridge/logo.tex', tex);]]
end