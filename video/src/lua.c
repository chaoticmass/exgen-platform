
int l_stream_test(void * L) {
    int index = 0;
    int i = 0;
    char *file = lua_tostring(L, 1);
    int retval = 0;
    
    av_register_all();
    index = create_stream();
    
    printf("Attempting to open file: %s\n", file);
    retval = stream_open_file(index, file);
    if (retval == 0) {
	printf("Successfully opened file.\n");
	stream_get_info(index);
    
	for (i = 0; i < streams[index]->video_stream_count; i++) {
	    printf("Video Stream at Stream Index: %i\n", streams[index]->video_streams[i]);
	}
	for (i = 0; i < streams[index]->audio_stream_count; i++) {
	    printf("Audio Stream at Stream Index: %i\n", streams[index]->audio_streams[i]);
	}
	
	printf("Getting video codec...");
	retval = stream_open_video_codec(index, 0);
	if (retval == 0) {
	    printf("success.\n");
	    
	    printf("Initializing video buffer...");
	    retval = stream_initialize_video_buffer(index);
	    if (retval == 0) {
		printf("success.\n");
		
		i = 0;
		while (stream_read_frame(index)) {
		    printf("Read frame %i.\n", i);
		    i += 1;
		    stream_render_frame(index, 0);
		}
	    } else {
		printf("failure.\n");
	    }
	    	    
	} else {
	    printf("failure.\n");
	}
		
    }
    return 0;
}

int l_create_stream(void * L) {
    int index = 0;
    
    index = create_stream();
    
    lua_pushnumber(L, index);
    
    return 1;
}

int l_destroy_stream(void * L) {
    int index = 0;
    int retval = 0;
    
    index = lua_tonumber(L, 1);
    retval = destroy_stream(index);
    
    lua_pushnumber(L, retval);
    
    return 1;
}

int l_stream_open_file(void * L) {
    int index = 0;
    int retval = 0;
    char *file = NULL;
    
    index = lua_tonumber(L, 1);
    file = lua_tostring(L, 2);
    
    retval = stream_open_file(index, file);
    
    lua_pushnumber(L, retval);
    
    return 1;
}

int l_stream_get_info(void * L) {
    int index = lua_tonumber(L, 1);
    int retval = stream_get_info(index);
    
    lua_pushnumber(L, retval);
    
    return 1;
}

int l_stream_open_video_codec(void * L) {
    int index = lua_tonumber(L, 1);
    int video_index = lua_tonumber(L, 2);
    int retval = stream_open_video_codec(index, video_index);
    
    lua_pushnumber(L, retval);
    
    return 1;
}

int l_stream_initialize_video_buffer(void * L) {
    int index = lua_tonumber(L, 1);
    int retval = stream_initialize_video_buffer(index);
    
    lua_pushnumber(L, retval);
    
    return 1;
}

int l_stream_read_frame(void * L) {
    int index = lua_tonumber(L, 1);
    int retval = stream_read_frame(index);
    
    lua_pushnumber(L, retval);
    
    return 1;
}

int l_stream_render_frame(void * L) {
    int index = lua_tonumber(L, 1);
    BITMAP * bmap = lua_touserdata(L, 2);
    int retval = stream_render_frame(index, bmap);
    
    lua_pushnumber(L, retval);
    
    return 1;
}

int l_stream_get_width(void * L) {
    int index = lua_tonumber(L, 1);
    
    if (check_stream(index)) {
	lua_pushnumber(L, streams[index]->video_codec_context->width);
	return 1;
    } else {
	return 0;
    }
}

int l_stream_get_height(void * L) {
    int index = lua_tonumber(L, 1);
    
    if (check_stream(index)) {
	lua_pushnumber(L, streams[index]->video_codec_context->height);
	return 1;
    } else {
	return 0;
    }
}


void video_lua_register_call(void * L) {
//    lua_register(L, "blah", l_blah);
    lua_register(L, "stream_test", l_stream_test);
    lua_register(L, "create_stream", l_create_stream);
    lua_register(L, "destroy_stream", l_destroy_stream);
    lua_register(L, "stream_open_file", l_stream_open_file);
    lua_register(L, "stream_get_info", l_stream_get_info);
    lua_register(L, "stream_open_video_codec", l_stream_open_video_codec);
    lua_register(L, "stream_initialize_video_buffer", l_stream_initialize_video_buffer);
    lua_register(L, "stream_read_frame", l_stream_read_frame);
    lua_register(L, "stream_render_frame", l_stream_render_frame);
    lua_register(L, "stream_get_width", l_stream_get_width);
    lua_register(L, "stream_get_height", l_stream_get_height);
    
    
    
    
    av_register_all();
}

int _init_thread(char * mod_name) {
    mod_lua_register(mod_name, video_lua_register_call);
    SetVar(mod_name, "version", VERSION_STRING);
    SetVar(mod_name, "arch", ARCH_STRING);
    //test();
    return 0;
}