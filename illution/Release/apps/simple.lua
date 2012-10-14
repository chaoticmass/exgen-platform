
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
window1.caption = "exterm";
window1.visible = 1;

window1.top = 0;
window1.left = 0;
window1.z = 9999;

term = terminal:new();
term.parent = window1;
term.font = font1;
--term.text = "Hey";
term.top = 32;
term.left = 6;
term.backcolor = makecol(255, 255, 255);
term.width = window1.width - 12;
term.height = window1.height - term.top - 6;

window1.z = 9999;
window1.on_resize = function ()
    term.width = window1.width - 12;
    term.height = window1.height - term.top - 6;
end

window1.on_click = function ()
    term.set_focus();
end
term.set_focus();

--[[term.before_keypress = function(keycode, keyval)
    term.write(" keycode\n");
    return keycode, keyval;
}]]

-- end setup code--------------------

term.forkpty("/bin/bash");
render_loop(window1.surface);
