
font1 = surface:new();
font1.visible = 0;
font1.bmp = "fonts/black_small_computer.bmp";

font2 = surface:new();
font2.visible = 0;
font2.bmp = "fonts/black_micro_computer.bmp";


window1 = window:new();
window1.font = font2;
window1.width = screen_width();
window1.height = screen_height() - 32;
--window1.no_titlebar = 1;
window1.caption = "exgen";
window1.visible = 1;

window1.top = 0;
window1.left = 0;
window1.z = 9999;

window1.z = 9999;
window1.on_resize = function ()
    term.width = window1.width - 12;
    term.height = window1.height - term.top - 6;
end

window1.on_click = function ()
    term.set_focus();
end

text1 = inputbox:new();
text1.parent = window1;
text1.top = window1.align_top(text1);
text1.left = window1.align_left(text1);
text1.font = font2;
text1.width = window1.inside_width;
text1.set_focus();

-- end setup code --------------------

render_loop(window1.surface);
