
latex_interface = {};
function latex_interface:new()
    local object = {};
    local daters = {};
    

    local tex = daters;
    tex.hostname = '127.0.0.1';
    tex.url = '/latex/latex';
    tex.source = '';
    tex.output_format = 'jpg';
    tex.output_path = '';
    tex.post_fields = '';
    tex.page_count = 0;
    tex.image_size = 'tight';
    tex.id = '';
    tex.pages = {};
    tex.images = {};
    tex.fonts = '';

    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    return daters[key];
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    daters[key] = value;
	end	
    }
    );

    tex.create_document = function()
	tex.id = download_string(tex.hostname, tex.url, tex.post_fields .."&source="..tex.source.."&output="..tex.output_format.."&image_size="..tex.image_size.."&action=return_id&id="..tex.id.."&fonts="..tex.fonts);
	tex.page_count = tonumber(download_string(tex.hostname, tex.url, tex.post_fields .."&action=return_page_count&id="..tex.id));
	return retval;
    end

    tex.destroy_document = function()
	if (tex.id ~= '') then
	    download_string(tex.hostname, tex.url, tex.post_fields .."&action=delete&id="..tex.id);
	    tex.id = '';
	end
	return retval;
    end
    

    tex.download_page = function(page)
	if (page == nil) then
	    page = 0;
	end
	if (tonumber(page) > 0) then
	    filename = tex.output_path..tex.id.."_"..page.."."..tex.output_format;
	    retval = download_file(tex.hostname, tex.url, tex.post_fields .."source="..tex.source.."&output="..tex.output_format.."&page="..page.."&image_size="..tex.image_size.."&action=return_output&id="..tex.id, filename);
	    tex.pages[1] = filename;
	else
	    echo(tex.page_count);
	    echo("\n");
	    tex.pages = {};
	    if (tex.page_count ~= nil) then
	    if (tonumber(tex.page_count) > 0) then
		for i=1,tex.page_count,1 do
		    filename = tex.output_path..tex.id.."_"..i.."."..tex.output_format;
		    retval = download_file(tex.hostname, tex.url, tex.post_fields .."source="..tex.source.."&output="..tex.output_format.."&page="..i.."&image_size="..tex.image_size.."&action=return_output&id="..tex.id, filename);
		    tex.pages[i] = filename;
		end
	    end
	    end
	end
	return retval;
    end

    tex.download_pdf = function()
	filename = tex.output_path..tex.id..".pdf";
	retval = download_file(tex.hostname, tex.url, tex.post_fields .."source="..tex.source.."&output=pdf&action=return_output&id="..tex.id, filename);
	tex.pdf = filename;
	return retval;
    end


    tex.load_page = function(page)
    	if (page == nil) then
	    page = 0;
	end
	if (tonumber(page) > 0) then
	    tex.images[1] = surface:new();
	    img = tex.images[1];
	    img.visible = 0;
	    img.gif = tex.pages[1];
	else
	    for i=1,tex.page_count,1 do
		echo("Loading image "..i.."\n");
		tex.images[i] = surface:new();
		img = tex.images[i];
		--img.visible = 0;
		img.gif = tex.pages[i];	
	    end
	end
    end

    return object;
end


function tprint (t, indent, done)
  done = done or {}
  indent = indent or 0
  for key, value in pairs (t) do
    print (string.rep (" ", indent)) -- indent it
    if type (value) == "table" and not done [value] then
      done [value] = true
      print (tostring (key), ":");
      tprint (value, indent + 2, done)
    else
      print (tostring (key), "=")
      print (value)
    end
  end
end