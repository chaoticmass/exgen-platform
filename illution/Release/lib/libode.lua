


worldspace_class = {};
function worldspace_class:new(gravity)
    local object = {};
    local daters = {};
    
    if (type(gravity) == "nil") then
	gravity = -9.81;
    end
    
    daters.ws = create_worldspace(gravity);

    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "face_count" then
		--return get_model_face_count(daters.model);
	    end
	    return daters[key];
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    daters[key] = value;
	end	
    }
    );

    daters.create_plane = function ()
	return create_plane(daters.ws);
    end
    
    daters.simulate = function ()
	simulate(daters.ws);
    end

    daters.simulate_step = function (step)
	simulate(daters.ws, step);
    end

    daters.destroy = function()
	destroy_worldspace(daters.ws);
	daters.ws = nil;
    end
        
    return object;
end

body_class = {};
function body_class:new(worldspace, body)
    local object = {};
    local daters = {};
    
    if (type(body) == "nil") then
	daters.body = create_body(worldspace.ws, "sphere", 1, 1); 
    else
	daters.body = body;
    end
    
    
    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "x" then
		--return get_camera_x(daters.camera);
	    else 
		return daters[key];
	    end
	end,
	-- SET DATA
	__newindex = function (table, key, value)
	    if key == "x" then
		--set_camera_x(daters.camera, value);
	    else
		daters[key] = value;
	    end
	    daters[key] = value;
	end	
    }
    );

    daters.is_connected = function(body)
	return body_is_connected(daters.body, body);
    end

    daters.get_mass = function () 
	return body_get_mass(daters.body);
    end

    daters.set_mass = function (mass) 
	body_set_mass(daters.body, mass);
    end
	
    daters.get_position = function () 
	return body_get_position(daters.body);
    end

    daters.set_position = function (x, y, z) 
	body_set_position(daters.body, x, y, z);
    end

    daters.get_rotation = function ()
	return body_get_rotation(daters.body);
    end

    daters.set_rotation = function (r_x, r_y, r_z)
	body_set_rotation(daters.body, r_x, r_y, r_z);
    end


    daters.set_linear_velocity = function (f_x, f_y, f_z)
	body_set_linear_velocity(daters.body, f_x, f_y, f_z);
    end

    daters.set_angular_velocity = function (f_x, f_y, f_z)
	body_set_angular_velocity(daters.body, f_x, f_y, f_z);
    end

    daters.get_angular_velocity = function ()
	return body_get_angular_velocity(daters.body);
    end

    daters.get_linear_velocity = function ()
	return body_get_linear_velocity(daters.body);
    end

    daters.set_force = function (f_x, f_y, f_z)
	body_set_force(daters.body, f_x, f_y, f_z);
    end

    daters.add_force = function (f_x, f_y, f_z)
	body_add_force(daters.body, f_x, f_y, f_z);
    end

    daters.add_relative_force = function (f_x, f_y, f_z)
	body_add_relative_force(daters.body, f_x, f_y, f_z);
    end

    daters.add_torque = function (f_x, f_y, f_z)
	body_add_torque(daters.body, f_x, f_y, f_z);
    end

    daters.add_relative_torque = function (f_x, f_y, f_z)
	body_add_relative_torque(daters.body, f_x, f_y, f_z);
    end
    
    daters.get_plane_depth = function(plane)
	return get_plane_depth(plane, daters.get_position());
    end
    
    daters.destroy = function()
	destroy_body(daters.body);
	daters.body = nil;
    end

    return object;
end
