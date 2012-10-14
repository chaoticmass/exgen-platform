include("lib/render3d.lua");
include("lib/libode.lua");
include("lib/latex.lua");

set_window_title("Media Player");

title_font = surface:new();
title_font.visible = 0;
title_font.bmp = "fonts/black_small_monospace.bmp";
body_font = surface:new();
body_font.visible = 0;
body_font.bmp = "fonts/black_micro_light.bmp";


screen = surface:new(0);
mouse = mouse_cursor:new();

bg = surface:new();
bg.width = screen_width();
bg.height = screen_height();
--bg.gradient(getvar("colors", "widget-forecolor"), getvar("colors", "widget-backcolor"));
bg.gradient(getvar("desktop", "forecolor"), getvar("desktop", "backcolor"));
bg.jpg = "images/colors.jpg";
bg.top =  (screen_height() * .5) - (bg.height * .5);


tex = latex_interface:new();
--tex.fonts = 'squarehead';

tex.source_head = [[
\\documentclass[letterpaper]{article}
\\usepackage{tikz}
\\usepackage{fancybox}
%\\usepackage[pdftex,dvips,papersize={16.2cm,8.8cm},vmargin={1cm,1cm},hmargin={1cm,1cm}]{geometry}
%\\usepackage[T1]{fontenc}
%\\font\\ttffelixlarge squarehead at88pt
%\\font\\ttffelixmedium squarehead at44pt
%\\font\\ttfsquareheadlarge squarehead at88pt
%\\font\\ttffsquareheadmedium squarehead at44pt
\\pagestyle{empty}
%\\pdfpagewidth 16.2cm
%\\pdfpageheight 8.8cm
\\usepackage{color}
\\definecolor{background}{rgb}{0.5, 0.5 , 0.5}
\\definecolor{foreground}{rgb}{1, 1 , 1}
\\pagecolor{background}
\\color{foreground}
\\renewcommand{\\familydefault}{\\sfdefault} 
\\begin{document}
\\noindent
]]

tex.source_foot = [[
\\end{document}
]]


print_window = window:new();
print_window.bare = 1;
print_window.backcolor = makecol(255, 0, 255);

print_window.height = 34 + print_window.margin_top + print_window.margin_bottom;
print_window.width = 63 + print_window.margin_left + print_window.margin_right;

print_window.no_titlebar = 1;
print_window.close.visible = 0;
print_window.transparent = 0;
print_window.alpha = 255;

print_output = surface:new();
print_output.parent = print_window;
print_output.top = print_window.margin_top;
print_output.left = print_window.margin_left;
bg.slide_left = 0;


bg.on_draw = function() 
    amount = 1;
    if (bg.slide_left > 0) then
	bg.left = bg.left + amount;
	print_window.left = print_window.left + (amount*2);
	bg.slide_left = bg.slide_left - amount;
	
    end
    if (bg.slide_left < 0) then
	bg.left = bg.left - amount;
	print_window.left = print_window.left - (amount * 2);
	bg.slide_left = bg.slide_left + amount;
	
    end
end

current_screen = 1;
screen_count = 3;
slide = function(screen) 
    if (screen > current_screen) then
	--bg.slide_left = 0 - ((bg.width - screen_width() / 2);
	current_screen = screen;
    end
    bg.set_dirty();
    echo("Slide left "..screen.."! \n");
end

console_source = '';
current_page = 0;
tex.page_count = 0;
console_write = function(latex) 

    console_source = console_source .. latex;
	old_source = [[
%\\fbox{
\\begin{minipage}[t][8.5cm]{15.75cm}
%\\begin{flushleft}
    
    \\usefont{T1}{pag}{m}{n} 
    ]]..console_source..[[
%\\end{flushleft}
\\end{minipage}
%}
]];
--\\usefont{T1}{pag}{m}{n} 
	source = [[
    ]]..console_source..[[
]];
	
	tex.source = tex.source_head..source..tex.source_foot;
	tex.output_format = 'gif';
	tex.create_document();
	tex.download_page();
	echo("Console Write!\n");
	
	if (tex.page_count ~= tex.last_page_count) then
	    current_page = tex.page_count;
	end
	print_output.gif = tex.pages[current_page];
	tex.last_page_count = tex.page_count;
	tex.destroy_document();
	print_window.old_width = print_window.width;
	print_window.old_height = print_window.height;
	print_window.width = print_output.width + print_window.margin_left + print_window.margin_right;
	print_window.height = print_output.height + print_window.margin_top + print_window.margin_bottom;
	print_window.top = screen_height() - print_window.old_height - 64;
	print_window.left = screen_width() - print_window.old_width;

	--print_window.top = (screen_height() / 2) - (print_window.height / 2);
	--print_window.left = (screen_width() / 2) - (print_window.width / 2);
end



global_on_keypress = function(keycode, keyval)
    echo("Keycode: "..keycode..", Keyval: "..keyval.."\n");

    if (keycode == 58) then
	shutdown();
    end
    
    -- F5
    if (keycode == 51) then
	echo(term.readterm());
	console_write(term.readterm());
	term.clearterm();
    end
    if (keycode == 52) then
	source = term.readterm();
	term.clearterm();
	--include_string(source);
	savefile("script.lua", source);
	branch("script.lua");
    end
    if (keycode == 54) then
	console_source = "";
	console_write([[
	\\color{foreground}
	{\\ttffelixlarge \\noindent exgen }\\newline
	\\normalfont
	
	]]);
	term.clearterm();
    end
    --slide(current_screen + 1);
	
end


set_global_on_keypress(global_on_keypress);

function round(num, idp)
  local mult = 10^(idp or 0)
  return math.floor(num * mult + 0.5) / mult
end


--print_window.alpha = 32;
--print_window.rotation = 34
--print_output.rotation = 34;
--print_window.top = screen_height();
print_window.old_height = print_window.height;
print_window.old_width = print_window.width;
print_window.top = screen_height() - print_window.old_height - 64;
print_window.left = screen_width() - print_window.old_width;

print_window.on_redraw = function()
    if (print_window.alpha > 1) then
	print_window.alpha = print_window.alpha - 1;
    end
    if (print_window.old_height < print_window.height) then
	print_window.old_height = print_window.old_height + 1;
    	print_window.top = screen_height() - print_window.old_height - 64;
    end
    if (print_window.old_width < print_window.width) then
	print_window.old_width = print_window.old_width + 1;
	print_window.left = screen_width() - print_window.old_width;
    end

    if (print_window.old_height > print_window.height) then
	print_window.old_height = print_window.old_height - 1;
    	print_window.top = screen_height() - print_window.old_height - 64;
    end
    if (print_window.old_width > print_window.width) then
	print_window.old_width = print_window.old_width - 1;
	print_window.left = screen_width() - print_window.old_width;
    end

    if (tex.page_count == 0) then
	console_write([[	
	\\color{foreground}
	\\normalfont
	Hey
	]]);
	
    end
end


include('media/exterm.lua');


render_loop();