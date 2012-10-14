


model_class = {};
function model_class:new(model_filename)
    local object = {};
    local daters = {};
    daters.model = create_model();
    if (type(model_filename) == 'string') then
	daters.filename = model_filename;
	load_3ds(daters.model, model_filename);
    end

    daters.faces = {}

    setmetatable(object, {
	-- GET DATA
	__index = function (table, key) 
	    if key == "face_count" then
		return get_model_face_count(daters.model);
	    elseif key == "self" then
		return daters;
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
	daters.faces[face] = {};
	daters.faces[face]["vertices"] = {};
	daters.faces[face]["vertices"][vertex] = {};
	daters.faces[face]["vertices"][vertex]["x"] = x;
	daters.faces[face]["vertices"][vertex]["y"] = y;
	daters.faces[face]["vertices"][vertex]["z"] = z;
	daters.faces[face]["vertices"][vertex]["u"] = u;
	daters.faces[face]["vertices"][vertex]["v"] = v;
	daters.faces[face]["vertices"][vertex]["c"] = c;
    end

    daters.set_offset = function (ox, oy, oz)
	set_model_offset(daters.model, ox, oy, oz);
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
	    elseif key == "fov" then
		return get_camera_fov(daters.camera);
	    elseif key == "r_x" then
		return get_camera_r_x(daters.camera);
	    elseif key == "r_y" then
		return get_camera_r_y(daters.camera);
	    elseif key == "r_z" then
		return get_camera_r_z(daters.camera);
	    elseif key == "pitch" then
		return get_camera_pitch(daters.camera);
	    elseif key == "heading" then
		return get_camera_heading(daters.camera);
	    elseif key == "self" then
		return daters;
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
	    elseif key == "fov" then
		set_camera_fov(daters.camera, value);
	    elseif key == "r_x" then
		set_camera_r_x(daters.camera, value);
	    elseif key == "r_y" then
		set_camera_r_y(daters.camera, value);
	    elseif key == "r_z" then
		set_camera_r_z(daters.camera, value);
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

    daters.look_at = function (entity)
	dx = daters.x - entity.x;
--	if dx < 0 then
--	    dx = entity.x - daters.x;
--	end
	dy = entity.y - daters.y;
--	if dy < 0 then
--	    dy = daters.y - entity.y;
--	end
	
	object.heading = math.deg(math.atan(dy / dx));

	dx = entity.x - daters.x ;
--	if (dx < 0) then
--	    dx = entity.x - daters.x;
--	end
	dy = entity.z - daters.z;
--	if (dy < 0) then
--	    dy = daters.z - entity.z;
--	end
	
	object.pitch = math.deg(math.atan(dy / dx));
    end

    daters.turn_to = function (entity)
	dx = daters.x - entity.x;
--	if dx < 0 then
--	    dx = entity.x - daters.x;
--	end
	dy = entity.y - daters.y;
--	if dy < 0 then
--	    dy = daters.y - entity.y;
--	end
	--echo(object.heading.."\n");
	object.heading = math.deg(math.atan(dy / dx));

    end

    daters.set_rotation = function (w, x, y, z)
	set_matrix_rotation(daters.matrix, w, x, y, z);
    end

	
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
		return get_matrix_scale(daters.matrix);
	    elseif key == "self" then
		return daters;
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
    
    daters.set_rotation = function (w, x, y, z)
	set_matrix_rotation(daters.matrix, w, x, y, z);
    end
    
    daters.render = function (camera)
	render_model(daters.model.model, daters.matrix, camera.camera, get_surface_bitmap(daters.texture.surface), get_surface_bitmap(camera.surface.surface));
	camera.surface.set_dirty();
    end

    daters.render_wireframe = function (camera)
	render_model_wireframe(daters.model.model, daters.matrix, camera.camera, get_surface_bitmap(camera.surface.surface));
	camera.surface.set_dirty();
    end

    daters.plot = function (camera)
	fc = daters.model.face_count;
	daters.projection = {};
	for i=0,fc-1,1 do
	    x1, y1, z1, u1, v1, c1 = daters.model.get_face_data(i, 0);
	    x2, y2, z2, u2, v2, c2 = daters.model.get_face_data(i, 1);
	    x3, y3, z3, u3, v3, c3 = daters.model.get_face_data(i, 2);
	    x1, y1, d1 = plot_point(x1, y1, z1, daters.matrix, camera.camera);
	    x2, y2, d2  = plot_point(x2, y2, z2, daters.matrix, camera.camera);
	    x3, y3, d3  = plot_point(x3, y3, z3, daters.matrix, camera.camera);
	    
	    if (d1 < d2) and (d1 < d3) then
		if (x2 < x3) then
		    mid_x = x2 + ((x3 - x2)*.5);
		else 
		    mid_x = x3 + ((x2 - x3)*.5);
		end
		if (y2 < y3) then
		    mid_y = y2 + ((y3 - y2)*.5);
		else 
		    mid_y = y3 + ((y2 - y3)*.5);
		end
		o_x = x1;
		o_y = y1;
	    elseif (d2 < d1) and (d2 < d3) then
		if (x1 < x3) then
		    mid_x = x1 + ((x3 - x1)*.5);
		else 
		    mid_x = x3 + ((x1 - x3)*.5);
		end
		if (y1 < y3) then
		    mid_y = y1 + ((y3 - y1)*.5);
		else 
		    mid_y = y3 + ((y1 - y3)*.5);
		end
		o_x = x2;
		o_y = y2;		
	    elseif (d3 < d2) and (d3 < d1) then
		if (x1 < x2) then
		    mid_x = x1 + ((x2 - x1)*.5);
		else 
		    mid_x = x2 + ((x1 - x2)*.5);
		end		
		if (y1 < y2) then
		    mid_y = y1 + ((y2 - y1)*.5);
		else 
		    mid_y = y2 + ((y1 - y2)*.5);
		end	
		o_x = x3;
		o_y = y3;
	    end
	    
	    if (o_y > mid_y) then
		d_y = o_y - mid_y;
	    else
		d_y = mid_y - o_y;
	    end
	    if (o_x > mid_x) then
		d_x = o_x - mid_x;
	    else
		d_x = mid_x - o_x;
	    end
	    angle = math.atan2(d_y, d_x);
	    
	    daters.projection[i] = {};
	    daters.projection[i].x1 = x1;
	    daters.projection[i].x2 = x2;
	    daters.projection[i].x3 = x3;
	    daters.projection[i].y1 = y1;
	    daters.projection[i].y2 = y2;
	    daters.projection[i].y3 = y3;
	    daters.projection[i].d1 = d1;
	    daters.projection[i].d2 = d2;
	    daters.projection[i].d3 = d3;
	    
	    daters.projection[i].a = math.deg(angle);
	    daters.projection[i].ox = o_x;
	    daters.projection[i].oy = o_y;
	end
	--plot_model(daters.model.model, daters.matrix, camera.camera, get_surface_bitmap(camera.surface.surface));
    end



    daters.render_gouraud = function (camera)
	render_model_gouraud(daters.model.model, daters.matrix, camera.camera, get_surface_bitmap(camera.surface.surface));
	camera.surface.set_dirty();
    end

--    object.self = daters;
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
	    elseif key == "self" then
		return daters;
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
		hw = value.width * 1;
		hh = value.height * 1;
		scale = 1;
		sw = value.width / scale;
		sh = value.height / scale;
		square.model.set_face_data(0, 0, 0,hw*0, hh*0, 0-sw*0, 0-sh*0, 0); -- a
		square.model.set_face_data(0, 1, 0,hw*1, hh*0, 0-sw*1, 0-sh*0, 0); -- b
		square.model.set_face_data(0, 2, 0,hw*0, hh*1, 0-sw*0, 0-sh*1, 0); -- c
    
		square.model.set_face_data(1, 0, 0,hw*0, hh*1, 0-sw*0, 0-sh*1, 0); -- c
		square.model.set_face_data(1, 1, 0,hw*1, hh*1, 0-sw*1, 0-sh*1, 0); -- d 
		square.model.set_face_data(1, 2, 0,hw*1, hh*0, 0-sw*1, 0-sh*0, 0); -- b
	    else
		square[key] = value;		
	    end
	    square[key] = value;
	end	
    }
    );
    return object;
