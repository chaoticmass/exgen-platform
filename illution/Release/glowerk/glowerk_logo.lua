ltex = latex_interface:new();
ltex.source = [[
\\documentclass{report}
\\usepackage{tikz}
\\usepackage{fancybox}
\\usepackage[pdftex,dvips,papersize={8.5in,11in},vmargin={0cm,0cm},hmargin={0cm,0cm}]{geometry}
\\usepackage[T1]{fontenc}
\\pdfpagewidth 11in
\\pdfpageheight 8.5in
\\pagestyle{empty}
\\begin{document}
\\begin{center}
    \\fontsize{115mm}{115mm}\\usefont{T1}{pag}{b}{n}GLO \\newline
    \\fontsize{95mm}{95mm}\\usefont{T1}{pag}{m}{n}WERK \\newline
\\end{center}
\\end{document}
]]


ltex.output_format = 'gif';
ltex.create_document();
ltex.download_page();
logo.gif = ltex.pages[1];
--savefile("glowerk/backgrounds/glowerk_logo.gif", openfile(ltex.pages[1]));
ltex.destroy_document();
