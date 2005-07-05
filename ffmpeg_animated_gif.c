#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"

#include "php_ffmpeg.h"

#include "ffmpeg_frame.h"
#include "ffmpeg_animated_gif.h"

#define GET_ANIMATED_GIF_RESOURCE(ff_animated_gif_ctx) {\
	zval **_tmp_zval;\
    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmpeg_animated_gif",\
                sizeof("ffmpeg_animated_gif"), (void **)&_tmp_zval) == FAILURE) {\
        zend_error(E_ERROR, "Unable to find ffmpeg_animated_gif property");\
        RETURN_FALSE;\
    }\
\
    ZEND_FETCH_RESOURCE(ff_animated_gif_ctx, ff_animated_gif_context*, _tmp_zval, -1,\
            "ffmpeg_animated_gif", le_ffmpeg_animated_gif);\
}\

static zend_class_entry *ffmpeg_animated_gif_class_entry_ptr;
zend_class_entry ffmpeg_animated_gif_class_entry;

extern int le_ffmpeg_frame;
static int le_ffmpeg_animated_gif;

/* {{{ ffmpeg_animated_gif methods[]
    Methods of the ffmpeg_animated_gif class 
*/
zend_function_entry ffmpeg_animated_gif_class_methods[] = {
   
    /* contructor */
    PHP_FE(ffmpeg_animated_gif,     NULL)

    /* methods */
    PHP_FALIAS(addframe,            addFrame,           NULL)
	{NULL, NULL, NULL}
};
/* }}} */


/* {{{ _php_alloc_ff_animated_gif()
 */
static ff_animated_gif_context* _php_alloc_ff_animated_gif()
{
    ff_animated_gif_context *ff_animated_gif;
    
    ff_animated_gif = emalloc(sizeof(ff_animated_gif_context));
    ff_animated_gif->fmt = guess_format("gif", NULL, NULL);
    
    ff_animated_gif->fmt_ctx = av_alloc_format_context();
    if (!ff_animated_gif->fmt_ctx) {
        zend_error(E_ERROR, "Error allocating format context\n");
    }
    ff_animated_gif->fmt_ctx->oformat = ff_animated_gif->fmt;
    
    ff_animated_gif->codec_ctx = NULL;
    return ff_animated_gif;
}
/* }}} */


/* {{{ _php_add_video_stream()
 */
AVStream * _php_add_video_stream(AVFormatContext *oc, int codec_id, int width, 
        int height, int frame_rate, int loop_count)
{
    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(oc, 0);
    if (!st) {
        zend_error(E_ERROR, "could not alloc stream\n");
    }

#if LIBAVFORMAT_BUILD > 4626
    oc->loop_output = loop_count;
#endif
    
    c = &st->codec;
    c->codec_id = CODEC_ID_RAWVIDEO;
    c->codec_type = CODEC_TYPE_VIDEO;
    c->pix_fmt = PIX_FMT_RGB24;

    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = width;
    c->height = height;

    /* frames per second */
#if LIBAVCODEC_BUILD > 4753
    c->time_base.den = frame_rate;
    c->time_base.num = 1;
#else
	c->frame_rate = frame_rate;
	c->frame_rate_base = 1;
#endif

    /* probably doesn't matter for animated gif */
    c->gop_size = 12; /* emit one intra frame every twelve frames at most */

    return st;
}
/* }}} */


/* {{{ _php_open_movie_file()
 */
static void _php_open_movie_file(ff_animated_gif_context *ff_animated_gif, 
        char* filename)
{
    AVCodec *codec;
    AVCodecContext *c = &ff_animated_gif->video_st->codec;

    snprintf(ff_animated_gif->fmt_ctx->filename, 
            sizeof(ff_animated_gif->fmt_ctx->filename), "%s", filename);

    /* find the video encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        zend_error(E_ERROR, "codec not found\n");
    }

    /* open the codec */
    if (avcodec_open(c, codec) < 0) {
        zend_error(E_ERROR, "could not open codec\n");
    }

    /* allocate output buffer */
    /* TODO: This should computed based on width / height */
    ff_animated_gif->video_outbuf_size = 200000;
    ff_animated_gif->video_outbuf = malloc(ff_animated_gif->video_outbuf_size);

    /* open the output file, if needed */
    if (!(ff_animated_gif->fmt->flags & AVFMT_NOFILE)) {
        if (url_fopen(&ff_animated_gif->fmt_ctx->pb, filename, URL_WRONLY) < 0) {
            zend_error(E_ERROR, "Could not open '%s'\n", filename);
        }
    }
    
    /* write the stream header, if any */
    av_write_header(ff_animated_gif->fmt_ctx);
}
/* }}} */


