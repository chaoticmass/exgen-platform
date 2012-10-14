font1 = surface:new();
font1.visible = 0;
font1.bmp = 'fonts/small.bmp';


form = window:new();
form.font = font1;
--form.no_titlebar = 1;
--form.bare = 1;
--form.draggable = 0;
form.top = 0;

form.width = screen_width() * .9;
form.left = (screen_width() * .5) - (form.width * .5);
form.height = 320;
form.backcolor = makecol(255, 255, 255);

form.z = 9999;
form.clear()
form.on_draw();
form.visible = 1;

form.on_redraw = function() 
end
-- end setup code--------------------


db = client_interface:new();

db.hostname = "65.66.245.90";
db.form_query = [[
request:type=hidden
request:dataset=inventory
request:id=1241898521
request:title="%_title:value_%"


label1:type=label
:parent=form
:font=font1
:top=form.align_top(label1)
:left=form.align_left(label1)
:caption=Title:
:transparent=1
:backcolor=makecol(255, 0, 255)

title:type=inputbox
:parent=form
:font=font1
:backcolor=makecol(255, 255, 255)
:width=.8
:top=label1.top
:left=title.parent.align_center(title)
:z=9999
:db=title

label2:type=label
:parent=form
:font=font1
:top=form.align_top(label2)
:left=label2.left
:caption=Thing:
:transparent=1
:backcolor=makecol(255, 0, 255)
:transcolor=makecol(255, 0, 255)

buyitnow:type=inputbox
:parent=form
:font=font1
:backcolor=makecol(255, 255, 255)
:width=.8
:top=label2.top
:left=form.align_center(buyitnow)
:z=9999
:db=buyitnow

buyitnow2:type=inputbox
:parent=form
:font=font1
:backcolor=makecol(255, 255, 255)
:width=.8
:top=form.align_top(buyitnow2)
:left=form.align_center(buyitnow2)
:z=9999
:db=buyitnow

buyitnow3:type=inputbox
:parent=form
:font=font1
:backcolor=makecol(255, 255, 255)
:width=.8
:top=form.align_top(buyitnow3)
:left=form.align_center(buyitnow3)
:z=9999
:db=buyitnow

buyitnow4:type=inputbox
:parent=form
:font=font1
:backcolor=makecol(255, 255, 255)
:width=.8
:top=form.align_top(buyitnow4)
:left=form.align_center(buyitnow4)
:z=9999
:db=buyitnow

description:type=textbox
:parent=form
:font=font1
:backcolor=makecol(255, 255, 255)
:width=.8
:height=.3
:top=form.align_top(description)
:left=form.align_center(description)
:z=9999
:db=description



submit:type=button
:parent=form
:font=font1
:caption=Save
:width=.25
:top=submit.parent.align_top(submit)
:left=submit.parent.align_center(submit)
:z=9999

]]

db.db_query = "select * from meta->forms where name = help_page get all";
data = db.db_post();

include_string(data);

--db.form_query = results.records[1].form_string.."\n";
echo(db.form_query.."\n\n");
data = db.form_post();
echo("\n\n");

echo(data);
include_string(data);


new_height = form.current_top + form.margin_top + form.margin_bottom;
if (new_height > screen_height()) then
    new_height = screen_height() - 32;
end

echo(form.current_top);
echo(" <-- current top \n");
echo(new_height);
echo(" <-- new height \n");
form.height = new_height;
echo(request.title.."<--- Title\n");
form.caption = request.title;
if (form.height < screen_height() - 32) then
    form.top = ((screen_height() - 32)* .5) - (form.height * .5);
else
    form.width = screen_width();
    form.left = 0;
end
--form.current_top = 2000;
render_loop(form.surface);
