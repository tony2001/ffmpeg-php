#ifndef FFMPEG_FRAME_H
#define FFMPEG_FRAME_H

#include <avcodec.h>
#include <avformat.h>

PHP_FUNCTION(ffmpeg_frame);
PHP_FUNCTION(getWidth);
PHP_FUNCTION(getHeight);
PHP_FUNCTION(resize);
PHP_FUNCTION(crop);
PHP_FUNCTION(isKeyFrame);
PHP_FUNCTION(getPresentationTimestamp);

#if HAVE_LIBGD20
PHP_FUNCTION(toGDImage);
#endif // HAVE_LIBGD20

typedef struct {
    AVFrame *av_frame;
    int width;
    int height;
    int pixel_format;
    int keyframe;
} ff_frame_context;

ff_frame_context* _php_create_ffmpeg_frame(INTERNAL_FUNCTION_PARAMETERS);

#define GET_FRAME_RESOURCE(ffmpeg_frame_object, ffmpeg_frame) {\
	zval **_tmp_zval;\
    if (zend_hash_find(Z_OBJPROP_P(ffmpeg_frame_object), "ffmpeg_frame",\
                sizeof("ffmpeg_frame"), (void **)&_tmp_zval) == FAILURE) {\
        zend_error(E_ERROR, "Unable to locate ffmpeg_frame resource in this object.");\
        RETURN_FALSE;\
    }\
\
    ZEND_FETCH_RESOURCE(ffmpeg_frame, ff_frame_context*, _tmp_zval, -1,\
            "ffmpeg_frame", le_ffmpeg_frame);\
}\

#endif // FFMPEG_FRAME_H