/* {{{ proto object ffmpeg_animated_gif(string filename) 
   Constructor for ffmpeg_animated_gif objects
 */
PHP_FUNCTION(ffmpeg_animated_gif)
{
    zval ***argv;
    int ret, width, height, frame_rate, loop_count;
    char *filename = NULL;
    ff_animated_gif_context *ff_animated_gif;
    
    /* retrieve arguments */ 
    argv = (zval ***) safe_emalloc(sizeof(zval **), ZEND_NUM_ARGS(), 0);
    
    if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), argv) != SUCCESS) {
        efree(argv);
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "Error parsing arguments");
    }

#if LIBAVCODEC_BUILD > 4753
	loop_count = AVFMT_NOOUTPUTLOOP;
#endif

    switch (ZEND_NUM_ARGS()) {
        case 5:
#if LIBAVCODEC_BUILD < 4627 
            zend_error(E_WARNING, "Animated GIF looping not supported \
                    by this version of ffmpeg.\n");
#endif
			/* parse optional loop count */
            convert_to_long_ex(argv[4]);
            loop_count = Z_LVAL_PP(argv[4]);

            if (loop_count < 0 || loop_count > 65535) {
                zend_error(E_ERROR, "Loop count must be a number between 0 and 65535.\n");
            }
        case 4:
            /* parse file path */
            convert_to_string_ex(argv[0]);
            filename = Z_STRVAL_PP(argv[0]);

            /* parse width */
            convert_to_long_ex(argv[1]);
            width = Z_LVAL_PP(argv[1]);

            /* width must be even number for ffmpeg encoders */
            if (width % 2) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "Width must be an even number");
            }

            /* parse height */
            convert_to_long_ex(argv[2]);
            height = Z_LVAL_PP(argv[2]);

            /* height must be even number for ffmpeg encoders */
            if (height % 2) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "Height must be an even number");
            }

            /* parse frame rate */
            /* TODO: bound check rate (1-100?) */
            convert_to_long_ex(argv[3]);
            frame_rate = Z_LVAL_PP(argv[3]);
            break;
        default:
            WRONG_PARAM_COUNT;
    } 

    efree(argv);

	ff_animated_gif = _php_alloc_ff_animated_gif();

    ff_animated_gif->fmt_ctx->oformat->flags &= AVFMT_RAWPICTURE;

    /* add the video stream using the default format codecs
     *        and initialize the codecs */
    ff_animated_gif->video_st = NULL;
    if (ff_animated_gif->fmt->video_codec != CODEC_ID_NONE) {
        ff_animated_gif->video_st =
            _php_add_video_stream(ff_animated_gif->fmt_ctx, 
                    ff_animated_gif->fmt->video_codec, width, height,
                    frame_rate, loop_count);
    } else {
        zend_error(E_ERROR, "Codec not found\n");\
    }

    /* set the output parameters (must be done even if no parameters). */
    if (av_set_parameters(ff_animated_gif->fmt_ctx, NULL) < 0) {
        zend_error(E_ERROR, "Invalid output format parameters\n");\
    }
    
    /* TODO: hold of on the open until the first frame is added */
    _php_open_movie_file(ff_animated_gif, filename);
    
    /* pass NULL for resource result since we're not returning the resource
     directly, but adding it to the returned object. */
	ret = ZEND_REGISTER_RESOURCE(NULL, ff_animated_gif, le_ffmpeg_animated_gif);
    
    object_init_ex(getThis(), ffmpeg_animated_gif_class_entry_ptr);
    add_property_resource(getThis(), "ffmpeg_animated_gif", ret);
}
/* }}} */


/* {{{ _php_free_ffmpeg_animated_gif
 */