end

--model, texture_width, texture_height, side_length, origin_x, origin_y
create_cube = function(cube, cube_width, cube_height, side_length, origin_x, origin_y, origin_z, color) 
--cube = model_class:new();
echo("Cube Width: "..cube_width.." Cube Height: "..cube_height.." Side Length: "..side_length.." X: "..origin_x.." Y: "..origin_y.."\n");
l = side_length * 0.5;
ox = origin_x;
oy = origin_y;
oz = origin_z;

fc = cube.face_count;
cube.add_face();
cube.add_face();

cube.set_face_data(fc+0, 0, 	ox-l,  oy-l,  oz-l,   0,   0, color);
cube.set_face_data(fc+0, 1, 	ox+l, oy-l,  oz-l, cube_width,   0, color);
cube.set_face_data(fc+0, 2, 	ox+l, oy-l, oz+l, cube_width, cube_height, color);

cube.set_face_data(fc+1, 0, 	ox+l, oy-l, oz+l, cube_width, cube_height, color);
cube.set_face_data(fc+1, 1, 	ox-l, oy-l, oz+l,   0, cube_height, color);
cube.set_face_data(fc+1, 2, 	ox-l, oy-l,  oz-l,   0,   0, color);

cube.add_face();
cube.add_face();

cube.set_face_data(fc+2, 0, 	ox-l, oy-l,  oz-l,   0,   cube_height, color);
cube.set_face_data(fc+2, 1, 	ox-l, oy+l,  oz-l, 0,   0, color);
cube.set_face_data(fc+2, 2, 	ox-l, oy+l, oz+l, cube_width, 0, color);

