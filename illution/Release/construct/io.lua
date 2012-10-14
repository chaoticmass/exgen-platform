

global_on_keypress = function(keycode, keyval)
    echo("Keycode: "..keycode..", Keyval: "..keyval.."\n");

    amount = .01;

    if (keycode == 58) then
	shutdown();
    end

    if (keycode == 74) then
	if (entity[selected_entity].visible == 1) then
	    entity[selected_entity].visible = 0;
	else
	    entity[selected_entity].visible = 1;
	end
    end

    if (keycode == 79) then
	main.pitch = main.pitch - 1;
    elseif (keycode == 78) then
	main.pitch = main.pitch + 1;
    end
    if (keycode == 77) then
	main.heading = main.heading - 1;
    elseif (keycode == 81) then
	main.heading = main.heading + 1;
    end


    if (keycode == 84) then
	main.x = main.x + amount;
    elseif (keycode == 85) then
	main.x = main.x - amount;
    end

    if (keycode == 82) then
	main.y = main.y + amount;
    elseif (keycode == 83) then
	main.y = main.y - amount;
    end
    if (keycode == 1) then
	main.z = main.z + amount;
    elseif (keycode == 26) then
	main.z = main.z - amount;
    end


--[[
    if (keycode == 47) then 
	if (model_window.visible == 0) then
	    model_window.visible = 1;
	else 
	    model_window.visible = 0;
	end
    end
]]
    if (keycode == 48) then 
	if (prop_window.visible == 0) then
	    prop_window.visible = 1;
	else 
	    prop_window.visible = 0;
	end
    end
