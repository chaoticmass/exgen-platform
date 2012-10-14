include("lib/form.lua");

font1 = surface:new();
font1.visible = 0;
font1.bmp = "fonts/black_small_monospace.bmp";

font2 = surface:new();
font2.visible = 0;
font2.bmp = "fonts/black_micro_computer.bmp";

font3 = surface:new();
font3.visible = 0;
font3.bmp = "fonts/black_large_tahoma.bmp";


db = client_interface:new();
db.hostname = "65.66.245.90";
db.db_query = "select * from meta->forms get all";
db_source = db.db_post();
echo(db_source);
include_string(db_source);

window1 = window:new();
window1.font = font3;
window1.caption = "";
window1.draggable = 0;
window1.top = 0;
window1.left = 0;
window1.width = screen_width();
window1.height = screen_height() - 32;
window1.visible = 1;




text1 = inputbox:new();
text1.parent = window1;
text1.font = font1;
text1.top = window1.margin_top;
text1.left = window1.margin_left;
text1.backcolor = makecol(128, 128, 128);
text1.width = window1.inside_width - 32;
text1.text = db.db_query;
text1.on_return = function()
    ibox.clear_icons();
    db.db_query = text1.text;
    db_source = db.db_post();
    echo(db_source);
    include_string(db_source);
    ibox.arrange_auto = 0;
    start = timer()
    ibox.visible = 0;
    for key, value in pairs(results.records) do
	ibox.add_icon(tostring(value.title), icon, "", tostring(value.id));
	--echo(tostring(value.title).."\n");
    end
    ibox.arrange_icons();
    window1.vscroll.max = ibox.scroll_max;
    ibox.visible = 1;
    ibox.set_dirty();
    ibox.on_redraw();
    
    echo(timer() - start);
    echo("\n");
end


icon = surface:new();
icon.visible = 0;
icon.fuck = 1;
icon.gif = "images/icons/folder.gif";



ibox = listbox:new();
ibox.parent = window1;
ibox.top = text1.top + text1.height + 3;
ibox.left = ibox.parent.margin_left;
ibox.width = 200;
ibox.height = 300;

ibox.font = font2;
ibox.backcolor = makecol(255, 255, 255);
ibox.transparent = 0;
ibox.z = 1;
ibox.arrange_vertical = 0;
ibox.visible = 1;

for key, value in pairs(results.records) do
    ibox.add_icon(tostring(value.title), nil, "", tostring(value.id));
end

ibox.on_icon_activate = function (icon) 
    echo(icon.data.." actuvated\n");
    
    script = [[
include("lib/form.lua");
font1 = surface:new();
font1.visible = 0;
font1.bmp = "font/small.bmp";

    ]];
    
    script = script.. 'load_form("view_item", '..icon.data..');';
--    run_string(script);
    ibox.z = 1;
end
ibox.on_redraw();

--[[
window1.vscroll = scrollbar:new();
window1.vscroll.parent = window1;
window1.vscroll.max = ibox.scroll_max;
window1.vscroll.size = 20;
window1.vscroll.value = 0;

window1.vscroll.on_change = function ()
    ibox.scroll_position = 0 - window1.vscroll.value;
end

window1.vscroll.on_mouse_up = function()
    ibox.set_dirty();
    ibox.on_redraw();
end]]

--window1.vscroll.target = ibox;



--[[
window1.on_draw = function()
    
end
]]

render_loop(window1.surface);
