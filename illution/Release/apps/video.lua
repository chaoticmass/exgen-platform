
font1 = surface:new();
font1.visible = 0;
font1.bmp = "fonts/black_small_computer.bmp";

font2 = surface:new();
font2.visible = 0;
font2.bmp = "fonts/black_micro_computer.bmp";


window1 = window:new();
window1.font = font2;
window1.width = 320;
window1.height = 240;
window1.caption = "Video Test";
window1.visible = 1;

window1.top = 0;
window1.left = 0;
window1.z = 9999;

window1.z = 9999;
window1.on_resize = function ()
end

window1.on_click = function ()
    term.set_focus();
end


image1 = surface:new();
image1.parent = window1;
image1.top = window1.align_top(image1);
image1.left = 10;
image1.width = 320;
image1.height = 240;
--image1.animated_gif = "/root/yt/test.gif";
image1.visible = 1;
window1.width = image1.width + 20;
window1.height = image1.height + window1.margin_top + window1.margin_bottom+10;
image1.set_dirty();
image1.frame = 0;


-- end setup code--------------------
mod_load("video");
vindex = create_stream();
echo(stream_open_file(vindex, "/root/test.flv").."\n");
echo(stream_get_info(vindex).."\n");
echo(stream_open_video_codec(vindex, 0).."\n");
echo(stream_initialize_video_buffer(vindex).."\n");

image1.width = stream_get_width(vindex);
image1.height = stream_get_height(vindex);
window1.width = image1.width + 20;
window1.height = image1.height + window1.margin_top + window1.margin_bottom+10;


image1.on_draw = function ()
    image1.set_dirty();
    if retval == -1 then
	
	
	--image1.frame = image1.frame + 1;
        --window1.set_dirty();
    end
end


for i = 0, 100,1 do
    retval = stream_read_frame(vindex);
    stream_render_frame(vindex, get_surface_bitmap(image1.surface));
end    
image1.set_dirty();

--image1.on_draw();
--render_loop(window1.surface);
