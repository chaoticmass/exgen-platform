terminal = {};
function terminal:new(cols, rows)
    local object = {};
    local daters = {};

    daters.surface = surface:new();
    daters.surface.rows = rows;
    daters.surface.cols = cols;

    echo("ROWS (Y): "..daters.surface.rows.."\n");
    echo("COLS (X): "..daters.surface.cols.."\n");
    local vt = vt_create(daters.surface.rows, daters.surface.cols);

    ypos = 0;
    daters.surface.vt = vt;
    daters.surface.buffer = {};
    daters.surface.buffer[0] = "";
    daters.surface.buffer[1] = "";
    
    daters.surface.page = 0;
    daters.surface.page_count = 1;
    daters.surface.row = 0;
    daters.surface.col = 0;
    daters.surface.v_row = 0;
    daters.surface.v_col = 1;
    daters.surface.buffer = "";

    vt_update(vt);
    daters.surface.pid = 0;

    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "self" then
		--return daters;
	    else
		return daters.surface[key];
	    end
	end,
	-- SET DATA
	__newindex = function (table, key, value)
--[[   	    if key == "cursor_row" then
		return vt_cursor_row(vt);
	    elseif key == "cursor_col" then
		vt_cursor_col(vt);
	    else]]
		daters.surface[key] = value;
--	    end
	end	
    }
    );


    object.cursor_row = function ()
	return vt_cursor_row(vt);
    end
    object.cursor_col = function ()
	return vt_cursor_col(vt);
    end

    daters.surface.termcolor = function (color)
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

    daters.surface.drawterm = function ()
        daters.surface.clear();
	
	set_thread_priority(100);
    
	xpos = 0;
	ypos = 0;
	
	vt = daters.surface.vt;
	term = daters.surface;
	rows = daters.surface.rows;
	cols = daters.surface.cols;
--	termcolor = term.termcolor;
        ccol = vt_cursor_col(vt);
	crow = vt_cursor_row(vt);
	ysize = daters.surface.text_height();
	xsize = 8;
--	daters.surface.orig_bg = daters.surface.backcolor;
--	daters.surface.orig_fg = daters.surface.forecolor;
	
	this_line = "";    
        white = makecol(255, 255, 255);
	for row = 0, rows - 1, 1 do
	    for col = 0, cols - 1, 1 do
		char = vt_char(vt, row, col);
		xsize = daters.surface.text_length(char);
		this_line = this_line .. char;
		if (crow == row) then
		    if (ccol == col) then
			daters.surface.print(xpos, ypos, "_");
		    end
		end
		xpos = xpos + xsize;
	    end
	    xpos = 0;
	    daters.surface.print(xpos, ypos, this_line);
	    this_line = "";
	    
	    ypos = ypos + ysize;
	end
    
	vt_clean(vt);
--	daters.surface.backcolor = daters.surface.orig_bg;
--	daters.surface.forecolor = daters.surface.orig_fg;
	
	set_thread_priority(getvar("configuration", "default-thread-priority"));
    end 


    daters.surface.readterm = function ()
	set_thread_priority(100);
	vt = daters.surface.vt;
	term = daters.surface;
	rows = daters.surface.rows;
	cols = daters.surface.cols;
        ccol = vt_cursor_col(vt);
	crow = vt_cursor_row(vt);
	ysize = daters.surface.text_height();
	xsize = 8;
	xpos = 0;
	this_line = "";    
	text = "";
	for row = 0, rows - 1, 1 do
	    for col = 0, cols - 1, 1 do
		char = vt_char(vt, row, col);
		xsize = daters.surface.text_length(char);
		this_line = this_line .. char;
		if (crow == row) then
		    if (ccol == col) then
		    end
		end
		xpos = xpos + xsize;
	    end
	    xpos = 0;
	    if (this_line ~= "") then
		text = text .."\n".. this_line;
	    end
	    this_line = "";
	    
	    ypos = ypos + ysize;
	end
    
	vt_clean(vt);
