

prop_window = window:new();
prop_window.font = title_font;
prop_window.top = 0;
prop_window.left = 0;
prop_window.width = 320;
prop_window.height = 240;
prop_window.visible = 1;
prop_window.caption = "Entity Properties";
echo(prop_window.surface.." prop_window \n");

model_x_label = label:new();
model_x_label.parent = prop_window;
model_x_label.font = body_font;
model_x_label.top = prop_window.align_top(model_x_label);
model_x_label.left = prop_window.align_left(model_x_label);
model_x_label.caption = "X:";

model_x_input = inputbox:new();
model_x_input.parent = prop_window;
model_x_input.font = body_font;
model_x_input.top = model_x_label.top;
model_x_input.left = prop_window.align_left(model_x_input);
model_x_input.text = entity[4].x;
model_x_input.drag = 0;

model_rx_label = label:new();
model_rx_label.parent = prop_window;
model_rx_label.font = body_font;
model_rx_label.top = model_x_label.top;
model_rx_label.left = prop_window.align_left(model_rx_label);
model_rx_label.caption = "RX:";

model_rx_input = inputbox:new();
model_rx_input.parent = prop_window;
model_rx_input.font = body_font;
model_rx_input.top = model_rx_label.top;
model_rx_input.left = prop_window.align_left(model_rx_input);
model_rx_input.text = entity[4].r_x;
model_rx_input.drag = 0;


prop_window.align_clear_left();

model_y_label = label:new();
model_y_label.parent = prop_window;
model_y_label.font = body_font;
model_y_label.top = prop_window.align_top(model_y_label);
model_y_label.left = prop_window.align_left(model_y_label);
model_y_label.caption = "Y:";

model_y_input = inputbox:new();
model_y_input.parent = prop_window;
model_y_input.font = body_font;
model_y_input.top = model_y_label.top;
model_y_input.left = prop_window.align_left(model_y_input);
model_y_input.text = entity[4].y;
model_y_input.drag = 0;

model_ry_label = label:new();
model_ry_label.parent = prop_window;
model_ry_label.font = body_font;
model_ry_label.top = model_y_label.top;
model_ry_label.left = prop_window.align_left(model_ry_label);
model_ry_label.caption = "RY:";

model_ry_input = inputbox:new();
model_ry_input.parent = prop_window;
model_ry_input.font = body_font;
model_ry_input.top = model_ry_label.top;
model_ry_input.left = prop_window.align_left(model_ry_input);
model_ry_input.text = entity[4].r_y;
model_ry_input.drag = 0;


prop_window.align_clear_left();
model_z_label = label:new();
model_z_label.parent = prop_window;
model_z_label.font = body_font;
model_z_label.top = prop_window.align_top(model_z_label);
model_z_label.left = prop_window.align_left(model_z_label);
model_z_label.caption = "Z:";

model_z_input = inputbox:new();
model_z_input.parent = prop_window;
model_z_input.font = body_font;
model_z_input.top = model_z_label.top;
model_z_input.left = prop_window.align_left(model_z_input);
model_z_input.text = entity[4].z;
model_z_input.drag = 0;

model_rz_label = label:new();
model_rz_label.parent = prop_window;
model_rz_label.font = body_font;
model_rz_label.top = model_z_label.top;
model_rz_label.left = prop_window.align_left(model_rz_label);
model_rz_label.caption = "RZ:";

model_rz_input = inputbox:new();
model_rz_input.parent = prop_window;
model_rz_input.font = body_font;
model_rz_input.top = model_rz_label.top;
model_rz_input.left = prop_window.align_left(model_rz_input);
model_rz_input.text = entity[4].r_z;
model_rz_input.drag = 0;


prop_window.align_clear_left();
model_scale_label = label:new();
model_scale_label.parent = prop_window;
model_scale_label.font = body_font;
model_scale_label.top = prop_window.align_top(model_scale_label);
model_scale_label.left = prop_window.align_left(model_scale_label);
model_scale_label.caption = "Scale:";

model_scale_input = inputbox:new();
model_scale_input.parent = prop_window;
model_scale_input.font = body_font;
model_scale_input.top = model_scale_label.top;
model_scale_input.left = prop_window.align_left(model_scale_input);
model_scale_input.text = entity[4].scale;
model_scale_input.drag = 0;

model_scale_input.on_return = function () 
    entity[4].scale = model_scale_input.text;
end
model_scale_input.on_mouse_down = function ()
    x, y = mouse_xy();
    if (model_scale_input.drag == 0) then
	model_scale_input.drag = 1;
	model_scale_input.drag_start = y;
	model_scale_input.start = model_scale_input.text;
    end
    model_scale_input.text = model_scale_input.start + ((y - model_scale_input.drag_start)*.01);
    model_scale_input.on_return();