cube.set_face_data(fc+3, 0, 	ox-l, oy+l, oz+l, cube_width, 0, color);
cube.set_face_data(fc+3, 1, 	ox-l, oy-l, oz+l,   cube_width, cube_height, color);
cube.set_face_data(fc+3, 2, 	ox-l, oy-l, oz-l,   0,  cube_height, color);

cube.add_face();
cube.add_face();

cube.set_face_data(fc+4, 0, 	ox+l,  oy-l,  oz-l,   cube_width,   cube_height, color);
cube.set_face_data(fc+4, 1, 	ox+l, oy+l,  oz-l, cube_width,   0, color);
cube.set_face_data(fc+4, 2, 	ox+l, oy+l, oz+l, 0, 0, color);
 
cube.set_face_data(fc+5, 0, 	ox+l, oy+l, oz+l, 0, 0, color);
cube.set_face_data(fc+5, 1, 	ox+l,  oy-l, oz+l, 0, cube_height, color);
cube.set_face_data(fc+5, 2, 	ox+l,  oy-l,  oz-l,   cube_width,   cube_height, color);

cube.add_face();
cube.add_face();

cube.set_face_data(fc+6, 0, 	ox-l,  oy+l,  oz-l,   0,   0, color);
cube.set_face_data(fc+6, 1, 	ox+l, oy+l,  oz-l, cube_width,   0, color);
cube.set_face_data(fc+6, 2, 	ox+l, oy+l, oz+l, cube_width, cube_height, color);

cube.set_face_data(fc+7, 0, 	ox+l, oy+l, oz+l, cube_width, cube_height, color);
cube.set_face_data(fc+7, 1, 	ox-l,  oy+l, oz+l,   0, cube_height, color);
cube.set_face_data(fc+7, 2,  	ox-l,  oy+l,  oz-l,   0,   0, color);

cube.add_face();
cube.add_face();

cube.set_face_data(fc+8, 0, 	ox-l,  oy-l,  oz-l,   cube_width,   cube_height, color);
cube.set_face_data(fc+8, 1, 	ox+l, oy-l,  oz-l, 0, cube_height, color);
cube.set_face_data(fc+8, 2, 	ox+l, oy+l, oz-l, 0, 0, color);

cube.set_face_data(fc+9, 0, 	ox+l, oy+l, oz-l, 0, 0, color);
cube.set_face_data(fc+9, 1, 	ox-l,  oy+l, oz-l,   cube_width, 0, color);
cube.set_face_data(fc+9, 2, 	ox-l,  oy-l,  oz-l,   cube_width,  cube_height, color);

cube.add_face();
cube.add_face();

cube.set_face_data(fc+10, 0, 	ox-l,  oy-l,  oz+l,   0,   cube_height, color);
cube.set_face_data(fc+10, 1, 	ox+l, oy-l,  oz+l, cube_width,   cube_height, color);
cube.set_face_data(fc+10, 2, 	ox+l, oy+l, oz+l, cube_width, 0, color);

cube.set_face_data(fc+11, 0, 	ox+l, oy+l, oz+l, cube_width, 0, color);
cube.set_face_data(fc+11, 1, 	ox-l,  oy+l, oz+l,   0, 0, color);
cube.set_face_data(fc+11, 2, 	ox-l,  oy-l,  oz+l,   0, cube_height, color);

end

create_3d_rectangle = function(cube, cube_width, cube_height, cube_depth, origin_x, origin_y, origin_z, color, face) 
--cube = model_class:new();
echo("Rect Width: "..cube_width.." Cube Height: "..cube_height.." Cube Depth: "..cube_depth.." X: "..origin_x.." Y: "..origin_y.."\n");