--[[
    if (keycode == 49) then 
	if (anim_window.visible == 0) then
	    anim_window.visible = 1;
	else 
	    anim_window.visible = 0;
	end
    end


    if (keycode == 50) then 
	if (term_window.visible == 0) then
	    term_window.visible = 1;
	else 
	    term_window.visible = 0;
	end
    end
]]
    -- F5
    if (keycode == 51) then
	tex.output_format = 'jpg';
	tex.create_document();
	tex.download_page();
	print_output.jpg = tex.pages[1];
	--echo(tex.pages[1].."\n");
	tex.destroy_document();
	print_window.width = print_output.width + print_window.margin_left + print_window.margin_right;
	print_window.height = print_output.height + print_window.margin_top + print_window.margin_bottom;
    end
    -- F6
    if (keycode == 52) then
	tex.create_document();
	tex.download_pdf();
	id = tex.id;
	tex.destroy_document();
    end
    
    -- F8
    if (keycode == 54) then
	--save("global", _G);
	save("entity", entity);
	savefile( "source.tex", tex.source);
	echo(tex.source);
    
	text = "";
	text = text .. "\nCamera:\n";
	text = text .. "	X:	"..main.x.."\n";
	text = text .. "	Y:	"..main.y.."\n";
	text = text .. "	Z:	"..main.z.."\n";
	text = text .. "	Pitch:	"..main.pitch.."\n";
	text = text .. "	Heading:	"..main.heading.."\n";
	
	for i=1,entity_count,1 do
	    if type(entity[i]) ~= "nil" then
		if (entity[i].visible == 1) then
		    text = text .. "\nEntity "..i..":\n";
		    text = text .. "	Model:	"..entity[i].model.index.."\n";
		    text = text .. "	X:	"..entity[i].x.."\n";
		    text = text .. "	Y:	"..entity[i].y.."\n";
		    text = text .. "	Z:	"..entity[i].z.."\n";
		    text = text .. "	RX:	"..entity[i].r_x.."\n";
		    text = text .. "	RY:	"..entity[i].r_y.."\n";
		    text = text .. "	RZ:	"..entity[i].r_z.."\n";
		    text = text .. "	Scale:	"..entity[i].scale.."\n";
		end
	    end
	end
	savefile("save.txt", text);
    end


    -- -
    if (keycode == 62) then
	main.surface.width = screen_width();
	main.surface.height = screen_height();
	main.initialize();
    end
    -- +
    if (keycode == 61) then
	main.surface.width = 63*2;
	main.surface.height = 34*2;
	main.initialize();
    end

    if (keyval == 44) then
    	entity_controls = 0;
	vertex_controls = 1;    
	selected_face = 0;
	selected_model = selected_model - 1;
	if (selected_model < 1) then
	    selected_model = model_count;
	end
    end
    if (keyval == 46) then
    	entity_controls = 0;
	vertex_controls = 1;

	selected_face = 0;
	selected_model = selected_model + 1;
	if (selected_model > model_count) then
	    selected_model = 1;
	end
    end

    if (keyval == 60) then
	entity_controls = 1;
	vertex_controls = 0;
	selected_entity = selected_entity - 1;
	if (selected_entity < 1) then
	    selected_entity = entity_count;
	end
    end
    if (keyval == 62) then
	entity_controls = 1;
	vertex_controls = 0;
	selected_entity = selected_entity + 1;
	if (selected_entity > entity_count) then
	    selected_entity = 1;
	end
    end
    
    if (keycode == 68) then
	selected_face = selected_face - 1;
	if (selected_face < 0) then
	    selected_face = model[selected_model].face_count;
	end
    end
    if (keycode == 69) then
	selected_face = selected_face + 1;
	if (selected_face > model[selected_model].face_count) then
	    selected_face = 0;
	end
    end
    if (keycode == 65) then
	selected_vertex = selected_vertex - 1;
	if (selected_vertex < 0) then
	    selected_vertex = 2;
	end
    end
    if (keycode == 66) then
	selected_vertex = selected_vertex + 1;
	if (selected_vertex > 2) then
	    selected_vertex = 0;
	end
    end

    if (vertex_controls == 1) then
    if (keycode == 41) then
	x, y, z, u, v, c = model[selected_model].get_face_data(selected_face, selected_vertex);
	x = x - 0.1;
	model[selected_model].set_face_data(selected_face, selected_vertex, x, y, z, u, v, c);
    end
    if (keycode == 43) then
	x, y, z, u, v, c = model[selected_model].get_face_data(selected_face, selected_vertex);
	x = x + 0.1;
	model[selected_model].set_face_data(selected_face, selected_vertex, x, y, z, u, v, c);
    end
    if (keycode == 45) then
	x, y, z, u, v, c = model[selected_model].get_face_data(selected_face, selected_vertex);
	y = y - 0.1;
	model[selected_model].set_face_data(selected_face, selected_vertex, x, y, z, u, v, c);
    end
    if (keycode == 39) then
	x, y, z, u, v, c = model[selected_model].get_face_data(selected_face, selected_vertex);
	y = y + 0.1;
	model[selected_model].set_face_data(selected_face, selected_vertex, x, y, z, u, v, c);
    end
    end
    
    if (entity_controls == 1) then
	if (keycode == 41) then
	    entity[selected_entity].y = entity[selected_entity].y + 0.01;
	end
	if (keycode == 43) then
	    entity[selected_entity].y = entity[selected_entity].y - 0.01;
	end
	if (keycode == 45) then
	    entity[selected_entity].z = entity[selected_entity].z + 0.01;
	end
	if (keycode == 39) then
	    entity[selected_entity].z = entity[selected_entity].z - 0.01;
	end
	if (keycode == 44) then
	    entity[selected_entity].r_x = entity[selected_entity].r_x + 0.1;
	end
	if (keycode == 46) then
	    entity[selected_entity].r_x = entity[selected_entity].r_x - 0.1;
	end

	if (keycode == 38) then
	    entity[selected_entity].r_y = entity[selected_entity].r_y + 0.1;
	end
	if (keycode == 40) then
	    entity[selected_entity].r_y = entity[selected_entity].r_y - 0.1;
	end

	if (keycode == 89) then
	    entity[selected_entity].scale = entity[selected_entity].scale + 0.0005;
	end
	if (keycode == 88) then
	    entity[selected_entity].scale = entity[selected_entity].scale - 0.0005;
	end
	if (keycode == 86) then
	    current_model = entity[selected_entity].model.index;
	    current_model = current_model - 1;
	    if (current_model < 1) then
		current_model = model_count;
	    end
	    entity[selected_entity].model = model[current_model];
	end
	if (keycode == 87) then
	    current_model = entity[selected_entity].model.index;
	    current_model = current_model + 1;
	    if (current_model > model_count) then
		current_model = 1;
	    end
	    entity[selected_entity].model = model[current_model];
	end


    end

    --echo("Model: "..selected_model..", Faces: "..model[selected_model].face_count.."\n");
    if (keycode == 28) then
	reset_entities();
    end
    
    if (keycode == 7) then
	if (grid_visible == 0) then
	    grid_visible = 1;
	else
	    grid_visible = 0;
	end
    end
end

set_global_on_keypress(global_on_keypress);

