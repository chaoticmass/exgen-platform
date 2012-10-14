include("lib/latex.lua");
include("lib/db.lua");

font1 = surface:new();
font1.visible = 0;
font1.bmp = "fonts/black_small_computer.bmp";

font2 = surface:new();
font2.visible = 0;
font2.bmp = "fonts/black_micro_computer.bmp";


window1 = window:new();
window1.close.visible = 0;
window1.no_titlebar = 1;
window1.bare = 1;
window1.backcolor = makecol(255, 255, 255);
window1.font = font2;
window1.width = screen_width();
window1.height = screen_height();
window1.caption = "latex";
window1.visible = 1;
window1.draggable = 0;

window1.top = 0;
window1.left = 0;
window1.z = 9999;


window1.z = 9999;

text1 = inputbox:new();
text1.font = font1;
text1.parent = window1;
text1.width = window1.inside_width - 64;
text1.top = window1.align_top(text1);
text1.left = window1.align_center(text1);
text1.text = 'illution';
text1.can_have_focus = 1;
text1.position_absolute = 0;


-- end setup code--------------------
--\usepackage[pdftex,dvips,papersize={]]..'320mm'..','..'240mm'..[[}]{geometry}
tex = latex_interface:new();
tex.image_size = 'bbox';
tex.source = [[
\documentclass[letterpaper]{report}

\usepackage[T1]{fontenc}
\usepackage{yfonts}
\usepackage{duerer}
\usepackage{fix-cm}
\begin{document}
\thispagestyle{empty}
\usefont{T1}{pag}{m}{n}
\Huge illution
\normalsize

\emph{illution} is a graphical user interface system.

]].. timer() ..[[
\normalfont
\section*{MAGI System}

MAGI System is a general interface system. It uses \emph{forms} and 
\emph{elements} for user interaction.

Use LaTeX to render display elements. Using custom LaTeX macros, return display
element coordinates which illution can use to know where in an image an element
is located. The user can then tab through the display elements and a border
box or some other graphical cue can indicate which element has focus. The user
can then invoke the interactive representation of the element, rendered by 
the MAGI client of choice, in this case illution.

Furthermore, the user can potentially add or edit elements to a form.

]]
tex.source = tex.source .. openfile('apps/lorem.tex');
tex.source = tex.source .. openfile('apps/lorem.tex');
tex.source = tex.source .. [[

\end{document}
]]

tex.output_path = 'system/tmp/';
--echo(tex.source);

--result = tex.post();




tex.create_document();
tex.download_page();
tex.load_page();
tex.destroy_document();
for i=1,tex.page_count,1 do
    tex.images[i].parent = window1;
    tex.images[i].top = window1.align_top(tex.images[i]);
    tex.images[i].left = window1.margin_left;
    tex.images[i].visible = 1;
    echo("Image "..i.." top = "..tex.images[i].top..", left = "..tex.images[i].left.."\n");
end


button1 = button:new();
button1.parent = window1;
button1.top = window1.align_top(button1);
button1.left = 0; --window1.align_left(button1);
button1.z = 999;



render_loop(window1.surface);