local ox = origin_x;
local oy = origin_y;
local oz = origin_z;

local fc = cube.face_count;

local w = cube_width * 0.5;
local h = cube_height * 0.5;
local d = cube_depth * 0.5;
local findex = 0;


if face == 1 or face == 0 then
    echo("FACE\n\n");
    cube.add_face();
    cube.add_face();

    -- A
    cube.set_face_data(fc+findex, 0, 	ox-w,  oy-h,  oz-d,   0,   0, color);
    -- B
    cube.set_face_data(fc+findex, 1, 	ox+w, oy-h,  oz-d, cube_width,   0, color);
    -- C
    cube.set_face_data(fc+findex, 2, 	ox+w, oy+h, oz-d, cube_width, cube_height, color);

    findex = findex + 1;
    -- C
    cube.set_face_data(fc+findex, 0, 	ox+w, oy+h, oz-d, cube_width, cube_height, color);
    -- D
    cube.set_face_data(fc+findex, 1, 	ox-w, oy+h, oz-d,   0, cube_height, color);
    -- A
    cube.set_face_data(fc+findex, 2, 	ox-w, oy-h,  oz-d,   0,   0, color);
    findex = findex + 1;
end

if face == 2 or face == 0 then
    cube.add_face();
    cube.add_face();
    cube.set_face_data(fc+findex, 0, 	ox-w, oy-h,  oz+d,   0,   cube_height, color);
    cube.set_face_data(fc+findex, 1, 	ox+w, oy-h,  oz+d, 0,   0, color);
    cube.set_face_data(fc+findex, 2, 	ox+w, oy+h, oz+d, cube_width, 0, color);
    findex = findex + 1;
    
    cube.set_face_data(fc+findex, 0, 	ox+w, oy+h, oz+d, cube_width, 0, color);
    cube.set_face_data(fc+findex, 1, 	ox-w, oy+h, oz+d,   cube_width, cube_height, color);
    cube.set_face_data(fc+findex, 2, 	ox-w, oy-h, oz+d,   0,  cube_height, color);
    findex = findex + 1;
end

if face == 3 or face == 0 then
    cube.add_face();
    cube.add_face();
    cube.set_face_data(fc+findex, 0, 	ox-w,  oy-h,  oz+d,   cube_width,   cube_height, color);
    cube.set_face_data(fc+findex, 1, 	ox-w, oy-h,  oz-d, cube_width,   0, color);
    cube.set_face_data(fc+findex, 2, 	ox-w, oy+h, oz-d, 0, 0, color);
    findex = findex + 1;

    cube.set_face_data(fc+findex, 0, 	ox-w, oy+h, oz-d, 0, 0, color);
    cube.set_face_data(fc+findex, 1, 	ox-w,  oy+h, oz+d, 0, cube_height, color);
    cube.set_face_data(fc+findex, 2, 	ox-w,  oy-h,  oz+d, cube_width, cube_height, color);
    findex = findex + 1;
end

if face == 4 or face == 0 then
    cube.add_face();
    cube.add_face();
    cube.set_face_data(fc+findex, 0, 	ox+w,  oy-h,  oz-d,   0,   0, color);
    cube.set_face_data(fc+findex, 1, 	ox+w, oy-h,  oz+d, cube_width,   0, color);
    cube.set_face_data(fc+findex, 2, 	ox+w, oy+h, oz+d, cube_width, cube_height, color);
    findex = findex + 1;
    
    cube.set_face_data(fc+findex, 0, 	ox+w, oy+h, oz+d, cube_width, cube_height, color);
    cube.set_face_data(fc+findex, 1, 	ox+w,  oy+h, oz-d,   0, cube_height, color);
    cube.set_face_data(fc+findex, 2,  	ox+w,  oy-h,  oz-d,   0,   0, color);
    findex = findex + 1;
end

if face == 5 or face == 0 then
    cube.add_face();
    cube.add_face();
    cube.set_face_data(fc+findex, 0, 	ox-w,  oy-h,  oz+d,   cube_width,   cube_height, color);
    cube.set_face_data(fc+findex, 1, 	ox+w, oy-h,  oz+d, 0, cube_height, color);
    cube.set_face_data(fc+findex, 2, 	ox+w, oy-h, oz-d, 0, 0, color);
    findex = findex + 1;
    
    cube.set_face_data(fc+findex, 0, 	ox+w, oy-h, oz-d, 0, 0, color);
    cube.set_face_data(fc+findex, 1, 	ox-w,  oy-h, oz-d,   cube_width, 0, color);
    cube.set_face_data(fc+findex, 2, 	ox-w,  oy-h,  oz+d,   cube_width,  cube_height, color);
    findex = findex + 1;