--	daters.surface.backcolor = daters.surface.orig_bg;
--	daters.surface.forecolor = daters.surface.orig_fg;
	
	set_thread_priority(getvar("configuration", "default-thread-priority"));
	return text;
    end 




--[[    daters.surface.old_drawterm = function ()
        daters.surface.clear();
	
	set_thread_priority(100);
    
	xpos = 0;
	ypos = 0;
	
	vt = daters.surface.vt;
	term = daters.surface;
	rows = term.rows;
	cols = term.cols;
	termcolor = term.termcolor;
        ccol = vt_cursor_col(vt);
	crow = vt_cursor_row(vt);
	ysize = term.text_height();
	xsize = 8;
	term.orig_bg = term.backcolor;
	term.orig_fg = term.forecolor;
	    
        white = makecol(255, 255, 255);
	for row = 0, rows - 1, 1 do
	    for col = 0, cols - 1, 1 do
		char = vt_char(vt, row, col);
		xsize = term.text_length(char);
		fg = 0; --termcolor(vt_fgcolor(vt, row, col));
		bg = 0; --termcolor(vt_bgcolor(vt, row, col));

		if (crow == row) then
		    if (ccol == col) then
			fg, bg = bg, fg;
			term.print(xpos, ypos, "_");
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
	term.backcolor = term.orig_bg;
	term.forecolor = term.orig_fg;
	
	set_thread_priority(getvar("configuration", "default-thread-priority"));
    end 
]]
    daters.surface.keyhandler = function ()
	--[[key, scan = readkeyscan();
        if (key == 0) then
	    vt_keypress(vt, vt_convscan(scan));
	else
	    vt_keypress(vt, key);
	end
	if (key == 57) then
	    quit();
	end ]]
	
    end


    daters.surface.on_draw = function ()
	--vt_update(daters.surface.vt);
        if (vt_dirty(daters.surface.vt)) then
    	    daters.surface.drawterm();
	    --echo("draw\n");
	    if (type(daters.surface.on_vt_dirty) == 'function') then
		daters.surface.on_vt_dirty();
	    end
	end    
	daters.surface.set_dirty();
    end

    object.keypress = function (keycode, keyval)
	if (keyval == 0) then
	    vt_keypress(object.vt, vt_convscan(keycode));
	else
	    vt_keypress(object.vt, keyval);
	end    
    end

    object.on_keypress = function (keycode, keyval)
	echo(object.surface.." KEYPRESSED\n");
	if (type(object.before_keypress) == 'function') then
	    keycode, keyval = object:before_keypress(keycode, keyval);
	end
	--else 
	    if (keyval == 0) then
		vt_keypress(vt, vt_convscan(keycode));
	    else
		vt_keypress(vt, keyval);
	    end    
	--end
        object.on_draw();
	object.set_dirty();
	if (type(object.after_keypress) == 'function') then
	    object:after_keypress(keycode, keyval);
	end
    end    

    daters.surface.write = function (string)
	vt_write(daters.surface.vt, string);
	daters.surface.set_dirty();
    end
    
    daters.surface.read_line = function ()
        ccol = vt_cursor_col(vt);
	crow = vt_cursor_row(vt);
	this_line = "";
	for col = 0, cols - 1, 1 do
	    char = vt_char(vt, crow, col);
	    this_line = this_line .. char;
        end
        return this_line;
    end
    
    daters.surface.forkpty = function (cmd) 
	daters.surface.pid = vt_forkpty(daters.surface.vt, cmd);
	--vt_forsake_child(daters.surface.vt);
	daters.surface.on_draw();
	daters.surface.set_dirty();
    end
    
    daters.surface.child_alive = function()
	if (vt_child_alive(daters.surface.pid) == 0) then
	    return 1;
	else 
	    return 0;
	end
    end

    function object:home_cursor ()
	self.write(string.char(27).."[{1};{1}H");
    end

    function object:read_buffer ()
	return self.buffer;
    end

    function object:write_buffer (b)
	self.buffer = b;
	self:render_view();
    end


    function object:move_cursor (row, col)
	echo("MOVING TO ROW: "..row.." COL: "..col.."\n");
	echo("Viewing: "..self.v_row.." COL: "..self.v_col.."\n");
	vt_inject(self.vt, string.char(27).."[{"..row.."};{"..col.."}H");
	vt_update(self.vt);
	for i = 1, row, 1 do
	    vt_inject(self.vt, string.char(27).."[1B");
	    vt_update(self.vt);
	end
	for i = 1, col, 1 do
	    vt_inject(self.vt, string.char(27).."[1C");
	    vt_update(self.vt);
	end
	self.set_dirty();
	self.row = row;
	self.col = col;
    end

    function object:clear_term ()
	vt_inject(self.vt, string.char(27).."[2J ");
