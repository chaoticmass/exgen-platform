
font1 = surface:new();
font1.visible = 0;
font1.bmp = "fonts/black_micro_computer.bmp";

font2 = surface:new();
font2.visible = 0;
font2.bmp = "fonts/black_small_monospace.bmp";

term_window = window:new();
term_window.font = font2;
term_window.width = screen_width();
term_window.height = screen_height() - 32;
term_window.caption = "exterm";
term_window.visible = 1;

term_window.top = 0;
term_window.left = 0;
term_window.z = 9999;

term = terminal:new(80, 25);
term.parent = term_window;
term.font = font1;
term.text = "Hey";
term.top = term_window.margin_top;
term.left = term_window.margin_left;
term.width = (font1.text_length("W")-3) * (term.cols + 1);
term.height = font1.text_height() * (term.rows + 13);
--term.transparent = 1;
term.backcolor = makecol(255, 255, 255);
term.clear();

term.on_vt_dirty = function ()
    --term_window.on_draw();
end

term_window.width = term.width + term_window.margin_left + term_window.margin_right;
term_window.height = term.height + term_window.margin_top + term_window.margin_bottom;
term_window.z = 9999;
term_window.on_resize = function ()
    term.width = term_window.width - 12;
    term.height = term_window.height - term.top - 6;
end

term_window.on_click = function ()
    term.set_focus();
end
term.set_focus();

function term:after_keypress(keycode, keyval)
    if keycode == 51 then
	script = self:read_buffer();
	script = [[
output = "";
cecho = function(text)
    output = output .. text;
    setvar("exterm", "output", output);
end

]].. script;
	savefile("script.lua", script);
	include("script.lua");
	if (type(term.l) == "nil") then
	    --term.l = branch("script.lua");
	else
	    --branch("script.lua", term.l);
	end
--	term.poll_output();
    end
end

term_window.top = (screen_height() * 0.5) - (term_window.height * 0.5);
term_window.left = (screen_width() * 0.5) - (term_window.width * 0.5);
term_window.close.on_click = function()
    term_window.visible = 0;
end

term.poll_output = function()
--    if (getvar("exterm", "output") ~= "") then
--	term.write(getvar("exterm", "output"));
--	setvar("exterm", "output", "");
--    end
end

term_window.backcolor = makecol(255, 255, 255);
--[[term_window.before_draw = function()
    term.poll_output();
end]]
term_window.close.visible = 0;
term_window.on_draw = function()
    term_window.clear();
end
-- end setup code--------------------

--term.forkpty("/bin/bash");
--render_loop(term_window.surface);
