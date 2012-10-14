
font1 = surface:new();
font1.visible = 0;
font1.bmp = "fonts/black_small_computer.bmp";

font2 = surface:new();
font2.visible = 0;
font2.bmp = "fonts/black_micro_computer.bmp";


window1 = window:new();
window1.font = font1;
window1.width = 135;
window1.height = 64;
window1.left = (screen_width() * .5) - (window1.width * .5);
window1.top = (screen_height() * .5) - (window1.height * .5);
window1.caption = "Window "..window1.surface;
window1.z = 9999;

text = textbox:new();
text.font = font2;
text.parent = window1;
text.top = window1.margin_top;
text.left = window1.margin_left;
text.text = "what a success";

button1 = button:new();
button1.font = font2;
button1.parent = window1;
button1.top = window1.margin_top;
button1.left = text.left + text.width + 6;
button1.caption = "Aye!";

button1.on_click = function()
    branch("apps/dead_simple.lua");
end

window1.visible = 1;
