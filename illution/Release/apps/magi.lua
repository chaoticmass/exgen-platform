
font1 = surface:new();
font1.visible = 0;
font1.bmp = "fonts/black_small_computer.bmp";

font2 = surface:new();
font2.visible = 0;
font2.bmp = "fonts/black_micro_computer.bmp";

desktop = surface:new();
desktop.width = screen_width();
desktop.height = screen_height();

frame = window:new();
frame.height = screen_height() / 3;
frame.width = screen_width() / 3;
frame.no_titlebar = 1;
frame.close.noevents = 1;
frame.vscroll.noevents = 1;
frame.close.visible = 0;
frame.visible = 1;

render_loop(frame.surface);
