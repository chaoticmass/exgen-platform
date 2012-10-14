mouse = mouse_cursor:new();

desktop = surface:new();
desktop.top = 0;
desktop.left = 0;
desktop.width = screen_width();
desktop.height = screen_height();
desktop.z = -1;
--desktop.forecolor = getvar("desktop", "forecolor");
--desktop.backcolor = getvar("desktop", "backcolor");
--desktop.gradient(desktop.forecolor, desktop.backcolor);
desktop.backcolor = makecol(0, 0, 0);
desktop.clear();