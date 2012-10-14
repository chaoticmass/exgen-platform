

int draw_polygon(struct camera_type *camera, V3D_f *v1, V3D_f *v2, V3D_f *v3, BITMAP *texture_bmp, BITMAP *target) {
    MATRIX_f *cam = &camera->camera;
    V3D_f _vout[8], _vtmp[8];
    V3D_f *v[4], *vout[8], *vtmp[8];
    int flags[4], out[8];
    int c, vc;

    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
    for (c = 0; c < 8; c += 1) {
	vout[c] = &_vout[c];
	vtmp[c] = &_vtmp[c];
    }

    for (c = 0; c < 3; c += 1) {
	apply_matrix_f(cam, v[c]->x, v[c]->y, v[c]->z, 
			    &v[c]->x, &v[c]->y, &v[c]->z);
	flags[c] = 0;
	if (v[c]->x < -v[c]->z) {
	    flags[c] |= 1;
	} else if (v[c]->x > v[c]->z) {
	    flags[c] |= 2;
	} 
	
	if (v[c]->y < -v[c]->z) {
	    flags[c] |= 4;
	} else if (v[c]->y > v[c]->z) {
	    flags[c] |= 8;
	}
	if (v[c]->z < 0.1) {
	    flags[c] |= 16;
	}
	
    }
    if (flags[0] & flags[1] & flags[2]) {
	//printf("A exited early\n");
	return 0;
    }

    if (flags[0] | flags[1] | flags[2]) {
	vc = clip3d_f(POLYTYPE_PTEX , 0.1, 0.1, 3, (AL_CONST V3D_f **)v, vout, vtmp, out);
	if (vc <= 0) {
	    //printf("B exited early\n");
	    return 1;
	}	
    } else {
	vout[0] = v[0];
	vout[1] = v[1];	
	vout[2] = v[2];
	
	vc = 3;
    }

    for (c = 0; c < vc; c += 1) {
	persp_project_f(vout[c]->x, vout[c]->y, vout[c]->z,
			&vout[c]->x, &vout[c]->y);
	
    }
    polygon3d_f(target, POLYTYPE_PTEX | POLYTYPE_ZBUF, texture_bmp, vc, vout);
    return 0;
}

int draw_polygon_gouraud(struct camera_type *camera, V3D_f *v1, V3D_f *v2, V3D_f *v3, BITMAP *target) {
    MATRIX_f *cam = &camera->camera;
    V3D_f _vout[8], _vtmp[8];
    V3D_f *v[4], *vout[8], *vtmp[8];
    int flags[4], out[8];
    int c, vc;

    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
    for (c = 0; c < 8; c += 1) {
	vout[c] = &_vout[c];
	vtmp[c] = &_vtmp[c];
    }

    for (c = 0; c < 3; c += 1) {
	apply_matrix_f(cam, v[c]->x, v[c]->y, v[c]->z, 
			    &v[c]->x, &v[c]->y, &v[c]->z);
	flags[c] = 0;
	if (v[c]->x < -v[c]->z) {
	    flags[c] |= 1;
	} else if (v[c]->x > v[c]->z) {
	    flags[c] |= 2;
	} 
	
	if (v[c]->y < -v[c]->z) {
	    flags[c] |= 4;
	} else if (v[c]->y > v[c]->z) {
	    flags[c] |= 8;
	}
	if (v[c]->z < 0.1) {
	    flags[c] |= 16;
	}
	
    }
    if (flags[0] & flags[1] & flags[2]) {
	//printf("A exited early\n");
	return 0;
    }

    if (flags[0] | flags[1] | flags[2]) {
	vc = clip3d_f(POLYTYPE_GCOL , 0.1, 0.1, 3, (AL_CONST V3D_f **)v, vout, vtmp, out);
	if (vc <= 0) {
	    //printf("B exited early\n");
	    return 1;
	}	
    } else {
	vout[0] = v[0];
	vout[1] = v[1];	
	vout[2] = v[2];
	
	vc = 3;
    }

    for (c = 0; c < vc; c += 1) {
	persp_project_f(vout[c]->x, vout[c]->y, vout[c]->z,
			&vout[c]->x, &vout[c]->y);
	
    }
    polygon3d_f(target, POLYTYPE_GCOL | POLYTYPE_ZBUF, NULL, vc, vout);
    return 0;
}


