#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"

#include "php_ffmpeg.h"

#include "ffmpeg_frame.h"
#include "ffmpeg_output_movie.h"

#define GET_OUTPUT_MOVIE_RESOURCE(ff_output_movie_ctx) {\
	zval **_tmp_zval;\
    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmpeg_output_movie",\
                sizeof("ffmpeg_output_movie"), (void **)&_tmp_zval) == FAILURE) {\
        zend_error(E_ERROR, "Unable to find ffmpeg_output_movie property");\
        RETURN_FALSE;\
    }\
\
    ZEND_FETCH_RESOURCE(ff_output_movie_ctx, ff_output_movie_context*, _tmp_zval, -1,\
            "ffmpeg_output_movie", le_ffmpeg_output_movie);\
}\

static zend_class_entry *ffmpeg_output_movie_class_entry_ptr;
zend_class_entry ffmpeg_output_movie_class_entry;

extern int le_ffmpeg_frame;
static int le_ffmpeg_output_movie;

/* {{{ ffmpeg_output_movie methods[]
    Methods of the ffmpeg_output_movie class 
*/
zend_function_entry ffmpeg_output_movie_class_methods[] = {
   
    /* contructor */
    PHP_FE(ffmpeg_output_movie,     NULL)

    /* methods */
    PHP_FALIAS(addframe,            addFrame,           NULL)
	{NULL, NULL, NULL}
};
/* }}} */


/* {{{ _php_alloc_ff_output_movie()
 */
static ff_output_movie_context* _php_alloc_ff_output_movie()
{
    ff_output_movie_context *ff_output_movie;
    
    ff_output_movie = emalloc(sizeof(ff_output_movie_context));
    ff_output_movie->fmt_ctx = NULL;
    ff_output_movie->codec_ctx = NULL;
    return ff_output_movie;
}
/* }}} */


/* {{{ _php_open_movie_file()
 */
static void _php_open_movie_file(ff_output_movie_context *ff_output_movie, 
        char* filename)
{
    AVFormatParameters params;

    /* open the file with generic libav function */
    if (av_open_input_file(&(ff_output_movie->fmt_ctx), filename, NULL, 0, 
                &params)) {
        zend_error(E_ERROR, "Can't open movie file %s", filename);
    }
}
/* }}} */


/* {{{ proto object ffmpeg_output_movie(string filename) 
   Constructor for ffmpeg_output_movie objects
 */
PHP_FUNCTION(ffmpeg_output_movie)
{
    int argc, ret;
    zval **argv[1];
    ff_output_movie_context *ff_output_movie;
    
    /* set the number of arguments */
    argc = ZEND_NUM_ARGS();

    if (argc != 1) {
        WRONG_PARAM_COUNT;
    }

    /* argument count is correct, now retrieve arguments */
    if (zend_get_parameters_array_ex(argc, argv) != SUCCESS) {
        WRONG_PARAM_COUNT;
    }
  
	ff_output_movie = _php_alloc_ff_output_movie();
    
    /* pass NULL for resource result since we're not returning the resource
     directly, but adding it to the returned object. */
	ret = ZEND_REGISTER_RESOURCE(NULL, ff_output_movie, le_ffmpeg_output_movie);
    
    object_init_ex(getThis(), ffmpeg_output_movie_class_entry_ptr);
    add_property_resource(getThis(), "ffmpeg_output_movie", ret);
}
/* }}} */


/* {{{ _php_free_ffmpeg_output_movie
 */
static void _php_free_ffmpeg_output_movie(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    ff_output_movie_context *ff_output_movie = (ff_output_movie_context*)rsrc->ptr;    

    if (ff_output_movie->codec_ctx) {
        avcodec_close(ff_output_movie->codec_ctx);
    }

    efree(ff_output_movie);
}
/* }}} */


/* {{{ register_ffmpeg_output_movie_class()
 */
void register_ffmpeg_output_movie_class(int module_number)
{
    TSRMLS_FETCH();
    le_ffmpeg_output_movie = zend_register_list_destructors_ex(_php_free_ffmpeg_output_movie,
            NULL, "ffmpeg_output_movie", module_number);

   
    INIT_CLASS_ENTRY(ffmpeg_output_movie_class_entry, "ffmpeg_output_movie", 
            ffmpeg_output_movie_class_methods);
    
    /* register ffmpeg movie class */
    ffmpeg_output_movie_class_entry_ptr = 
        zend_register_internal_class(&ffmpeg_output_movie_class_entry TSRMLS_CC);
}
/* }}} */


/* {{{ _php_get_stream_index()
 */
static int _php_get_stream_index(AVFormatContext *fmt_ctx, int type)
{
    int i;
    
    for (i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i] && 
                fmt_ctx->streams[i]->codec.codec_type == type) {
            return i;
        }
    }
    /* stream not found */
    return -1;
}
/* }}} */


/* {{{ _php_addframe()
   add a frame from to the movie.
 */
static AVFrame* _php_addframe(ff_output_movie_context *ff_output_movie, ff_frame_context *frame)
{
    AVCodecContext *encoder_ctx;
    AVPacket packet;
    AVFrame *av_frame = NULL;
    int video_stream;

    video_stream = _php_get_stream_index(ff_output_movie->fmt_ctx, CODEC_TYPE_VIDEO);
    if (video_stream < 0) {
        return NULL;
    }
 
    //encoder_ctx = _php_get_encoder_context(ff_output_movie, CODEC_TYPE_VIDEO);
}
/* }}} */


/* {{{ proto int addFrame(resource)
 */
PHP_FUNCTION(addFrame)
{
    zval **argv[1];
    ff_frame_context *ff_frame;
    ff_output_movie_context *ff_output_movie;

    if (ZEND_NUM_ARGS() != 1) {
        WRONG_PARAM_COUNT;
    }

    GET_OUTPUT_MOVIE_RESOURCE(ff_output_movie);

    /* retrieve arguments */ 
    if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), argv) != SUCCESS) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR,
                "Error parsing arguments");
    }

    convert_to_object_ex(argv[0]);

    GET_FRAME_RESOURCE(*argv[0], ff_frame);

    _php_addframe(ff_output_movie, ff_frame);
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
