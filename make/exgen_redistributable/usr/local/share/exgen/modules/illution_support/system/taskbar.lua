font1 = surface:new();
font1.visible = 0;
font1.bmp = "fonts/black_micro_computer.bmp";

task = surface:new();
task.height = 32;
task.width = screen_width();
task.left = 0;
task.top = screen_height() - task.height;
task.forecolor = getvar("colors", "widget-forecolor");
task.backcolor = getvar("colors", "widget-backcolor");
task.gradient(task.forecolor, task.backcolor);
task.z = 9999;

button1 = button:new();
button1.font = font1;
button1.parent = task;
button1.top = 7;
button1.left = 3;
button1.height = 26;
button1.caption = "illution";

button1.on_click = function ()
--    branch("apps/dead_simple.lua");
    branch("apps/windowed_physics_sia.lua");
end

button1.visible = 1;
task.visible = 1;