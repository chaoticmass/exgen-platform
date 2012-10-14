
font1 = surface:new();
font1.visible = 0;
font1.bmp = "fonts/small.bmp";


window1 = window:new();
window1.font = font1;
window1.width = screen_width();
window1.height = screen_height() - 32;
window1.caption = "Notepad";
window1.visible = 1;

window1.top = 0;
window1.left = 0;
window1.z = 9999;


text1 = multi_line_inputbox:new();
text1.parent = window1;
text1.font = font1;
text1.text = "Hey";
text1.top = 32;
text1.left = 6;
text1.backcolor = makecol(255, 255, 255);
text1.width = window1.width - 12;
text1.height = window1.height - text1.top - 6;

window1.z = 9999;
window1.on_resize = function ()
    text1.width = window1.width - 12;
    text1.height = window1.height - text1.top - 6;
end

echo("LUA: render_loop(".. window1.surface ..")\n");
render_loop(window1.surface);
echo("whoops\n");