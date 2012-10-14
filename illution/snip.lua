
term.before_keypress = function(keycode, keyval)
    echo("Keyval: "..keyval..", Keycode: "..keycode.."\n");
--    term_window.caption = "Keyval: "..keyval..", Keycode: "..keycode;
    if keycode == 80 then -- pageup
	term.buffer[term.page] = term.readterm();
	term.page = term.page - 1;
	echo(term.page.."\n");
	term.clearterm();
	echo(type(term.buffer[page]).."\n");
	if (type(term.buffer[page]) == "string") then
	    term.write(term.buffer[page]);
	end
    elseif keycode == 81 then -- pagedown
	term.buffer[term.page] = term.readterm();
	term.page = term.page + 1;
	echo(term.page.."\n");
	term.clearterm();
	if (type(term.buffer[page]) == "string") then
	    term.write(term.buffer[page]);
	end
	
    elseif keyval == 13 then -- Enter
	this_line = term.read_line();
	left = string.sub(this_line, 0, term.cursor_col() - 1);
	right = string.sub(this_line, term.cursor_col() + 1);
	term.write(string.rep(string.char(32), string.len(right)));
	term.write("\n");
	term.write(right);
	term.write(string.rep(string.char(8), string.len(right)));
    elseif keycode == 78 then -- home
	
    elseif keycode == 79 then -- end
    elseif keyval == 9 then -- Tab
	tab_length = 4;
	keycode = 0;
	keyval = 0;
	this_line = term.read_line();
	left = string.sub(this_line, 0, term.cursor_col() - 1);
	right = string.sub(this_line, term.cursor_col() + 1);
--	term.write(string.rep(string.char(32), string.len(right)));
	term.write(string.rep(" ", tab_length));
	echo("Right Len: "..string.len(right).."\n");
	if (string.len(right)+tab_length+term.cursor_col() > term.cols) then
	    old_col = term.cursor_col();
	    term.write(right);
	    term.keypress(84, 0);
	    i = 0;
	    for i = 1, (old_col - term.cursor_col()), 1 do
		term.keypress(83, 0);
	    end
	else
	    --term.write(string.rep(string.char(8), string.len(right)));
	end
    elseif keyval == 8 then -- Backspace
	term.write(string.char(8)..string.char(32));
    elseif keyval == 127 then -- Delete
	keycode = 0;
	keyval = 0;
	this_line = term.read_line();
	left = string.sub(this_line, 0, term.cursor_col() - 1);
	right = string.sub(this_line, term.cursor_col() + 2);
	term.write(right);
	term.write(string.rep(string.char(8), string.len(right)));
    elseif keyval ~= 0 then
	this_line = term.read_line();
	left = string.sub(this_line, 0, term.cursor_col() - 1);
	right = string.sub(this_line, term.cursor_col() + 1);
	term.write(string.char(keyval));
	if (string.len(right)+1+term.cursor_col() > term.cols) then
	    old_col = term.cursor_col();
	    term.write(right);
	    term.keypress(84, 0);
	    i = 0;
	    for i = 1, (old_col - term.cursor_col()), 1 do
		term.keypress(83, 0);
	    end
	else
	    --term.write(string.rep(string.char(8), string.len(right)));
	end
	keycode = 0;
	keyval = 0;
    end
    return keycode, keyval;
end
