#ifndef FFMPEG_FRAME_H
#define FFMPEG_FRAME_H

#include <avcodec.h>
#include <avformat.h>

PHP_FUNCTION(ffmpeg_frame);
PHP_FUNCTION(getWidth);
PHP_FUNCTION(getHeight);
PHP_FUNCTION(resize);
PHP_FUNCTION(crop);
PHP_FUNCTION(toGDImage);

typedef struct {
    AVFrame *av_frame;
    int width;
    int height;
    int pixel_format;
} ff_frame_context;

ff_frame_context* _php_create_ffmpeg_frame(INTERNAL_FUNCTION_PARAMETERS);

#define GET_FRAME_RESOURCE(ffmpeg_frame) {\
	zval **_tmp_zval;\
    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmpeg_frame",\
                sizeof("ffmpeg_frame"), (void **)&_tmp_zval) == FAILURE) {\
        zend_error(E_ERROR, "Unable to find ffmpeg_frame property");\
        RETURN_FALSE;\
    }\
\
    ZEND_FETCH_RESOURCE(ffmpeg_frame, ff_frame_context*, _tmp_zval, -1,\
            "ffmpeg_frame", le_ffmpeg_frame);\
}\

#endif // FFMPEG_FRAME_H
