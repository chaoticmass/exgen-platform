

model_window = window:new();
model_window.font = title_font;
model_window.top = 0;
model_window.left = 0;
model_window.width = 320 + 6;
model_window.height = 140;
model_window.visible = 1;
model_window.caption = "Model Selector";
echo(model_window.surface.." model_window \n");

model_list = listbox:new();
model_list.parent = model_window;
model_list.visible = 1;
model_list.top = model_window.margin_top;
model_list.left = model_window.margin_left;
model_list.height = model_window.height - model_window.margin_top - model_window.margin_bottom;
echo(model_list.height.."\n");
model_list.width = model_window.width - model_window.margin_left - model_window.margin_right;

model_list.font = body_font;
model_list.selected = 1;
model_list.list = {"Spaceship", "Planet", "Asteroid", "Space Station", "Satellite", "Debris", "Spacecraft 2", "Foo", "Bar", "Baz", "Biz", "Boo"};


model_window.on_resize = function ()
    model_list.top = model_window.margin_top;
    model_list.left = model_window.margin_left;
    model_list.width = model_window.width - model_window.margin_left - model_window.margin_right;
    model_list.height = model_window.height - model_window.margin_top - model_window.margin_bottom;
end

model_list.on_change = function ()
    if model_list.selected == 1 then
	entity[1].model = model[1];
    elseif model_list.selected == 2 then
	entity[1].model = model[2];
    end
end
