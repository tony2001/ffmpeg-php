#ifndef FFMPEG_MOVIE_H
#define FFMPEG_MOVIE_H

#include <avcodec.h>
#include <avformat.h>

/* movie constructor */
PHP_FUNCTION(ffmpeg_movie);

/* movie methods */
PHP_FUNCTION(getDuration);
PHP_FUNCTION(getFrameCount);
PHP_FUNCTION(getFrameRate);
PHP_FUNCTION(getFileName);
PHP_FUNCTION(getComment);
PHP_FUNCTION(getTitle);
PHP_FUNCTION(getAuthor);
PHP_FUNCTION(getCopyright);
PHP_FUNCTION(getFrameWidth);
PHP_FUNCTION(getFrameHeight);
PHP_FUNCTION(getFrameNumber);
PHP_FUNCTION(getPixelFormat);
PHP_FUNCTION(getBitRate);
PHP_FUNCTION(hasAudio);
PHP_FUNCTION(getFrame);

typedef struct {
    AVFormatContext *fmt_ctx;
    AVCodecContext *codec_ctx;
} ff_movie_context;

#define GET_MOVIE_RESOURCE(ff_movie_ctx) {\
	zval **_tmp_zval;\
    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmpeg_movie",\
                sizeof("ffmpeg_movie"), (void **)&_tmp_zval) == FAILURE) {\
        zend_error(E_ERROR, "Unable to find ffmpeg_movie property");\
        RETURN_FALSE;\
    }\
\
    ZEND_FETCH_RESOURCE(ff_movie_ctx, ff_movie_context*, _tmp_zval, -1,\
            "ffmpeg_movie", le_ffmpeg_movie);\
}\

#endif // FFMPEG_MOVIE_H