--	vt_write(self.vt, string.char(27).."[{1};{1}H");
	vt_update(self.vt);
	self.set_dirty()
    end 


    function object:before_keypress (keycode, keyval)
	echo("Keycode: "..keycode..", Keyval: "..keyval.."\n");
	if keycode == 24 then
	    self:move_cursor(5, 5);
	end
	if keycode == 80 then -- pageup
	    o_col = vt_cursor_col(vt);
	    o_row = vt_cursor_row(vt);
	    self.v_row = self.v_row - rows;
	    if self.v_row < 0 then
		self.v_row = 0;
	    end
	    self:render_view();
	    self:move_cursor(o_row, o_col);

	    keycode = 0;
	    keyval = 0;

	    keycode = 0;
	    keyval = 0;
	elseif keycode == 81 then -- pagedown
	    o_col = vt_cursor_col(vt);
	    o_row = vt_cursor_row(vt);
	    self.v_row = self.v_row + rows;
	    self:render_view();
	    self:move_cursor(o_row, o_col);

	    keycode = 0;
	    keyval = 0;
	elseif keycode == 85 then -- down
	    echo(vt_cursor_row(vt).."\n");
	    o_col = vt_cursor_col(vt);
	    o_row = vt_cursor_row(vt);
	    if vt_cursor_row(vt) == rows -2 then
		self.v_row = self.v_row + 1;
		self:render_view();
		self:move_cursor(o_row-1, o_col);
	    end

	elseif keycode == 84 then -- up
	    echo(vt_cursor_row(vt).."\n");
	    o_col = vt_cursor_col(vt);
	    o_row = vt_cursor_row(vt);
	    if vt_cursor_row(vt) == 0 then
		if (self.v_row > 0) then
		    self.v_row = self.v_row - 1;
		    self:render_view();
		    self:move_cursor(o_row, o_col);
		end
	    end
	elseif keycode == 83 then -- right
	    echo(vt_cursor_col(vt).."\n");
	    o_col = vt_cursor_col(vt);
	    o_row = vt_cursor_row(vt);
	    echo("CURRENT ROW: "..o_row.." COL: "..o_col.."\n");
	    if vt_cursor_col(vt) == cols -1 then
		self.v_col = self.v_col + 1;
		self:render_view();
		self:move_cursor(o_row, o_col-1);
	    end
	elseif keycode == 82 then -- left
	    o_col = vt_cursor_col(vt);
	    o_row = vt_cursor_row(vt);
	    if vt_cursor_col(vt) < self.v_col then
		if vt_cursor_col(vt) > 0 then
		    self.v_col = self.v_col - 1;
		    self:render_view();
		    self:move_cursor(o_row, o_col);
		end
	    end
	elseif keyval == 13 then -- Enter
	    this_line = self.read_line();
	    pos = ((self.page - 1) * rows) + vt_cursor_col(vt) + ((vt_cursor_row(vt)-0)*cols);
	    echo(pos.."\n");
	    left = string.sub(this_line, 0, self.cursor_col() - 1);
	    right = string.sub(this_line, self.cursor_col() + 1);
	    self.write(string.rep(string.char(32), string.len(right)));
	    self.write("\n");
	    self.write(right);
	    self.write(string.rep(string.char(8), string.len(right)));
	elseif keycode == 78 then -- home
	    self:home_cursor();
	elseif keycode == 79 then -- end
	elseif keyval == 9 then -- Tab
	    tab_length = 4;
	    keycode = 0;
	    keyval = 0;
	    this_line = self.read_line();
	    left = string.sub(this_line, 0, self.cursor_col() - 1);
	    right = string.sub(this_line, self.cursor_col() + 1);
	    --self.write(string.rep(string.char(32), string.len(right)));
	    self.write(string.rep(" ", tab_length));
	    echo("Right Len: "..string.len(right).."\n");
	    if (string.len(right)+tab_length+self.cursor_col() > self.cols) then
		old_col = self.cursor_col();
		self.write(right);
		self.keypress(84, 0);
		i = 0;
		for i = 1, (old_col - self.cursor_col()), 1 do
		    self.keypress(83, 0);
		end
	    else
		--self.write(string.rep(string.char(8), string.len(right)));
	    end
	    elseif keyval == 8 then -- Backspace
		self.write(string.char(8)..string.char(32));
	    elseif keycode == 60 then -- tilde ` ~
		keycode = 0;
		keyval = 0;

	    elseif keyval == 127 then -- Delete
		keycode = 0;
		keyval = 0;
		this_line = self.read_line();
		left = string.sub(this_line, 0, self.cursor_col() - 1);
		right = string.sub(this_line, self.cursor_col() + 2);
		self.write(right);
		self.write(string.rep(string.char(8), string.len(right)));
	    elseif keyval ~= 0 then
		--[[
		this_line = self.read_line();
		left = string.sub(this_line, 0, self.cursor_col() - 1);
		right = string.sub(this_line, self.cursor_col() + 1);
		self.write(string.char(keyval));
		if (string.len(right)+1+self.cursor_col() > self.cols) then
		    old_col = self.cursor_col();
		    self.write(right);
		    self.keypress(84, 0);
		    i = 0;
		    for i = 1, (old_col - self.cursor_col()), 1 do
			self.keypress(83, 0);
		    end
		else
		    --self.write(string.rep(string.char(8), string.len(right)));
		end
		keycode = 0;
		keyval = 0;]]
	    end
	return keycode, keyval;
    end

    function object:render_view()
	--echo("\n\n");
	v_col_s = self.v_col;
	v_row_s = self.v_row;
	v_col_e = v_col_s + self.cols;
	v_row_e = v_row_s + self.rows-4;
	c_col = 0; --self.col;
	c_row = 0; --self.row;
	start_pos = -1;
	--echo("VS-- "..v_row_s..", "..v_col_s.."\n");
	--echo("VE-- "..v_row_e..", "..v_col_e.."\n");

	b = "";
	for pos = 0, string.len(self.buffer), 1 do
	    if c_row >= v_row_s then
	        if c_col == v_col_s then
	    	    --echo("START POS: "..pos.."\n");
		    start_pos = pos;
		end
	    end
	    if c_col > v_col_e then
	        --b = b .. string.sub(self.buffer, start_pos, pos);
	        --start_pos = pos;
	    end
	    if c_row > v_row_e + 1 then
		break
	    end
	    f = 0;
	    if string.char(10) == string.sub(self.buffer, pos, pos) then
		if c_row >= v_row_s then
		    if c_col >= v_col_s then
			--echo(start_pos..", "..pos.."\n");
			b = b .. string.sub(self.buffer, start_pos, pos);
			f = 1;
			--echo("ROW: "..c_row.." COL: "..c_col.."\n");
			--echo("\""..string.sub(self.buffer, start_pos, pos).."\"\n");
		    end
		end
		if f == 0 then
		    b = b .. string.char(10);
		end
		c_col = 0;
		c_row = c_row + 1;
	    end
	    c_col = c_col + 1;
	end
	object:clear_term ();
	object:home_cursor();
	daters.surface.write(b);
    end

    return object;
end



