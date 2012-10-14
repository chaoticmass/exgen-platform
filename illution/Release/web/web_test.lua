include("lib/render3d.lua");
include("lib/libode.lua");
include("lib/latex.lua");
include("lib/serialize.lua");
function round(num, idp)
  local mult = 10^(idp or 0)
  return math.floor(num * mult + 0.5) / mult
end

VERSION_BASE = "0.1";
VERSION_SUFFIX = "A";
VERSION = VERSION_BASE .. VERSION_SUFFIX;
set_window_title("Web Interface Test v."..VERSION);


title_font = surface:new();
title_font.visible = 0;
title_font.bmp = "fonts/black_small_monospace.bmp";

body_font = surface:new();
body_font.visible = 0;
body_font.bmp = "fonts/black_micro_light.bmp";

screen = surface:new(0);
mouse = mouse_cursor:new();

bg = surface:new();
--bg.parent = screen;
bg.width = screen_width();
bg.height = screen_height();
bg.on_draw = function() 
    bg.gradient(makecol(255, 255, 255), makecol(200, 200, 200));
end

button1 = button:new();
button1.parent = bg;
button1.font = body_font;
button1.top = bg.margin_top+6;
button1.left = (bg.width * .5) - (button1.width * .5);

include("web/base64.lua");
include("web/http.lua");