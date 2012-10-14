
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
window1.caption = "exterm";
window1.visible = 1;

window1.top = 0;
window1.left = 0;
window1.z = 9999;

window1.z = 9999;
window1.on_resize = function ()
end

window1.on_click = function ()
    term.set_focus();
end

window1.on_close = function ()
    window1.destroy();
end

text1 = inputbox:new();
text1.parent = window1;
text1.top = window1.align_top(text1);
text1.left = window1.align_center(text1);
text1.font = font2;
text1.height = text1.height + 3;
text1.text = memory_usage();

button1 = button:new();
button1.parent = window1;
button1.top = window1.align_top(button1);
button1.left = window1.align_center(button1);

button1.on_click = function()
    text1.text = memory_usage();
end
--[[
text2 = multi_line_inputbox:new();
text2.parent = window1;
text2.top = window1.align_top(text2);
text2.left = window1.align_center(text2);
text2.font = font2;
]]

-- end setup code--------------------

render_loop(window1.surface);