int draw_polygon_wireframe(struct camera_type *camera, V3D_f *v1, V3D_f *v2, V3D_f *v3, BITMAP *target) {
    MATRIX_f *cam = &camera->camera;
    V3D_f _vout[8], _vtmp[8];
    V3D_f *v[4], *vout[8], *vtmp[8];
    int flags[4], out[8];
    int c, vc;

    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
    for (c = 0; c < 8; c += 1) {
	vout[c] = &_vout[c];
	vtmp[c] = &_vtmp[c];
    }

    for (c = 0; c < 3; c += 1) {
	apply_matrix_f(cam, v[c]->x, v[c]->y, v[c]->z, 
			    &v[c]->x, &v[c]->y, &v[c]->z);
	flags[c] = 0;
	if (v[c]->x < -v[c]->z) {
	    flags[c] |= 1;
	} else if (v[c]->x > v[c]->z) {
	    flags[c] |= 2;
	} 
	
	if (v[c]->y < -v[c]->z) {
	    flags[c] |= 4;
	} else if (v[c]->y > v[c]->z) {
	    flags[c] |= 8;
	}
	if (v[c]->z < 0.1) {
	    flags[c] |= 16;
	}
	
    }
    if (flags[0] & flags[1] & flags[2]) {
	//printf("A exited early\n");
	return 0;
    }

    if (flags[0] | flags[1] | flags[2]) {
	vc = clip3d_f(POLYTYPE_GCOL , 0.1, 0.1, 3, (AL_CONST V3D_f **)v, vout, vtmp, out);
	if (vc <= 0) {
	    //printf("B exited early\n");
	    return 1;
	}	
    } else {
	vout[0] = v[0];
	vout[1] = v[1];	
	vout[2] = v[2];
	
	vc = 3;
    }

    for (c = 0; c < vc; c += 1) {
	persp_project_f(vout[c]->x, vout[c]->y, vout[c]->z,
			&vout[c]->x, &vout[c]->y);
	
	if (c > 0) {
	    fastline(target, vout[c-1]->x, vout[c-1]->y, vout[c]->x, vout[c]->y, vout[c]->c);
	}
    }
    //polygon3d_f(target, POLYTYPE_GCOL | POLYTYPE_ZBUF, NULL, vc, vout);
    return 0;
}

int plot_polygon(struct camera_type *camera, V3D_f *v1, V3D_f *v2, V3D_f *v3, BITMAP *target) {
    MATRIX_f *cam = &camera->camera;
    V3D_f _vout[8], _vtmp[8];
    V3D_f *v[4], *vout[8], *vtmp[8];
    int flags[4], out[8];
    int c, vc;

    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
    for (c = 0; c < 8; c += 1) {
	vout[c] = &_vout[c];
	vtmp[c] = &_vtmp[c];
    }

    for (c = 0; c < 3; c += 1) {
	apply_matrix_f(cam, v[c]->x, v[c]->y, v[c]->z, 
			    &v[c]->x, &v[c]->y, &v[c]->z);
	flags[c] = 0;
	if (v[c]->x < -v[c]->z) {
	    flags[c] |= 1;
	} else if (v[c]->x > v[c]->z) {
	    flags[c] |= 2;
	} 
	
	if (v[c]->y < -v[c]->z) {
	    flags[c] |= 4;
	} else if (v[c]->y > v[c]->z) {
	    flags[c] |= 8;
	}
	if (v[c]->z < 0.1) {
	    flags[c] |= 16;
	}
	
    }
    if (flags[0] & flags[1] & flags[2]) {
	//printf("A exited early\n");
	return 0;
    }

    if (flags[0] | flags[1] | flags[2]) {
	vc = clip3d_f(POLYTYPE_GCOL , 0.1, 0.1, 3, (AL_CONST V3D_f **)v, vout, vtmp, out);
	if (vc <= 0) {
	    //printf("B exited early\n");
	    return 1;
	}	
    } else {
	vout[0] = v[0];
	vout[1] = v[1];	
	vout[2] = v[2];
	
	vc = 3;
    }

    for (c = 0; c < vc; c += 1) {
	persp_project_f(vout[c]->x, vout[c]->y, vout[c]->z,
			&vout[c]->x, &vout[c]->y);
	
	printf("X: %f, Y: %f, Z: %f\n", vout[c]->x, vout[c]->y, vout[c]->z);
    }
    //polygon3d_f(target, POLYTYPE_GCOL | POLYTYPE_ZBUF, NULL, vc, vout);
    return 0;
}

