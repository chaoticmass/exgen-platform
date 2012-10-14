
void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
  FILE *pFile;
  char szFilename[32];
  int  y;
  
  // Open file
  sprintf(szFilename, "frame%d.ppm", iFrame);
  pFile=fopen(szFilename, "wb");
  if(pFile==NULL)
    return;
  
  // Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);
  
  // Write pixel data
  for(y=0; y<height; y++)
    fwrite(pFrame->data[0] + y * pFrame->linesize[0], 1, width*3, pFile);
  
  // Close file
  fclose(pFile);
}



int check_stream(int index) {
    if (index >= 0) {
	if (index < stream_count) {
	    if (streams[index] != NULL) {
		return -1;
	    }
	}
    }
}

int create_stream() {
    int i = 0;
    int index = -1;
    
    // Get the index of a free spot in the streams array
    for (i = 0; i < stream_count; i += 1) {
	if (streams[i] == NULL) {
	    index = i;
	    break;
	}
    }
    
    // if no free spot is found, increment stream_count and
    // reallocate the array, creating a new free spot
    if (index == -1) {
	stream_count += 1;
	struct stream ** tmp = calloc(stream_count, sizeof(struct surface *));
	for (i = 0; i < stream_count - 1; i += 1) {
	    tmp[i] = streams[i];
	}
	if (streams != NULL) {
	    free(streams);
	}
	index = stream_count - 1;
	streams = calloc(stream_count, sizeof(struct stream *));
	for (i = 0; i < stream_count - 1; i += 1) {
	    streams[i] = tmp[i];
	}
	free(tmp);
    }
    
    streams[index] = calloc(1, sizeof(struct stream));
    streams[index]->format_context = NULL;
    streams[index]->video_codec_context = NULL;
    streams[index]->audio_codec_context = NULL;
    streams[index]->video_codec = NULL;
    streams[index]->audio_codec = NULL;

    streams[index]->video_frame = NULL;
    streams[index]->rgb_video_frame = NULL;
    streams[index]->video_frame_finished = 0;
    streams[index]->video_num_bytes = 0;
    streams[index]->video_buffer = NULL;

    streams[index]->file = NULL;
    //streams[index]-> = NULL;
    streams[index]->video_streams = NULL;
    streams[index]->audio_streams = NULL;
    streams[index]->video_stream_count = 0;
    streams[index]->audio_stream_count = 0;
    streams[index]->video_stream = -1;
    streams[index]->audio_stream = -1;
    
    return index;        
}

int destroy_stream(int index) {
    int i = 0;
    int retval = 0;
    
    if (check_stream(index)) {
	free(streams[index]);
	streams[index] = NULL;
    } else {
	retval = -1;
    }
    return retval;
}

int stream_open_file(int index, char *file) {
    int retval = 0;
    
    if (check_stream(index)) {
	if (findfile(&file)) {
    	    retval = av_open_input_file(&streams[index]->format_context, file, NULL, 0, NULL);	    
	}
    }
    return retval;
}

int stream_get_info(int index) {
    int retval = 0;
    int i = 0;
    
    if (check_stream(index)) {
	retval = av_find_stream_info(streams[index]->format_context);
	if (retval < 0) {
	    retval = -1;
	} else {
	    if (streams[index]->video_streams != NULL) {
		free(streams[index]->video_streams);
	    }
	    if (streams[index]->audio_streams != NULL) {
		free(streams[index]->audio_streams);
	    }
	    streams[index]->video_streams = calloc(streams[index]->format_context->nb_streams, sizeof(int));
	    streams[index]->audio_streams = calloc(streams[index]->format_context->nb_streams, sizeof(int));
	    
	    for(i = 0; i < streams[index]->format_context->nb_streams; i++) {
		streams[index]->video_streams[i] = -1;
		streams[index]->audio_streams[i] = -1;
	    }
	    
	    for(i = 0; i < streams[index]->format_context->nb_streams; i++) {
		if (streams[index]->format_context->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO) {
		    streams[index]->video_streams[streams[index]->video_stream_count] = i;
		    streams[index]->video_stream_count += 1;
		}
		if (streams[index]->format_context->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO) {
		    streams[index]->audio_streams[streams[index]->audio_stream_count] = i;
		    streams[index]->audio_stream_count += 1;		    
		}
	    }
	}
    }
    return retval;
    
}