end

if face == 6 or face == 0 then
    cube.add_face();
    cube.add_face();
    cube.set_face_data(fc+findex, 0, 	ox-w,  oy+h,  oz-d,   0,   cube_height, color);
    cube.set_face_data(fc+findex, 1, 	ox+w, oy+h,  oz-d, cube_width,   cube_height, color);
    cube.set_face_data(fc+findex, 2, 	ox+w, oy+h, oz+d, cube_width, 0, color);
    findex = findex + 1;
    
    cube.set_face_data(fc+findex, 0, 	ox+w, oy+h, oz+d, cube_width, 0, color);
    cube.set_face_data(fc+findex, 1, 	ox-w,  oy+h, oz+d,   0, 0, color);
    cube.set_face_data(fc+findex, 2, 	ox-w,  oy+h,  oz-d,   0, cube_height, color);
    findex = findex + 1;
end
end

create_model_from_image =  function(model, surface, scale, size, invert) 
    local w = surface.width - 1;
    local h = surface.height - 1;
    local l = 0;

--    size = 64;
    local thing = size;
    local hw = (w * .5) * thing;
    local hh = (h * .5) * thing;
    
    local x = 0;
    local y = 0;
    local y2 = 0;
    local ys = 0;
    
--[[    for y=1, h, scale do
	for x=1, w, scale do
	    c = surface.get_pixel(x, y);
	    cl = surface.get_pixel(x-1, y);
	    ct = surface.get_pixel(x, y-1);
	    
	    r, g, b = get_rgb(c);
	    rl, gl, bl = get_rgb(c);
	    rt, gt, bt = get_rgb(c);
	    
	    cube_color = makecol(r, g, b);
	    if (invert ~= 0) then
		--cube_color = makecol(255-r, 255-g, 255-g);
		r = 255 - r;
	    else 
		
	    end
	    
	    echo(x..", "..y.." = "..r.." c= "..cube_color.."\n");
	    if r > 128 then
			--  model, 0, 0, size, ox,     oy,    oz, color
		create_cube(model, 0, 0, size, hw - (x*thing), hh-(y*thing), 0, cube_color);
		echo("Create Cube: "..x..", "..y.."\n");
	    else
	    end
	end
    end]]
    
    for x=1, w, scale do
	echo("FOR X\n");
	h = surface.height - 1;
	w = surface.width - 1;
	echo("H: "..h.." W: "..w.."\n");
	for y=1, h, scale do
	    c = surface.get_pixel(x, y);
	    r, g, b = get_rgb(c);
	    
	    cube_color = makecol(r, g, b);
	    if (invert ~= 0) then
		--cube_color = makecol(255-r, 255-g, 255-g);
		r = 255 - r;
	    else 
		
	    end
	    echo("X: "..x.." Y: "..y.."\n");
	    if r > 128 then
		
		-- We're at X, Y and we found a pixel
		-- run down Y until we find a different pixel
		ys = y;
		for y2=ys, h+1, scale do
		    c2 = surface.get_pixel(x, y2);
		    if (c2 ~= c) then
			l = (y2 - y)
			-- 1 = front
			-- 2 = back
			-- 3 = right side
			-- 4 = left side
			-- 5 = bottom
			-- 6 = top
			create_3d_rectangle(model, thing, (l)*thing, (size/128)*(r-128), hw - (x*thing), hh - ((y+(l*.5))*thing), 0, c, 1);
			--create_3d_rectangle(model, thing, (l)*thing, (size/128)*(r-128), hw - (x*thing), hh - ((y+(l*.5))*thing), 0, makecol(255,0,0), 6);
			--create_3d_rectangle(cube, cube_width, cube_height, cube_depth, origin_x, origin_y, origin_z, color) 
			echo("Created Rect From X:"..x.." Y: "..y.." hh:  "..(hh).." thing: "..(thing).."\n");
			y = y2-1;
			break;
		    end
		end
		echo("WOOP: "..y.."\n");
			--  model, 0, 0, size, ox,     oy,    oz, color
		--create_cube(cube, cube_width, cube_height, side_length, origin_x, origin_y, origin_z, color) 
		--create_cube(model, 0, 0, size, hw - (x*thing), hh-(y*thing), 0, cube_color);
		--echo("Create Cube: "..x..", "..y.."\n");
	    else
	    end
	end
    end
    
end