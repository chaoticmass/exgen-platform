load_form = function(form_name, id) 

    elements = {};
    form = window:new();
    if (font2) then
	form.font = font2;
    elseif (font1) then
	form.font = font1;
    end

    --form.font = font1;
    --form.no_titlebar = 1;
    --form.bare = 1;
    --form.draggable = 0;
    form.top = 0;

    form.width = screen_width() * .9;
    form.left = (screen_width() * .5) - (form.width * .5);
    form.height = 240;
    form.top = (screen_height() * .5) - (form.height * .5);
    form.backcolor = makecol(255, 255, 255);

    form.z = 9999;
    form.clear()
    form.on_draw();
    form.visible = 1;

    form.on_redraw = function() 
    end

    render();
    


    if form_name == "" then
	form_name = "login";
    end

    db = client_interface:new();

    db.hostname = "65.66.245.90";
    db.db_query = "select * from meta->exgen_forms where name = ".. form_name  .." get all";
    db_source = db.db_post();
    echo(db_source);
    include_string(db_source);
    
    --[[
    start = timer();
    while timer() - start < 10 do
	echo("waiting ".. timer() - start .."\n");
	form.on_draw();
	render(form.surface);
    end ]]

    if (type(id) == 'string') then
	db.form_query = 'request:id='..id.."\n";
    else
	db.form_query = '';
    end
    db.form_query = db.form_query .. results.records[1].form_string.."\n";
    db.post_fields = post;
    form_source = db.form_post();

    echo("\n\n\nForm Source:\n".. form_source .."\n");
    include_string(form_source);


    new_height = form.current_top + form.margin_top + form.margin_bottom;
    if (new_height > screen_height()) then
	new_height = screen_height() - 32;	
    end

    --[[
    echo(form.current_top);
    echo(" <-- current top \n");
    echo(new_height);
    echo(" <-- new height \n");]]
    form.height = new_height;
    if (type(request) == "table") then
	if (type(request.title) == "string") then
	    form.caption = request.title;
	end
    end
    if (form.height < screen_height() - 32) then
	form.top = ((screen_height() - 32)* .5) - (form.height * .5);
    else
	form.width = screen_width();
	form.left = 0;
	form.top = 0;
    end

    --form.set_focus();
    form.on_mouse_down(0, 0, 0);    
    form.on_mouse_up(0, 0, 0);
    render_loop(form.surface);
    echo("request.id: ");
    echo(request.id);
    echo("\n");
    return elements;
end


render_form = function(form_source) 

    elements = {};
    form = window:new();
    if (font2) then
	form.font = font2;
    elseif (font1) then
	form.font = font1;
    end

    --form.font = font1;
    --form.no_titlebar = 1;
    --form.bare = 1;
    --form.draggable = 0;
    form.top = 0;

    form.width = screen_width() * .9;
    form.left = (screen_width() * .5) - (form.width * .5);
    form.height = 240;
    form.top = (screen_height() * .5) - (form.height * .5);
    form.backcolor = makecol(255, 255, 255);

    form.z = 9999;
    form.clear()
    form.on_draw();
    form.visible = 1;

    form.on_redraw = function() 
    end

    render();

--    include_string(form_source);
    
    new_height = form.current_top + form.margin_top + form.margin_bottom;
    if (new_height > screen_height()) then
	new_height = screen_height() - 32;	
    end

    --[[
    echo(form.current_top);
    echo(" <-- current top \n");
    echo(new_height);
    echo(" <-- new height \n");]]
    form.height = new_height;
    if (type(request) == "table") then
	if (type(request.title) == "string") then
	    form.caption = request.title;
	end
    end
    if (form.height < screen_height() - 32) then
	form.top = ((screen_height() - 32)* .5) - (form.height * .5);
    else
	form.width = screen_width();
	form.left = 0;
	form.top = 0;
    end

    --form.set_focus();
    form.on_mouse_down(0, 0, 0);    
    form.on_mouse_up(0, 0, 0);
    render_loop(form.surface);
    echo("request.id: ");
    echo(request.id);
    echo("\n");
    return elements;
end
