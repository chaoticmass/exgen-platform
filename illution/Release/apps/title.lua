include("lib/latex.lua");

tex = latex_interface:new();
tex.hostname = '127.0.0.1';
tex.image_size = 'tight';
tex.output_format = 'jpg';

overlay = {}
function overlay:new(title, subtitle, width, height)
--[[    local object = {};
    local daters = {};

    setmetatable(object, {
	-- GET DATA
	__index = function (table, key)
	    return daters[key];
	end,
	-- SET DATA
	__newindex = function(table, key, value)
	    daters[key] = value;
	end
    }
    };
]]

    local window2 = window.new();
    window2.bare = 1;
    local box2 = surface:new();
    local logo2 = surface:new();
    window2.top = 0;
    window2.visible = 1;
    i = 0;

    window2.update = function(title, subtitle)
	i = i + 1;
	tex.source = [[
\\documentclass[letterpaper]{report}
\\usepackage{graphicx}
\\usepackage[T1]{fontenc}
\\usepackage{yfonts}
\\usepackage{color}
\\usepackage[usenames,dvipsnames,svgnamesm,table]{xcolor}
\\usepackage{anyfontsize}
\\usepackage{t1enc}
\\usepackage[paperheight=]]..height..[[cm,paperwidth=]]..width..[[cm,margin=.01in]{geometry}
\\definecolor{dg}{rgb}{.1,.1,.1}
\\usepackage[parfill]{parskip}
\\begin{document}
    \\noindent 
    \\thispagestyle{empty}
    \\pagecolor{dg}
    \\usefont{T1}{bch}{m}{n}
    %\\begin{center}
    \\textcolor{White}{
	\\fontsize{30}{32}\\selectfont ]].. title ..[[
	\\fontsize{20}{22}\\selectfont ]]..subtitle..[[
    }
    %\\end{center}
    
    \\normalsize
\\end{document}
]]

        tex.create_document();
	tex.download_page();
        tex.destroy_document();
--	echo( tex.page_count.."\n");

        window2.no_titlebar = 1;
	window2.visible = 1;
        window2.after_mouse_up = function (button, x, y) 
	    if button == 2 then
		window2.bare = 1;
    	    else 
	        window2.bare = 0;
	    end
	end


	box2.parent = window2;
	box2.top = window2.margin_top;
	box2.left = window2.margin_left;
        box2.height = window2.height - window2.margin_top - window2.margin_bottom;
        box2.width = window2.width - window2.margin_left - window2.margin_right;
        box2.backcolor = makecol(0, 0, 0);
        box2.clear();

        box2.visible = 1;

        logo2.jpg = tex.pages[1] --"6079c6b9abb1fa0aa1e33d2434bbe213_1.jpg"
	box2.width = logo2.width;
        box2.height = logo2.height;
	logo2.parent = window2;
        logo2.top = window2.margin_top; --+ ((window1.height * .5) - (logo.height * .5));
	logo2.left = window2.margin_left; -- + ((window1.width * .5) - (logo.width * .5));
        logo2.z = 9999;
	logo2.visible = 1;
        logo2.on_mouse_up = function(b, x, y)
    	    if b == 1 then
		logo2.alpha = 0;
	    elseif b == 2 then
		box2.clear();
		logo2.alpha = 255;
	    end
	end

	window2.height = logo2.height + window2.margin_top + window2.margin_bottom + 16;
        window2.width = logo2.width + window2.margin_left + window2.margin_right;
    end

    window2.update(title, subtitle);
    

end

ov1 = overlay:new("Jacob Huss", "Inna-Houz", 25, 25);

ov2 = overlay:new([[\\noindent  \\hspace{-2.5mm}\\usefont{T1}{pag}{m}{n}old\\%26\\usefont{T1}{bch}{m}{n}busted
]], 
[[\\\\ \\noindent not interested in new hotness
]], 25, 25);



echo("Win?\n");