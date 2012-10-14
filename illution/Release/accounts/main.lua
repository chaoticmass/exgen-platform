font1 = surface:new();
font1.visible = 0;
font1.bmp = "fonts/black_small_computer.bmp";

main = window:new();
main.font = font1;
main.top = 0;
main.left = 0;
main.width = screen_width();
main.height = 64;
main.draggable = 0;
main.no_titlebar = 1;
main.visible = 1;
echo("Main window done.\n");