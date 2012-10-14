 -- 

window1 = window:new();
window1.width = 320;
window1.height = 240;
--window1.top = 0;
--window1.left = 0;

window1.top = (screen_height() / 2) - (window1.height / 2);
window1.left = (screen_width() / 2) - (window1.width / 2);
--window1.on_draw = function () window1.gradient(makecol(190, 190, 190), makecol(128, 128, 128)); end
window1.z = 9998;

textbox1 = textbox:new();
textbox1.parent = window1;
textbox1.width = window1.width - 8;
textbox1.height = window1.height - 32 - 8;
textbox1.top = 32 + 4;
textbox1.left = 4;
textbox1.wordwrap = 0;
textbox1.text = openfile("text.txt");

textbox1.set_focus();