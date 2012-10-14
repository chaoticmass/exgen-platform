
font1 = surface:new();
font1.visible = 0;
font1.bmp = "fonts/small_tahoma.bmp";

window1 = surface:new();
window1.top = 0;
window1.left = 0;
window1.width = screen_width();
window1.height = screen_height();
window1.backcolor = makecol(0, 0, 0);
window1.forecolor = makecol(255, 255, 255);
--window1.font = font1;
window1.print(0, 0, "Slideshow");
window1.visible = 1;
window1.z = 9999;

orig = surface:new();
orig.visible = 0;

slide = surface:new();
slide.parent = window1;
slide.top = 0;
slide.left = 0;
slide.width = window1.width;
slide.height = window1.height;

dir = surface:new();
dir.parent = window1;
dir.top = 0;
dir.left = 0;
dir.width = window1.width;
dir.height = window1.height;
dir.visible = 0;

path = '/root/.pr0n/';
files = {};
file_count = 0;

for i in io.popen("ls "..path):lines() do
    if string.find(i,"%.jpg$") then 
	table.insert(files, path..i);
	file_count = file_count + 1;
    end
end

index = 1;


slide.loadjpeg = function (filename) 
    if (file_exists(filename..".tmb")) then
	slide.loadjpeg(filename..".tmb");
	render();
    end
    echo("loading "..filename.."\n");
    os.execute("djpeg -bmp "..filename.." > /tmp/slide.bmp");
    orig.bmp = "/tmp/slide.bmp";
    
    x_ratio = slide.width / orig.width;
    y_ratio = slide.height / orig.height;
    
    if ((x_ratio * orig.height) < slide.height) then
	dest_height = math.ceil(x_ratio * orig.height);
	dest_width = slide.width;
    else 
	dest_width = math.ceil(y_ratio * orig.width);
	dest_height = slide.height;
    end
    x = (slide.width * 0.5) - (dest_width * 0.5);
    y = (slide.height * 0.5) - (dest_height * 0.5);
    slide.clear();
    slide.set_dirty();
    surface_stretch_blit(orig.surface, slide.surface, 	
					0, 0, orig.width, orig.height,
					x, y, dest_width, dest_height);
					
end

slide.on_got_focus = function ()
    slide.visible = 1;
end

slide.on_lost_focus = function ()
    slide.visible = 0;
end

slide.on_keypress = function (keycode, keyval)
    echo(keycode.."\n");
    
    -- 83 key right
    if (keycode == 83) then
	index = index + 1;
	slide.loadjpeg(files[index]);
    elseif (keycode == 82) then
	index = index - 1;
	if (index < 1) then
	    index = file_count;
	end
	slide.loadjpeg(files[index]);	
    end
end





slide.loadjpeg(files[index]);

slide.set_focus();

render_loop(window1);
