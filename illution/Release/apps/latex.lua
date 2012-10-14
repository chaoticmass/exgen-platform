include("lib/latex.lua");

font1 = surface:new();
font1.visible = 0;
font1.bmp = "fonts/black_small_computer.bmp";

font2 = surface:new();
font2.visible = 0;
font2.bmp = "fonts/black_micro_computer.bmp";


window1 = window:new();
window1.no_titlebar = 1;
window1.bare = 1;
window1.backcolor = makecol(255, 255, 255);
window1.font = font2;
window1.width = screen_width();
window1.height = screen_height() - 32;
window1.caption = "latex";
window1.visible = 1;

window1.top = 0;
window1.left = 0;
window1.z = 9999;


window1.z = 9999;
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

\section*{Features}

\begin{description}
\item[2D Graphics] illution can render 2D graphics with special effects such
as alpha blending, transparency, rotation and resizing.

\item[3D Graphics] illution can render 3D textured polygons and also supports
gourad shading. A 3DS model loaded is also built-in.

\item[Physics Engine] a simulation grade physics engine is provided.
\end{description}

]]
tex.source = tex.source .. openfile('apps/lorem.tex');
tex.source = tex.source .. [[

\end{document}
]]

tex.output_file = 'system/tmp/boo.gif';
--echo(tex.source);

result = tex.post();

image = surface:new();
image.parent = window1;
image.gif = tex.output_file;
image.top = window1.align_top(image);
image.left = window1.align_left(image);

button1 = button:new();
button1.parent = window1;
button1.top = window1.align_top(button1);
button1.left = window1.align_left(button1);


render_loop(window1.surface);
