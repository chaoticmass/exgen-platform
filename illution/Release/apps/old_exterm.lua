

function termcolor(color)
    if (color == 0) then
	retval = makecol(0, 0, 0);
    elseif (color == 1) then -- red
    	retval = makecol(255, 0, 0);
    elseif (color == 2) then -- green
    	retval = makecol(0, 255, 0);
    elseif (color == 3) then -- yellow
    	retval = makecol(0, 255, 255);
    elseif (color == 4) then -- blue
    	retval = makecol(0, 0, 255);
    elseif (color == 5) then -- magenta
    	retval = makecol(128, 0, 0);
    elseif (color == 6) then -- cyan
    	retval = makecol(0, 0, 128);
    elseif (color == 7) then -- white
    	retval = makecol(255, 255, 255);
    end
    return retval;
end

function drawterm()
    term.clear();
    set_thread_priority(100);
    
    ysize = 8;
    xpos = 0;
    ypos = 0;
    ccol = vt_cursor_col(vt);
    crow = vt_cursor_row(vt);
    
    white = makecol(255, 255, 255);
    for row = 0, rows - 1, 1 do
	for col = 0, cols - 1, 1 do
	    char = vt_char(vt, row, col);
	    xsize = 8; --text_length(myfont, char);
	    fg = termcolor(vt_fgcolor(vt, row, col));
	    bg = termcolor(vt_bgcolor(vt, row, col));

	    if (crow == row) then
		if (ccol == col) then
		    fg, bg = bg, fg;		    
		end
	    end
	    term.backcolor = bg;
	    term.forecolor = fg;
	    term.print(xpos, ypos, char);
	    xpos = xpos + xsize;
	end
	xpos = 0;
	ypos = ypos + ysize;
    end
    
    vt_clean(vt);
    set_thread_priority(getvar("configuration", "default-thread-priority"));
end 

function keyhandler()
    key, scan = readkeyscan();
    if (key == 0) then
	vt_keypress(vt, vt_convscan(scan));
    else
	vt_keypress(vt, key);
    end
    if (key == 27) then
	quit();
    end 
end

rows = 25;
cols = 80;

vt = vt_create(rows, cols);
vt_forkpty(vt, "/bin/bash");


window1 = window:new();
window1.width = 640;
window1.height = 320;
window1.top = 0;
window1.left = 0;
window1.visible = 1;
window1.on_mouse_click = function ()
    term.set_focus();
end

term = surface:new();
term.parent = window1;
term.top = 32 + 4;
term.left = 4;
term.width = window1.width - 8;
term.height = window1.height - 32 - 8;
term.on_draw = function ()
    vt_update(vt);
    if (vt_dirty(vt)) then
	drawterm();
	echo("draw\n");
    end    
    term.set_dirty();
end


term.set_focus();

term.on_keypress = function (keycode, keyval)
    if (keyval == 0) then
	vt_keypress(vt, vt_convscan(keycode));
    else
	vt_keypress(vt, keyval);
    end    
--    term.on_draw();
    term.set_dirty();
end


render_loop(window1.surface);