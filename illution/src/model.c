
long filelength(int f) {
    struct stat buf;
    fstat(f, &buf);
    return(buf.st_size);
}

int load3ds (char *p_filename, struct model_type * model)
{
    int i; //Index variable
	
    FILE *l_file; //File pointer
    float *ptmp;
    float tmpval = 0;
    ptmp = &tmpval;
    unsigned short l_chunk_id; //Chunk identifier
    unsigned int l_chunk_lenght; //Chunk lenght
    unsigned char l_char; //Char variable
    unsigned short l_qty; //Number of elements in each chunk
    unsigned short l_face_flags; //Flag that stores some face information
    unsigned short tmpf;
    unsigned short vcount;
    float fcount = 0;
    int index = 0;
    V3D_f *v = NULL;
    int *map = NULL;
//    float conv = 1 / 512;

    if ((l_file=fopen (p_filename, "rb"))== NULL) return 0; //Open the file
    while (ftell(l_file) < filelength(fileno(l_file))) {
	fread (&l_chunk_id, 2, 1, l_file); //Read the chunk header
	////printf("ChunkID: %x\n",l_chunk_id); 
	
	fread (&l_chunk_lenght, 4, 1, l_file); //Read the lenght of the chunk
	
	switch (l_chunk_id) {
			//----------------- MAIN3DS -----------------
			// Description: Main chunk, contains all the other chunks
			// Chunk ID: 4d4d 
			// Chunk Lenght: 0 + sub chunks
			//-------------------------------------------
	case 0x4d4d: break;    
			//----------------- EDIT3DS -----------------
			// Description: 3D Editor chunk, objects layout info 
			// Chunk ID: 3d3d (hex)
			// Chunk Lenght: 0 + sub chunks
			//-------------------------------------------
	case 0x3d3d: break;			
			//--------------- EDIT_OBJECT ---------------
			// Description: Object block, info for each object
			// Chunk ID: 4000 (hex)
			// Chunk Lenght: len(object name) + sub chunks
			//-------------------------------------------
	case 0x4000: 
	    i=0;
	    do {
		fread (&l_char, 1, 1, l_file);
                //p_object->name[i]=l_char;
		i++;
            } while(l_char != '\0' && i<20);
    	break;
			//--------------- OBJ_TRIMESH ---------------
			// Description: Triangular mesh, contains chunks for 3d mesh info
			// Chunk ID: 4100 (hex)
			// Chunk Lenght: 0 + sub chunks
			//-------------------------------------------
	case 0x4100: break;			
			//--------------- TRI_VERTEXL ---------------
			// Description: Vertices list
			// Chunk ID: 4110 (hex)
			// Chunk Lenght: 1 x unsigned short (number of vertices) 
			//             + 3 x float (vertex coordinates) x (number of vertices)
			//             + sub chunks
			//-------------------------------------------
	case 0x4110: 
	    //printf("VERTICES : %x\n",l_chunk_id); 
    	    fread (&l_qty, sizeof (unsigned short), 1, l_file);
            //vertices_qty = l_qty;
	    vcount = l_qty;
	    if (v != NULL) {
		free(v);
		free(map);
	    }
	    v = calloc(l_qty, sizeof(V3D_f));
	    map = calloc(l_qty, sizeof(int));
            //printf("Number of vertices: %d\n",l_qty);
            for (i=0; i<l_qty; i++) {
		fread (&v[i].x, sizeof(float), 1, l_file);
    		fread (&v[i].z, sizeof(float), 1, l_file);
		fread (&v[i].y, sizeof(float), 1, l_file);
		//v[i].y = 0 - v[i].y;
	    }
	break;

			//--------------- TRI_FACEL1 ----------------
			// Description: Polygons (faces) list
			// Chunk ID: 4120 (hex)
			// Chunk Lenght: 1 x unsigned short (number of polygons) 
			//             + 3 x unsigned short (polygon points) x (number of polygons)
			//             + sub chunks
			//-------------------------------------------
	case 0x4120:
	    //printf("POLYGONS: %x\n",l_chunk_id); 
	    fread (&l_qty, sizeof (unsigned short), 1, l_file);
            //polygons_qty = l_qty;
            //printf("Number of polygons: %d\n",l_qty); 
            for (i=0; i<l_qty; i++) {
		addface(model);
		index = fcount + i;
		fread (&tmpf, sizeof (unsigned short), 1, l_file);
		model->faces[index]->v[0].x = v[tmpf].x;
		model->faces[index]->v[0].y = v[tmpf].y;
		model->faces[index]->v[0].z = v[tmpf].z;
		model->faces[index]->v[0].u = v[tmpf].u;
		model->faces[index]->v[0].v = v[tmpf].v;
		fread (&tmpf, sizeof (unsigned short), 1, l_file);
		model->faces[index]->v[1].x = v[tmpf].x;
		model->faces[index]->v[1].y = v[tmpf].y;
		model->faces[index]->v[1].z = v[tmpf].z;
		model->faces[index]->v[1].u = v[tmpf].u;
		model->faces[index]->v[1].v = v[tmpf].v;
		fread (&tmpf, sizeof (unsigned short), 1, l_file);
		model->faces[index]->v[2].x = v[tmpf].x;
		model->faces[index]->v[2].y = v[tmpf].y;
		model->faces[index]->v[2].z = v[tmpf].z;
		model->faces[index]->v[2].u = v[tmpf].u;
		model->faces[index]->v[2].v = v[tmpf].v;
		fread (&tmpf, sizeof (unsigned short), 1, l_file);
		
	    }
	    fcount += l_qty;
    	    break;

			//------------- TRI_MAPPINGCOORS ------------
			// Description: Vertices list
			// Chunk ID: 4140 (hex)
			// Chunk Lenght: 1 x unsigned short (number of mapping points) 
			//             + 2 x float (mapping coordinates) x (number of mapping points)
			//             + sub chunks
			//-------------------------------------------
	case 0x4140:
	    //printf("MAPPING: %x\n",l_chunk_id); 
	    //printf("Number of mapping: %d\n",l_qty);
	    fread (&l_qty, sizeof (unsigned short), 1, l_file);
	    for (i=0; i<l_qty; i++) {		
		fread (&tmpval, sizeof (float), 1, l_file);
		tmpval *= 256;
		v[i].v = tmpval;
		//allegro_message("%f\n", tmpval);
                fread (&tmpval, sizeof (float), 1, l_file);
		tmpval *= 256;
		v[i].u = tmpval;
		//allegro_message("%f\n", tmpval);
	    }
        break;

			//----------- Skip unknow chunks ------------
			//We need to skip all the chunks that currently we don't use
			//We use the chunk lenght information to set the file pointer
			//to the same level next chunk
			//-------------------------------------------
	default:
	    fseek(l_file, l_chunk_lenght-6, SEEK_CUR);
        } 
    }
//    allegro_message("vcount: %i\n", vcount);
    fclose(l_file); // Closes the file stream
    return 1; // Returns ok
}