static void _php_free_ffmpeg_animated_gif(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    int i;
    ff_animated_gif_context *ff_animated_gif = (ff_animated_gif_context*)rsrc->ptr;    

    /* close each codec */
    if (ff_animated_gif->video_st) {
        avcodec_close(&ff_animated_gif->video_st->codec);
        av_free(ff_animated_gif->video_outbuf);
    }

    /* write the trailer, if any */
    av_write_trailer(ff_animated_gif->fmt_ctx);

    /* free the streams */
    for(i = 0; i < ff_animated_gif->fmt_ctx->nb_streams; i++) {
        av_freep(&ff_animated_gif->fmt_ctx->streams[i]);
    }

    if (!(ff_animated_gif->fmt->flags & AVFMT_NOFILE)) {
        /* close the output file */
        url_fclose(&ff_animated_gif->fmt_ctx->pb);
    }

    if (ff_animated_gif->codec_ctx) {
        avcodec_close(ff_animated_gif->codec_ctx);
    }

    /* free the contexts */
    if (ff_animated_gif->fmt_ctx) {
        av_free(ff_animated_gif->fmt_ctx);
    }
    
    efree(ff_animated_gif);
}
/* }}} */


/* {{{ register_ffmpeg_animated_gif_class()
 */
void register_ffmpeg_animated_gif_class(int module_number)
{
    TSRMLS_FETCH();
    le_ffmpeg_animated_gif = zend_register_list_destructors_ex(_php_free_ffmpeg_animated_gif,
            NULL, "ffmpeg_animated_gif", module_number);

   
    INIT_CLASS_ENTRY(ffmpeg_animated_gif_class_entry, "ffmpeg_animated_gif", 
            ffmpeg_animated_gif_class_methods);
    
    /* register ffmpeg movie class */
    ffmpeg_animated_gif_class_entry_ptr = 
        zend_register_internal_class(&ffmpeg_animated_gif_class_entry TSRMLS_CC);
}
/* }}} */


/* {{{ _php_addframe()
   add a frame to the animated gif.
 */
static int _php_addframe(ff_animated_gif_context *ff_animated_gif, ff_frame_context *frame)
{
    int out_size;
    AVCodecContext *c;

    c = &ff_animated_gif->video_st->codec;

    if (frame->width != c->width || frame->height != c->height) {
        _php_resample_frame(frame, c->width, c->height, 0,0,0,0);
    }
 
    /* convert frame to the RGB24 if needed */
    _php_convert_frame(frame, PIX_FMT_RGB24);
   
    /* encode the image */
    out_size = avcodec_encode_video(c, ff_animated_gif->video_outbuf, 
            ff_animated_gif->video_outbuf_size, frame->av_frame);

    /* if zero size, it means the image was buffered */
    if (out_size != 0) {
        AVPacket pkt;
        av_init_packet(&pkt);

        pkt.pts= c->coded_frame->pts;
        if(c->coded_frame->key_frame)
            pkt.flags |= PKT_FLAG_KEY;
        pkt.stream_index= ff_animated_gif->video_st->index;
        pkt.data= ff_animated_gif->video_outbuf;
        pkt.size= out_size;

        /* write the compressed frame in the media file */
        if (av_write_frame(ff_animated_gif->fmt_ctx, &pkt) != 0) {
            zend_error(E_ERROR, "Error while writing video frame\n");
        }
    }
    return 0;
}
/* }}} */


/* {{{ proto int addFrame(resource)
 */
PHP_FUNCTION(addFrame)
{
    zval **argv[1];
    ff_frame_context *ff_frame;
    ff_animated_gif_context *ff_animated_gif;

    if (ZEND_NUM_ARGS() != 1) {
        WRONG_PARAM_COUNT;
    }

    GET_ANIMATED_GIF_RESOURCE(ff_animated_gif);

    /* retrieve arguments */ 
    if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), argv) != SUCCESS) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR,
                "Error parsing arguments");
    }

    convert_to_object_ex(argv[0]);

    GET_FRAME_RESOURCE(*argv[0], ff_frame);

    _php_addframe(ff_animated_gif, ff_frame);
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4
 * vim<600: noet sw=4 ts=4
 */
