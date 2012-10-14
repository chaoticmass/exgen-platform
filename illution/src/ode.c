/*
static dWorldID world;
static dSpaceID space;
static dBodyID body;
static dGeomID geom;
static dMass m;
static dJointGroupID contactgroup;

*/


// dummy __cxa_pure_virtual
void __cxa_pure_virtual(void) {
}


int l_create_worldspace(void * L) {
    struct worldspace_type *ws = calloc(1, sizeof(struct worldspace_type));
    int n = lua_gettop(L);
    int gravity = 0;
    
    
    if (ode_initialized == 0) {
	//dInitODE();
	ode_initialized = 1;
    }
    ws->world = dWorldCreate();
    ws->space = dHashSpaceCreate(0);
    ws->contactgroup = dJointGroupCreate(0);
    ws->lasttime = timer();
    
    //set sane defaults
    if (n >= 1) {
	gravity = lua_tonumber(L, 1);
    } else {
	gravity = -9.81;
    }

    dWorldSetGravity(ws->world, 0, 0, gravity);
    dWorldSetCFM(ws->world, 1e-5);
    
    lua_pushlightuserdata(L, ws);
    return 1;
}

int l_destroy_worldspace(void * L) {
    struct worldspace_type *ws = lua_touserdata(L, 1);

    dSpaceDestroy(ws->space);
    dWorldDestroy(ws->world);
    free(ws);
    return 0;
}

int l_create_plane(void * L) {
    struct worldspace_type *ws = lua_touserdata(L, 1);
    
    lua_pushlightuserdata(L, dCreatePlane(ws->space, 0,0,1,0));
    return 1;
}