/*
int renderhudentity(int entityindex) {
    int i = 0;
    int index = 0;
    int f = 0;
    int e = 0;
    float tmp;
    int facecount = 0;
    int selected = 0;
    float ax = 0;
    float ay = 0;
    float az = 0;
    int area = 0;
    MATRIX_f matrix;
    
    area = entities[playerentity]->area;
    
    V3D_f *v[4];
    V3D_f vout[4];
    index = entityindex;
	i = entities[index]->model;
	//entities[index]->scale
	get_transformation_matrix_f(&matrix, 1,
	    entities[index]->rx + models[i]->rx, entities[index]->ry + models[i]->ry, entities[index]->rz + models[i]->rz,
	    //entities[index]->x, entities[index]->y, entities[index]->z);
	    0, 0, 400);
	
	facecount = models[i]->facecount;    
	//if (facecount > 1) {
	//    struct mquad **faces;
	//    faces = models[i]->faces;
	//    //qsort(faces, facecount, sizeof(struct mquad **), model_qsort_cmp);
	//}
	for (f = 0; f < facecount; f += 1) {
	    for (e = 0; e < 3; e += 1) {
		vout[e].x = models[i]->faces[f]->v[e].x;
		vout[e].y = models[i]->faces[f]->v[e].y;
		vout[e].z = models[i]->faces[f]->v[e].z;
		vout[e].u = models[i]->faces[f]->v[e].u;
		vout[e].v = models[i]->faces[f]->v[e].v;
		apply_matrix_f(&matrix, vout[e].x, vout[e].y, vout[e].z, &vout[e].x, &vout[e].y, &vout[e].z);
	    }
	    drawquad4(&hudcamera, &vout[0], &vout[1], &vout[2], entities[index]->texture);
	}
}

*/
/*
int createmodel(void) {
    modelcount += 1;
//    struct model ** tmpmodels = calloc(modelcount, sizeof(struct model));
    struct model ** tmpmodels = calloc(modelcount, sizeof(struct model *));
    int i = 0;
    for (i = 0; i < modelcount - 1; i += 1) {
	tmpmodels[i] = models[i];
    }
    if (models != NULL) {
	free(models);
    }
    models = calloc(modelcount, sizeof(struct model *));
    //models[modelcount - 1] = malloc(sizeof(struct model));
    models[modelcount - 1] = calloc(1, sizeof(struct model));
    models[modelcount - 1]->facecount = 0;
    for (i = 0; i < modelcount - 1; i += 1) {
	models[i] = tmpmodels[i];
    }    
    free(tmpmodels);
}
*/

int addface(struct model_type * model) {
    int facecount = 0;
    facecount = model->facecount;
    facecount += 1;
//    struct mquad ** tmpfaces = calloc(facecount, sizeof(struct mquad ));  // why does this work?
    struct mquad ** tmpfaces = calloc(facecount, sizeof(struct mquad *));
    int i = 0;
    for (i = 0; i < facecount - 1; i += 1) {
	tmpfaces[i] = model->faces[i];
    }
    if (facecount - 1 > 0) {
	free(model->faces);
    }
    model->faces = calloc(facecount, sizeof(struct mquad *));
//    model->faces[facecount - 1] = malloc(sizeof(struct mquad));
    model->faces[facecount - 1] = calloc(1, sizeof(struct mquad));
    model->faces[facecount - 1]->v[0].x = 0;
    model->faces[facecount - 1]->v[0].y = 64;
    model->faces[facecount - 1]->v[0].z = 0;
    model->faces[facecount - 1]->v[0].c = makecol(128, 0, 0);

    model->faces[facecount - 1]->v[1].x = 64;
    model->faces[facecount - 1]->v[1].y = 64;
    model->faces[facecount - 1]->v[1].z = 0;
    model->faces[facecount - 1]->v[1].c = makecol(0, 128, 0);

    model->faces[facecount - 1]->v[2].x = 64;
    model->faces[facecount - 1]->v[2].y = 64;
    model->faces[facecount - 1]->v[2].z = 64;
    model->faces[facecount - 1]->v[2].c = makecol(0, 0, 128);

    model->faces[facecount - 1]->v[3].x = 0;
    model->faces[facecount - 1]->v[3].y = 64;
    model->faces[facecount - 1]->v[3].z = 64;
    model->faces[facecount - 1]->v[3].c = makecol(0, 0, 0);

    for (i = 0; i < facecount - 1; i += 1) {
	model->faces[i] = tmpfaces[i];
    }    
    model->facecount = facecount;
}

