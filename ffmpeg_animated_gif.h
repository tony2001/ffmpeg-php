#ifndef FFMPEG_ANIMATED_GIF_H
#define FFMPEG_ANIMATED_GIF_H

#include <avcodec.h>
#include <avformat.h>

/* animated gif constructor */
PHP_FUNCTION(ffmpeg_animated_gif);

/* animated gif methods */
PHP_FUNCTION(addFrame);

typedef struct {
    AVStream *video_st;
    AVOutputFormat *fmt;
    AVFormatContext *fmt_ctx;
    AVCodecContext *codec_ctx;

    uint8_t *video_outbuf;
    int video_outbuf_size;
} ff_animated_gif_context;

#endif // FFMPEG_ANIMATED_GIF_H
