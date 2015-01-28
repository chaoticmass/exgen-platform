
initialize_video(1, 1024, 768);
include("lib/render3d.lua");

win = window:new();
win.width = screen_width();
win.height = screen_height();
win.visible = 1;
render_loop();