int l_plane_set_geom(void * L) {
    dGeomID gid = lua_touserdata(L, 1);
    
    dGeomPlaneSetParams(gid, lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
    return 0;
}




/*dReal heightfield_callback(void *user_data, int x, int z) {
    return 0;
}*/

int l_create_body(void * L) {
    struct worldspace_type *ws = lua_touserdata(L, 1);
    int n = lua_gettop(L);
    char *type = NULL;
    int type_num = -1;
    dReal radius = 0.5; 
    dReal mass = 1;
    dReal lx = 0;
    dReal ly = 0;
    dReal lz = 0;
    
    
    if (n >= 2) {
	type = lua_tostring(L, 2);
	if (strcmp(type, "sphere") == 0) {
	    type_num = 0;
	    radius = lua_tonumber(L, 3);
	    mass = lua_tonumber(L, 4);	    
	} else if (strcmp(type, "box") == 0) {
	    type_num = 1;
	    
	    // side lengths
	    lx = lua_tonumber(L, 3);
	    ly = lua_tonumber(L, 4);
	    lz = lua_tonumber(L, 5);
	    
	    mass = lua_tonumber(L, 6);
	} else if (strcmp(type, "trimesh") == 0) {
	    type_num = 2;
	} else if (strcmp(type, "heightfield") == 0) {
	//    type_num = 3;
	}
    }
    
    struct body_type *b = calloc(1, sizeof(struct body_type));
    
    b->body = dBodyCreate(ws->world);
    b->type_num = type_num;
    b->world = &ws;

    if (type_num == 0) {
	b->geom = dCreateSphere(ws->space, radius);
	dMassSetSphereTotal(&b->mass, mass, radius);
	dBodySetMass(b->body, &b->mass);
	dGeomSetBody(b->geom, b->body);
	dBodySetPosition(b->body, 0,0,3);

    } else if (type_num == 1) {
	b->geom = dCreateBox(ws->space, lx, ly, lz);
	dMassSetBoxTotal(&b->mass, mass, lx, ly, lz);
	dBodySetMass(b->body, &b->mass);
	dGeomSetBody(b->geom, b->body);
	dBodySetPosition(b->body, 0,0,3);

    } else if (type_num == 2) {
    } else if (type_num == 3) {
/*    
	float *height_data = calloc(10*10*4, sizeof(float));
	dReal width = 10;
	dReal depth = 10;
	int width_samples = 2;
	int depth_samples = 2;
	dReal scale = 1;
	dReal offset = 0;
	dReal thickness = 1;
	int wrap = 0;
	b->heightfield_data_id = dGeomHeightfieldDataCreate();
	printf("Heightfield Data ID: %i\n", b->heightfield_data_id);
	/dGeomHeightfieldDataBuildSingle (b->heightfield_data_id,
                                      height_data,
                                      1,
                                      width, depth,
                                      width_samples, depth_samples,
                                      scale, offset, thickness, wrap);	/
	dGeomHeightfieldDataBuildCallback(b->heightfield_data_id, NULL, 
	heightfield_callback, width, depth, width_samples, depth_samples, 
	1, 0, 0, 0);
	dGeomHeightfieldDataSetBounds(b->heightfield_data_id, -4, +6);
	
	
        b->geom = dCreateHeightfield(ws->space, b->heightfield_data_id, 1);
        
/        dVector3 pos;
        pos[0] = 0;
        pos[1] = 0;
        pos[2] = 0;
        
        dMatrix3 R;
        dRSetIdentity(R);
        dRFromAxisAndAngle(R, 1, 0, 0, Deg2Rad*90);
        
        dGeomSetRotation(b->geom, R);
        dGeomSetPosition(b->geom, pos[0], pos[1], pos[2]);/
        
        
        free(height_data);*/
    }
    	
    
    lua_pushlightuserdata(L, b);
    return 1;
}

int l_destroy_body(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    
    if (b->type_num == 2) {
    } else if (b->type_num == 3) {
	//dGeomHeightfieldDataDestroy(b->heightfield_data_id);
    }
    
    dBodyDestroy(b->body);
    dGeomDestroy(b->geom);
//    free(b->body);
//    free(b->geom);
    free(b);
    
    return 0;
}

int l_body_set_mass(void * L) {
    struct body_type *b = lua_touserdata(L, 1);
    
    b->mass.mass = lua_tonumber(L, 2);
    dBodySetMass(b->body, &b->mass);
    return 0;
}

int l_body_get_mass(void * L) {
    struct body_type *b = lua_touserdata(L, 1);
    
    dBodyGetMass(b->body, &b->mass);
    lua_pushnumber(L, (float)b->mass.mass);
    return 1;
}

int l_body_get_position(void * L) {
    struct body_type *b = lua_touserdata(L, 1);
    
    dReal * retval = dBodyGetPosition(b->body);
    lua_pushnumber(L, retval[0]);
    lua_pushnumber(L, retval[1]);    
    lua_pushnumber(L, retval[2]);    
    return 3;
}


int l_body_set_position(void * L) {
    struct body_type *b = lua_touserdata(L, 1);
    
    dBodySetPosition(b->body, lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    return 0;
}


int l_body_set_rotation(void * L) {
    struct body_type *b = lua_touserdata(L, 1);
    
//    dBodySetPosition(b->body, lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
//    dMatrix3 * rotation = dBodyGetRotation(b->body);
//    rotation[
    MATRIX_f m;
    QUAT q;
    dQuaternion q2;
//    get_rotation_matrix_f(&m, lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    get_rotation_matrix_f(&m, lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    matrix_to_quat(&m, &q);
    q2[0] = q.w;
    q2[1] = q.x;
    q2[2] = q.y;
    q2[3] = q.z;
    dBodySetQuaternion(b->body, q2);
    /*printf("Rotation: r0: %f, r1: %f, r2: %f \n", m.v[0][0], m.v[0][1], m.v[0][2]);
    printf("Rotation: r1: %f, r1: %f, r2: %f \n", m.v[1][0], m.v[1][1], m.v[1][2]);    
    printf("Rotation: r2: %f, r1: %f, r2: %f \n", m.v[2][0], m.v[2][1], m.v[2][2]);*/
    
//    dBodySetRotation(b->body, m);
    
    return 0;
}

// get_rotation_quaternion
int l_body_get_rotation(void * L) {
    struct body_type *b = lua_touserdata(L, 1);
    
    dReal * retval = dBodyGetQuaternion(b->body);
//    dMatrix3 * rotation = dBodyGetRotation(b->body);
/*    printf("Rotation: r0: %f, r1: %f, r2: %f \n", rotation[0], rotation[1], rotation[2]);
    printf("Rotation: r3: %f, r4: %f, r5: %f \n", rotation[3], rotation[4], rotation[5]);
    printf("Rotation: r6: %f, r7: %f, r8: %f \n", rotation[6], rotation[7], rotation[8]);*/
    //printf("Rotation: w: %f, x: %f, y: %f, z: %f\n", retval[0], retval[1], retval[2],  retval[3]);
    
    lua_pushnumber(L, retval[1]);
    lua_pushnumber(L, retval[0]);    
    lua_pushnumber(L, retval[2]);    
    lua_pushnumber(L, retval[3]);    
    return 4;
}

// old callback
void oldnearCallback(struct worldspace_type *ws, dGeomID o1, dGeomID o2) {
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    
    dContact contact;
    contact.surface.mode = dContactBounce | dContactSoftCFM;
    
    contact.surface.mu = dInfinity;
    
    contact.surface.bounce = 0.9;
    
    contact.surface.bounce_vel = 0.1;
    
    contact.surface.soft_cfm = .001;
    int numc = 0;
    if (numc = dCollide(o1,o2,1,&contact.geom, sizeof(dContact))) {
	dJointID c = dJointCreateContact(ws->world, ws->contactgroup, &contact);
	dJointAttach(c, b1, b2);
    }
}

// new callback
void nearCallback(struct worldspace_type *ws, dGeomID o1, dGeomID o2) {
    int i;
    int max_contacts = 3;
    
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    if (b1 && b2 && dAreConnectedExcluding (b1,b2,dJointTypeContact)) return;
    
    dContact contact[max_contacts];
    for (i = 0; i < max_contacts; i++) {
	contact[i].surface.mode = dContactBounce | dContactSoftCFM;
	contact[i].surface.mu = dInfinity;
	contact[i].surface.mu2 = 0;
	contact[i].surface.bounce = 0.09;
	contact[i].surface.bounce_vel = 0.01;
	contact[i].surface.soft_cfm = 0.001;
    }
    int numc = 0;
    if (numc = dCollide(o1,o2, max_contacts, &contact[0].geom, sizeof(dContact))) {
	dMatrix3 RI;
	dRSetIdentity (RI);
	const dReal ss[3] = {0.02, 0.02, 0.02};
	for(i = 0; i<numc; i++) {
	    dJointID c = dJointCreateContact(ws->world, ws->contactgroup, contact+i);
	    dJointAttach(c, b1, b2);
	}
    }
}
int l_simulate(void * L) {
    int n = lua_gettop(L);
    struct worldspace_type *ws = lua_touserdata(L, 1);
    dSpaceCollide(ws->space, ws, &nearCallback);
    if (n == 2) {
	dWorldQuickStep(ws->world, lua_tonumber(L, 2));
	//dWorldStep(ws->world, lua_tonumber(L, 2));
    } else {
	dWorldQuickStep(ws->world, timer() - ws->lasttime);
//	printf("%f \n", timer() - ws->lasttime);
	ws->lasttime = timer();
    }
    
    dJointGroupEmpty(ws->contactgroup);

    return 0;
}

int l_body_set_linear_velocity(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal fx = lua_tonumber(L, 2);
    dReal fy = lua_tonumber(L, 3);
    dReal fz = lua_tonumber(L, 4);
    
    dBodySetLinearVel(b->body, fx, fy, fz);
    
    return 0;
}

int l_body_set_angular_velocity(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal fx = lua_tonumber(L, 2);
    dReal fy = lua_tonumber(L, 3);
    dReal fz = lua_tonumber(L, 4);
    
    dBodySetAngularVel(b->body, fx, fy, fz);
    
    return 0;
}


int l_body_get_linear_velocity(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal * retval = NULL;
    
    retval = dBodyGetLinearVel(b->body);
    lua_pushnumber(L, retval[0]);
    lua_pushnumber(L, retval[1]);
    lua_pushnumber(L, retval[2]);
    
    return 3;
}

int l_body_get_angular_velocity(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal * retval = NULL;
    
    retval = dBodyGetAngularVel(b->body);
    lua_pushnumber(L, retval[0]);
    lua_pushnumber(L, retval[1]);
    lua_pushnumber(L, retval[2]);
    
    return 3;
}


int l_body_set_force(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal fx = lua_tonumber(L, 2);
    dReal fy = lua_tonumber(L, 3);
    dReal fz = lua_tonumber(L, 4);
    
    dBodySetForce(b->body, fx, fy, fz);
    
    return 0;
}

int l_body_add_force(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal fx = lua_tonumber(L, 2);
    dReal fy = lua_tonumber(L, 3);
    dReal fz = lua_tonumber(L, 4);
    
    dBodyAddForce(b->body, fx, fy, fz);
    
    return 0;
}

int l_body_add_torque(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal fx = lua_tonumber(L, 2);
    dReal fy = lua_tonumber(L, 3);
    dReal fz = lua_tonumber(L, 4);
    
    dBodyAddTorque(b->body, fx, fy, fz);
    
    return 0;
}


int l_body_add_relative_torque(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal fx = lua_tonumber(L, 2);
    dReal fy = lua_tonumber(L, 3);
    dReal fz = lua_tonumber(L, 4);
    
    dBodyAddTorque(b->body, fx, fy, fz);
    
    return 0;
}

int l_body_add_relative_force(void * L) {
    int n = lua_gettop(L);
    struct body_type *b = lua_touserdata(L, 1);
    dReal fx = lua_tonumber(L, 2);
    dReal fy = lua_tonumber(L, 3);
    dReal fz = lua_tonumber(L, 4);
    
    dBodyAddRelForce(b->body, fx, fy, fz);
    
    return 0;
}

int l_body_is_connected(void * L) {
    struct body_type *b1 = lua_touserdata(L, 1);
    struct body_type *b2 = lua_touserdata(L, 2);
    
    lua_pushnumber(L, dAreConnected(b1->body, b2->body));
    
    return 1;
}

int l_get_plane_depth(void * L) {
    dReal retval = 0;
    dGeomID plane = lua_touserdata(L, 1);
    dReal x = lua_tonumber(L, 2);
    dReal y = lua_tonumber(L, 3);
    dReal z = lua_tonumber(L, 4);
    
    retval = dGeomPlanePointDepth(plane, x, y, z);
    
    lua_pushnumber(L, retval);
    return 1;
}

void raycast_callback(void * data, dGeomID o1, dGeomID o2) {
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    dContact contact;
    printf("Inside Raycast Callback.\n");
//    if (numc = dCollide(o1,o2, max_contacts, &contact[0].geom, sizeof(dContact))) {
    if (dCollide(o1, o2, 1, &contact.geom, sizeof(dContact))) {
	printf("Raycast hit something!\n");
    }
}

int l_raycast(void * L) {
    dGeomID ray = NULL;
    struct worldspace_type *ws = lua_touserdata(L, 1);
    dReal length = lua_tonumber(L, 2);
    dReal x1 = lua_tonumber(L, 3);
    dReal y1 = lua_tonumber(L, 4);
    dReal z1 = lua_tonumber(L, 5);
    dReal x2 = lua_tonumber(L, 6);
    dReal y2 = lua_tonumber(L, 7);
    dReal z2 = lua_tonumber(L, 8);

    printf("\nRaycasting...\n");
    ray = dCreateRay(ws->space, length); 
    dGeomRaySet(ray, x1, y1, z1, x2, y2, z2); 
    dSpaceCollide(ws->space, ws, &raycast_callback); 
    dGeomDestroy(ray);
    printf("Raycast done\n");
    return 0;
}