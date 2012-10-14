
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


window1.z = 9999;


render_loop(window1.surface);