end
model_scale_input.on_mouse_up = function ()
    if (model_scale_input.drag == 1) then
	model_scale_input.drag = 0;
    end
end



model_x_input.on_return = function () 
    entity[4].x = model_x_input.text;
    entity[4].body.set_position(entity[4].x, entity[4].y, entity[4].z);
end
model_x_input.on_mouse_down = function ()
    x, y = mouse_xy();
    if (model_x_input.drag == 0) then
	model_x_input.drag = 1;
	model_x_input.drag_start = y;
	model_x_input.start = model_x_input.text;
    end
    model_x_input.text = model_x_input.start + (y - model_x_input.drag_start);
    model_x_input.on_return();
end
model_x_input.on_mouse_up = function ()
    if (model_x_input.drag == 1) then
	model_x_input.drag = 0;
    end
end

model_y_input.on_return = function () 
    entity[4].y = model_y_input.text;
    entity[4].body.set_position(entity[4].x, entity[4].y, entity[4].z);
end
model_y_input.on_mouse_down = function ()
    x, y = mouse_xy();
    if (model_y_input.drag == 0) then
	model_y_input.drag = 1;
	model_y_input.drag_start = y;
	model_y_input.start = model_y_input.text;
    end
    model_y_input.text = model_y_input.start + (y - model_y_input.drag_start);
    model_y_input.on_return();
end
model_y_input.on_mouse_up = function ()
    if (model_y_input.drag == 1) then
	model_y_input.drag = 0;	
    end
end



model_z_input.on_return = function () 
    entity[4].z = model_z_input.text;
    entity[4].body.set_position(entity[4].x, entity[4].y, entity[4].z);
end
model_z_input.on_mouse_down = function ()
    x, y = mouse_xy();
    if (model_z_input.drag == 0) then
	model_z_input.drag = 1;
	model_z_input.drag_start = y;
	model_z_input.start = model_z_input.text;
    end
    model_z_input.text = model_z_input.start + (y - model_z_input.drag_start);
    model_z_input.on_return();
end
model_z_input.on_mouse_up = function ()
    if (model_z_input.drag == 1) then
	model_z_input.drag = 0;	
    end
end


model_rx_input.on_return = function () 
    entity[4].r_x = model_rx_input.text;
    entity[4].body.set_rotation(entity[4].r_x, entity[4].r_y, entity[4].r_z);
    entity[4].body.set_position(entity[4].x, entity[4].y, entity[4].z);
end
model_rx_input.on_mouse_down = function ()
    x, y = mouse_xy();
    if (model_rx_input.drag == 0) then
	model_rx_input.drag = 1;
	model_rx_input.drag_start = y;
	model_rx_input.start = model_rx_input.text;
    end
    model_rx_input.text = model_rx_input.start + (y - model_rx_input.drag_start);
    model_rx_input.on_return();
end
model_rx_input.on_mouse_up = function ()
    if (model_rx_input.drag == 1) then
	model_rx_input.drag = 0;
    end
end

model_ry_input.on_return = function () 
    entity[4].r_y = model_ry_input.text;
    entity[4].body.set_rotation(entity[4].r_x, entity[4].r_y, entity[4].r_z);
    entity[4].body.set_position(entity[4].x, entity[4].y, entity[4].z);
end
model_ry_input.on_mouse_down = function ()
    x, y = mouse_xy();
    if (model_ry_input.drag == 0) then
	model_ry_input.drag = 1;
	model_ry_input.drag_start = y;
	model_ry_input.start = model_ry_input.text;
    end
    model_ry_input.text = model_ry_input.start + (y - model_ry_input.drag_start);
    model_ry_input.on_return();
end
model_ry_input.on_mouse_up = function ()
    if (model_ry_input.drag == 1) then
	model_ry_input.drag = 0;	
    end
end



model_rz_input.on_return = function () 
    entity[4].r_z = model_rz_input.text;
    entity[4].body.set_rotation(entity[4].r_x, entity[4].r_y, entity[4].r_z);
    entity[4].body.set_position(entity[4].x, entity[4].y, entity[4].z);
end
model_rz_input.on_mouse_down = function ()
    x, y = mouse_xy();
    if (model_rz_input.drag == 0) then
	model_rz_input.drag = 1;
	model_rz_input.drag_start = y;
	model_rz_input.start = model_rz_input.text;
    end
    model_rz_input.text = model_rz_input.start + (y - model_rz_input.drag_start);
    model_rz_input.on_return();
end
model_rz_input.on_mouse_up = function ()
    if (model_rz_input.drag == 1) then
	model_rz_input.drag = 0;	
    end
end