int stream_open_video_codec(index, video_index) {
    int retval = 0;
    int i = 0;
    int video_stream = 0;
    
    if (check_stream(index)) {
	video_stream = streams[index]->video_streams[video_index];
	streams[index]->video_codec_context = streams[index]->format_context->streams[video_stream]->codec;
	streams[index]->video_codec = avcodec_find_decoder(streams[index]->video_codec_context->codec_id);
	if (streams[index]->video_codec == NULL) {
	    retval = -1;
	} else {
	    if (avcodec_open(streams[index]->video_codec_context, streams[index]->video_codec) < 0) {
		retval = -1;
	    } else {
		streams[index]->video_stream = video_stream;
	    }
	}	
    }
    return retval;
}

int stream_initialize_video_buffer(index) {
    int retval = 0;
    int i = 0;
    
    if (check_stream(index)) {
	streams[index]->video_frame = avcodec_alloc_frame();
	streams[index]->rgb_video_frame = avcodec_alloc_frame();
	
	if (streams[index]->rgb_video_frame == NULL) {
	    retval = -1;
	} else {
	    streams[index]->video_num_bytes = avpicture_get_size(PIX_FMT_RGB24, streams[index]->video_codec_context->width, streams[index]->video_codec_context->height);
	    streams[index]->video_buffer = (uint8_t *)av_malloc(streams[index]->video_num_bytes * sizeof(uint8_t));
	    
	    avpicture_fill((AVPicture *)streams[index]->rgb_video_frame, streams[index]->video_buffer, PIX_FMT_RGB24, streams[index]->video_codec_context->width, streams[index]->video_codec_context->height);	    
	}
    }
    return retval;
}

int stream_read_frame(index) {
    int i = 0;
    int retval = 0;
    AVCodecContext *pCodecCtx;
    AVFrame *pFrame;
    AVFrame *pFrameRGB;
    
    
    if (check_stream(index)) {
	if (av_read_frame(streams[index]->format_context, &streams[index]->packet) >= 0) {
	    if (streams[index]->packet.stream_index == streams[index]->video_stream) {
		avcodec_decode_video(streams[index]->video_codec_context, streams[index]->video_frame, &streams[index]->video_frame_finished, streams[index]->packet.data, streams[index]->packet.size);
		
		if (streams[index]->video_frame_finished) {
		    //img_convert((AVPicture *)streams[index]->rgb_video_frame, PIX_FMT_RGB24, (AVPicture *)streams[index]->video_frame, streams[index]->video_codec_context->pix_fmt, streams[index]->video_codec_context->width, streams[index]->video_codec_context->height);
		    
		    //static struct SwsContext *img_convert_ctx;
		    
		    // Convert the image into YUV format
		    if(img_convert_ctx == NULL) {
			int w = pCodecCtx->width;
			int h = pCodecCtx->height;
			
			img_convert_ctx = sws_getContext(w, h,
							pCodecCtx->pix_fmt,
							w, h, PIX_FMT_RGB24, SWS_BICUBIC,
							NULL, NULL, NULL);
			if (img_convert_ctx == NULL) {
			    // fail
			}
		    }
		    int ret = sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, 
					pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
		}
	    } else if (streams[index]->packet.stream_index == streams[index]->audio_stream) {
		
	    } else {
		av_free_packet(&streams[index]->packet);
	    }
	    retval = -1;
	}
    }
    
    return retval;
}

int stream_render_frame(int index, BITMAP * bmap) {
    int retval = 0;
    int i = 0;
    int x = 0;
    int y = 0;
    int r = 0;
    int g = 0;
    int b = 0;
    int p = 0;
    int c = 0;
    
    if (check_stream(index)) {
	for (y = 0; y < streams[index]->video_codec_context->height; y++) {
	    for (x = 0; x < streams[index]->video_codec_context->width; x++) {
		p = x * 3 + y * streams[index]->rgb_video_frame->linesize[0];
		r = streams[index]->rgb_video_frame->data[0][p];
		g = streams[index]->rgb_video_frame->data[0][p+1];
		b = streams[index]->rgb_video_frame->data[0][p+2];
		c = makecol(r, g, b);
		putpixel(bmap, x, y, c);
	    }
	}
    }
    
    return retval;
}