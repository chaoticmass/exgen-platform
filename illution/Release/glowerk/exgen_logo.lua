ltex_font = "squarehead";
ltex = latex_interface:new();
ltex.source = [[
\\documentclass{report}
\\usepackage{tikz}
\\usepackage{fancybox}
\\usepackage[pdftex,dvips,papersize={8.5in,11in},vmargin={0cm,0cm},hmargin={0cm,0cm}]{geometry}
\\usepackage[T1]{fontenc}
\\font\\ttf]]..ltex_font..[[ ]]..ltex_font..[[ at 80pt 
\\pdfpagewidth 11in
\\pdfpageheight 8.5in
\\pagestyle{empty}
\\begin{document}
\\begin{center}
    \\ttf]]..ltex_font..[[ exgen
\\end{center}
\\end{document}
]]

ltex.fonts = ltex_font;
ltex.output_format = 'gif';
ltex.create_document();
ltex.download_page();
logo.gif = ltex.pages[1];
--savefile("glowerk/backgrounds/glowerk_logo.gif", openfile(ltex.pages[1]));
ltex.destroy_document();
