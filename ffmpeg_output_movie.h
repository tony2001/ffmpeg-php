#ifndef FFMPEG_MOVIE_H
#define FFMPEG_MOVIE_H

#include <avcodec.h>
#include <avformat.h>

/* movie constructor */
PHP_FUNCTION(ffmpeg_output_movie);

/* movie methods */
PHP_FUNCTION(setFrameRate);
PHP_FUNCTION(setComment);
PHP_FUNCTION(setTitle);
PHP_FUNCTION(setAuthor);
PHP_FUNCTION(setCopyright);
PHP_FUNCTION(setFrameWidth);
PHP_FUNCTION(setFrameHeight);
PHP_FUNCTION(setPixelFormat);
PHP_FUNCTION(setBitRate);
PHP_FUNCTION(addFrame);

typedef struct {
    AVFormatContext *fmt_ctx;
    AVCodecContext *codec_ctx;
} ff_output_movie_context;

#endif // FFMPEG_MOVIE_H
