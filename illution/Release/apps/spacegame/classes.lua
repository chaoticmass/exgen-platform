


model_class = {};
function model_class:new(model_filename)
    local object = {};
    local daters = {};
    daters.model = create_model();
    if (model_filename ~= '') then
	daters.filename = model_filename;
	load_3ds(daters.model, model_filename);
    end

    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "face_count" then
		return get_model_face_count(daters.model);
	    end
	    return daters[key];
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    daters[key] = value;
	end	
    }
    );

    daters.add_face = function ()
	model_add_face(daters.model);
    end
    
    daters.set_face_data = function (face, vertex, x, y, z, u, v, c)
	set_model_face_data(daters.model, face, vertex, x, y, z, u, v, c);
    end
    
    daters.get_face_data = function (face, vertex)
	return get_model_face_data(daters.model, face, vertex);
    end
    
    return object;
end

camera_class = {};
function camera_class:new()
    local object = {};
    local daters = {};
    daters.camera = create_camera(); 
    daters.surface = surface:new();
    set_camera_bitmap(daters.camera, get_surface_bitmap(daters.surface.surface));
    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "x" then
		return get_camera_x(daters.camera);
	    elseif key == "y" then
		return get_camera_y(daters.camera);
	    elseif key == "z" then
		return get_camera_z(daters.camera);
	    elseif key == "pitch" then
		return get_camera_pitch(daters.camera);
	    elseif key == "heading" then
		return get_camera_heading(daters.camera);
	    else 
		return daters[key];
	    end
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    if key == "x" then
		set_camera_x(daters.camera, value);
	    elseif key == "y" then
		set_camera_y(daters.camera, value);
	    elseif key == "z" then
		set_camera_z(daters.camera, value);
	    elseif key == "pitch" then
		set_camera_pitch(daters.camera, value);
	    elseif key == "heading" then
		set_camera_heading(daters.camera, value);
	    else
		daters[key] = value;
	    end
	    daters[key] = value;
	end	
    }
    );

	
    daters.update = function () 
	set_camera_bitmap(daters.camera, get_surface_bitmap(daters.surface.surface));
	daters.surface.clear();
	update_camera(daters.camera);
    end

    daters.initialize = function () 
	set_camera_bitmap(daters.camera, get_surface_bitmap(daters.surface.surface));
	daters.surface.clear();
	initialize_camera(daters.camera);
	update_camera(daters.camera);
    end


    return object;
end

entity_class = {};
function entity_class:new()
    local object = {};
    local daters = {};

    daters.matrix = create_matrix();
    daters.collision_model = model_class:new();
    
    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "x" then
		return get_matrix_x(daters.matrix);
	    elseif key == "y" then
		return get_matrix_y(daters.matrix);
	    elseif key == "z" then
		return get_matrix_z(daters.matrix);
	    elseif key == "r_x" then
		return get_matrix_r_x(daters.matrix);
	    elseif key == "r_y" then
		return get_matrix_r_y(daters.matrix);
	    elseif key == "r_z" then
		return get_matrix_r_z(daters.matrix);
	    elseif key == "scale" then
		return get_matrix_pitch(daters.matrix);
	    else 
		return daters[key];
	    end
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    if key == "scale" then
		set_matrix_scale(daters.matrix, value);
	    elseif key == "x" then
		set_matrix_x(daters.matrix, value);
	    elseif key == "y" then
		set_matrix_y(daters.matrix, value);
	    elseif key == "z" then
		set_matrix_z(daters.matrix, value);
	    elseif key == "r_x" then
		set_matrix_r_x(daters.matrix, value);
	    elseif key == "r_y" then
		set_matrix_r_y(daters.matrix, value);
	    elseif key == "r_z" then
		set_matrix_r_z(daters.matrix, value);
	    else
		daters[key] = value;
	    end
	    daters[key] = value;
	end	
    }
    );

    daters.update = function () 
	update_matrix(daters.matrix, daters.model.model, daters.collision_model.model);
    end
    
    daters.render = function (camera)
	render_model(daters.model.model, daters.matrix, camera.camera, get_surface_bitmap(daters.texture.surface), get_surface_bitmap(camera.surface.surface));
	camera.surface.set_dirty();
    end
    return object;
end

surface3d_class = {};
function surface3d_class:new()
    local object = {};
    local square = {};

    square = entity_class:new();
    square.model = model_class:new();
    square.model.add_face();
    square.model.add_face();
			    --        x,	y, 	z, 	u, 	v, 	c
    square.model.set_face_data(0, 0, 0, 	0, 	0, 	0, 	0, 	0);			
    square.model.set_face_data(0, 1, 64, 	0, 	0, 	128, 	0, 	0);
    square.model.set_face_data(0, 2, 64, 	0, 	64, 	128, 	128, 	0);

    
    square.model.set_face_data(1, 0, 64, 	0, 	64,	128, 	128, 	0);
    square.model.set_face_data(1, 1, 0, 	0, 	64,	0, 	128, 	0);
    square.model.set_face_data(1, 2, 0, 	0, 	0, 	0, 	0, 	0);

    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "x" then
		return square.x;
	    elseif key == "y" then
		return square.y;
	    else 
		return square[key];
	    end
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    if key == "x" then
		square.x = value;
	    elseif key == "y" then
		square.y = value;
	    elseif key == "texture" then
		square.texture = value;
		hw = value.width * .5;
		hh = value.height * .5;
		square.model.set_face_data(0, 0, 0 - hw, 	0, 	0 - hh, 	0, 		0, 	0);			
		square.model.set_face_data(0, 1, hw, 		0, 	0 - hh, 	value.width, 	0, 	0);
		square.model.set_face_data(0, 2, hw, 		0, 	hh, 		value.width, 	value.height, 	0);
    
		square.model.set_face_data(1, 0, hw, 		0, 	hh,	value.width, 	value.height, 	0);
		square.model.set_face_data(1, 1, 0 - hw, 	0, 	hh,	0, 	value.height, 	0);
		square.model.set_face_data(1, 2, 0 - hw, 	0, 	0 - hh, 	0, 	0, 	0);
		
	    else
		square[key] = value;		
	    end
	    square[key] = value;
	end	
    }
    );
    return object;
end