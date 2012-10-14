
font1 = surface:new();
font1.visible = 0;
font1.bmp = "fonts/black_small_computer.bmp";

font2 = surface:new();
font2.visible = 0;
font2.bmp = "fonts/black_micro_computer.bmp";


anim_window = window:new();
anim_window.font = font2;
anim_window.width = 320;
anim_window.height = 240;
anim_window.caption = "Animated GIF Example";
anim_window.visible = 1;

anim_window.top = 0;
anim_window.left = 0;
anim_window.z = 9999;

anim_window.z = 9999;
anim_window.on_resize = function ()
end

anim_window.on_click = function ()
    term.set_focus();
end


image1 = surface:new();
image1.parent = anim_window;
image1.top = anim_window.align_top(image1);
image1.left = 10;
image1.animated_gif = "images/kvsb.gif";
image1.visible = 1;
anim_window.width = image1.width + 20;
anim_window.height = image1.height + anim_window.margin_top + anim_window.margin_bottom+10;
image1.set_dirty();


-- end setup code--------------------

--render_loop(anim_window.surface);
