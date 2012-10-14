
font1 = surface:new();
font1.visible = 0;
font1.bmp = "fonts/black_small_computer.bmp";

font2 = surface:new();
font2.visible = 0;
font2.bmp = "fonts/black_micro_computer.bmp";


window1 = window:new();
window1.font = font2;
window1.width = 320;
window1.height = 240;
window1.left = (screen_width() * .5) - (window1.width * .5);
window1.top = (screen_height() * .5) - (window1.height * .5);
window1.caption = "Window";
window1.z = 9999;
window1.visible = 1;

--[[
window2 = window:new();
window2.font = font2;
window2.width = 320;
window2.height = 240;
window2.left = 15+(screen_width() * .5) - (window2.width * .5);
window2.top = 15+(screen_height() * .5) - (window2.height * .5);
window2.caption = "Window";
window2.z = 9999;
window2.visible = 1;
]]

button1 = button:new();
button1.font = font1;
button1.parent = window1;
button1.top = window1.margin_top;

echo("Win!\n");