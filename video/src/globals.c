#define VERSION_STRING "0.01A"
#define ARCH_STRING "linux_x86"

// #include <ffmpeg/avcodec.h>
// #include <ffmpeg/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>


#include "lua.h"
#include "allegro.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
struct surface {
    int index;
    float x;
    float y;
    float y_offset;
    float x_offset;
    int position_absolute;
    int z;
    int z2;
    int dirty;
    int parent;
    int forecolor;
    int backcolor;
    int transcolor;
    int transparent;
    int visible;
    float alpha;
    int rotation;
    int always_on_top;
    int has_focus;
    int can_have_focus;
    int destroy;
    
    FONT *font;
    BITMAP *bmap;
    
    BITMAP **frames;
    int **durations;
    int frame_count;
    double frame_timer;
    int current_frame;
    
    int noevents;
    int tabindex;
    
    int on_draw;
    int on_got_focus;
    int on_lost_focus;
    int on_keypress;
    int 
}
*/

struct stream {
    AVFormatContext *format_context;
    AVCodecContext *video_codec_context;
    AVCodecContext *audio_codec_context;
    AVCodec *video_codec;
    AVCodec *audio_codec;

    AVPacket packet;

    AVFrame *video_frame;
    AVFrame *rgb_video_frame;    
    int video_frame_finished;
    int video_num_bytes;
    uint8_t *video_buffer;

    char *file;
    int video_stream_count;
    int audio_stream_count;
    int *video_streams;
    int *audio_streams;
    int video_stream;
    int audio_stream;
};

struct stream **streams = NULL;
int stream_count = 0;

struct SwsContext *img_convert_ctx = NULL;
