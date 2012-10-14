pregrid.on_mouse_up = function(button, x, y)
    x = x - grid.left;
    y = y - grid.top;
    --glowire.segments[glowire.current_segment]:add_point(x/scale, y/scale);
    --glowire.segments[glowire.current_segment]:add_point(30,50);
    --echo("X: "..x.." Y:"..y.."\n");
--    list.set_dirty();
--    sel.set_dirty();
--    sel.update_list();
--    grid.set_dirty();
    glowire.new_point = 0;
    
end

pregrid.on_mouse_down = function(button, x, y)
    x = x - grid.left;
    y = y - grid.top - viewer.y_offset;
    if (glowire.current_segment > table.getn(glowire.segments)) then
	glowire:add_segment();
    end
    if (glowire.current_segment == 0) then
	glowire:add_segment();
    end
    if (glowire.new_point == 0) then
	glowire.segments[glowire.current_segment]:add_point(x/scale, y/scale);
	glowire.new_point = 1;
	--list.set_dirty();
	--sel.set_dirty();
	sel.update_list();
    else
	--if (selected_type == "point") then
	    selected_point.x = x/scale;
	    selected_point.y = y/scale;
	--end
    end
    --glowire.segments[glowire.current_segment]:add_point(30,50);
    --echo("X: "..x.." Y:"..y.."\n");
    grid.set_dirty();
end

last_key_time = timer();
global_on_keypress = function(keycode, keyval)
    echo("GLOBAL Keypressed: "..keycode..", "..keyval.."\n");
    if (keycode == 58) then
	shutdown();
    end
    amount = .1;
    -- up
    if (keycode == 84) then
        if (selected_type == "point") then
    	    selected_point.y = selected_point.y - amount;
    	    viewer.before_draw();
	end
    end
    -- down
    if (keycode == 85) then
        if (selected_type == "point") then
	    selected_point.y = selected_point.y + amount;
	    viewer.before_draw();
	end
    end
    -- left
    if (keycode == 82) then
        if (selected_type == "point") then
    	    selected_point.x = selected_point.x - amount;
    	    viewer.before_draw();
	end
    end
    -- right
    if (keycode == 83) then
        if (selected_type == "point") then
    	    selected_point.x = selected_point.x + amount;
    	    viewer.before_draw();
	end
    end

    echo("Timer: "..timer().."\n");
    if (timer() - last_key_time < .1) then
    else
	-- These keys can not repeat in less than 1/4 a second
	-- home 
	if (keycode == 78) then
	    if (glowire.current_segment - 1 > 0) then
		glowire.current_segment = glowire.current_segment - 1;
		glowire.segments[glowire.current_segment].current_point = 0;
		glowire.current_point = 0;
		sel.update_list();
	    end
	end
	-- end
	if (keycode == 79) then
	    if (glowire.current_segment + 1 <= table.getn(glowire.segments)) then
		glowire.current_segment = glowire.current_segment + 1;
		glowire.segments[glowire.current_segment].current_point = 0;
		glowire.current_point = 0;
		sel.update_list();
	    end
	end
	-- pg down
	if (keycode == 81) then
	    if (glowire.current_point + 1 <= table.getn(glowire.segments[glowire.current_segment].points)) then
		glowire.current_point = glowire.current_point + 1;
		glowire.segments[glowire.current_segment].current_point = glowire.current_point;
		sel.update_list();
	    end
	end
	-- pg up
	if (keycode == 80) then
	    if (glowire.current_point - 1 > 0) then
		echo("Current Segment: "..glowire.current_segment..", Point: "..glowire.current_point.."\n");
		glowire.current_point = glowire.current_point - 1;
		glowire.segments[glowire.current_segment].current_point = glowire.current_point;
		sel.update_list();
		echo("Current Segment: "..glowire.current_segment..", Point: "..glowire.current_point.."\n\n");
		
	    end
	end
	-- ins
	if (keycode == 76) then
	    echo("\n ADD SEGMENT \n");
	    if (glowire.current_segment > table.getn(glowire.segments)) then
		glowire:add_segment();
	    end
	    if (glowire.current_segment == 0) then
		glowire:add_segment();
	    end
	    if (table.getn(glowire.segments[glowire.current_segment].points) > 0) then
		last_x = glowire.segments[glowire.current_segment].points[table.getn(glowire.segments[glowire.current_segment].points)].x;
		last_y = glowire.segments[glowire.current_segment].points[table.getn(glowire.segments[glowire.current_segment].points)].y;
	    else
		last_x = 0;
		last_y = 0;
	    end
	    glowire.segments[glowire.current_segment]:add_point(last_x, last_y);
	    list.set_dirty();
	    sel.set_dirty();
	    sel.update_list();
	end
	-- del
	if (keycode == 77) then
	    button4.on_mouse_up();
	end

	--F2
	if (keycode == 48) then
	    save_project();
	end
	--F3
	if (keycode == 49) then
	    load_project(glowire.file_name);
	end
	-- F5
	if (keycode == 51) then
	    echo("Print_Output.Visible == "..tonumber(print_output.visible).."\n");
	    if (tonumber(print_output.visible) == 0) then
		update_preview_source();
		echo(tex.source);
		tex.output_format = 'jpg';
		tex.create_document();
		tex.download_page();
		print_output.jpg = tex.pages[1];
		if tex.page_count > 0 then
		    for i = 1, tex.page_count, 1 do
			os.remove(tex.pages[i]);
		    end
		end
		--echo(tex.pages[1].."\n");
		tex.destroy_document();
		--print_window.width = print_output.width + print_window.margin_left + print_window.margin_right;
		--print_window.height = print_output.height + print_window.margin_top + print_window.margin_bottom;
		print_output.visible = 1;
		grid.visible = 0;
	    else
		print_output.visible = 0;
		grid.visible = 1;
	    end
	    viewer.set_dirty();
	end
	-- F6
	if (keycode == 52) then
	    update_render_source();
	    tex.create_document();
	    tex.download_pdf();
	    id = tex.id;
	    tex.destroy_document();
	end
	-- F8
	if (keycode == 54) then
	end
    
	if (keycode == 55) then
	    if (grid_visible == 0) then
		grid_visible = 1;
	    else
		grid_visible = 0;
	    end
	    viewer.set_dirty();
	end
	
	if (keycode == 60) then
	    if (term_window.visible == 0) then
		term_window.visible = 1;
	    else
		term_window.visible = 0;
	    end
	end
	
    end
    last_key_time = timer();
    
end

set_global_on_keypress(global_on_keypress);

