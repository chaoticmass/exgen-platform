


intro1 = surface:new();
intro1.font = title_font;
intro1.width = screen_width();
intro1.height = screen_height();
intro1.backcolor = makecol(255, 255, 255);
intro1.clear();
intro1.z = 9999;

image1 = surface:new();
image1.gif = "glowerk/backgrounds/exgen_logo.gif";
image1.parent = intro1;
image1.top = (screen_height() * 0.5) - (image1.height * 1);
image1.left = (screen_width() * 0.5) - (image1.width * 0.5);

intro1.print(image1.left, image1.top + image1.height, "exgen version "..getvar("configuration", "version"));


intro2 = surface:new();
intro2.font = title_font;
intro2.width = screen_width();
intro2.height = screen_height();
intro2.backcolor = makecol(255, 255, 255);
intro2.clear();
intro2.z = 9999;

image2 = surface:new();
image2.gif = "glowerk/backgrounds/illution_logo.gif";
image2.parent = intro2;
image2.top = (screen_height() * 0.5) - (image2.height * 1);
image2.left = (screen_width() * 0.5) - (image2.width * 0.5);
intro2.print(image2.left, image2.top + image2.height, "illution version "..getvar("configuration", "illution_version"));




intro1.z = intro2.z + 1;
for i = 0, 255, 4 do
    render();
end

include("glowerk/exterm.lua");
term_window.left = (screen_width() * 0.5) - (term_window.width * 0.5);
term_window.top = image2.top + image2.height + (image2.text_height()*2);
term.write("");
--render_loop();
--term:after_keypress(51);

--term.write("\n-- Starting illution...");

for i = 0, 255, 8 do
    intro1.alpha = i;
    render();
end
intro1.visible = 0;

intro_fadeout = function()
    intro1.visible = 0;
    intro2.z = 9999;
--    term_window.visible = 0;
    for i = 0, 255, 16 do
	intro2.alpha = i;
	render();
    end
    
    intro2.visible = 0;
--    intro1.destroy();
--    intro2.destroy();
end

render();
