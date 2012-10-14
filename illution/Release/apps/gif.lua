
font1 = surface:new();
font1.visible = 0;
font1.bmp = "fonts/black_small_computer.bmp";

font2 = surface:new();
font2.visible = 0;
font2.bmp = "fonts/black_micro_computer.bmp";


window1 = window:new();
window1.bare = 1;
window1.backcolor = makecol(255, 255, 255);
window1.font = font2;
window1.width = screen_width();
window1.height = screen_height() - 32;
window1.caption = "Destrado";
window1.visible = 1;

window1.top = 0;
window1.left = 0;
window1.z = 9999;
window1.no_titlebar = 1;

window1.z = 9999;

image = surface:new();
image.parent = window1;
image.transcolor = makecol(0, 0, 0);
image.transparent = 1;
image.gif = "images/gif/main_document1.gif";
--window1.height = image.height / 2; --+ window1.margin_top + window1.margin_bottom;
image.top = window1.align_top(image);
image.left = window1.align_center(image);


--render_loop(window1.surface);
