#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"

#include "php_ffmpeg.h"

#include "ffmpeg_frame.h"
#include "ffmpeg_movie.h"

static zend_class_entry *ffmpeg_movie_class_entry_ptr;
zend_class_entry ffmpeg_movie_class_entry;

static int le_ffmpeg_movie;

/* {{{ ffmpeg_movie methods[]
    Methods of the ffmpeg_movie class 
*/
zend_function_entry ffmpeg_movie_class_methods[] = {
   
    /* contructor */
    PHP_FE(ffmpeg_movie, NULL)

    /* methods */
	PHP_FALIAS(getduration,         getDuration,        NULL)
	PHP_FALIAS(getframecount,       getFrameCount,      NULL)
	PHP_FALIAS(getframerate,        getFrameRate,       NULL)
	PHP_FALIAS(getfilename,         getFileName,        NULL)
	PHP_FALIAS(getcomment,          getComment,         NULL)
	PHP_FALIAS(gettitle,            getTitle,           NULL)
    PHP_FALIAS(getauthor,           getAuthor,          NULL)
	PHP_FALIAS(getcopyright,        getCopyright,       NULL)
    PHP_FALIAS(getframewidth,       getFrameWidth,      NULL)
    PHP_FALIAS(getframeheight,      getFrameHeight,     NULL)
    PHP_FALIAS(getframenumber,      getFrameNumber,     NULL)
    PHP_FALIAS(getpixelformat,      getPixelFormat,     NULL)
    PHP_FALIAS(getbitrate,          getBitRate,         NULL)
    PHP_FALIAS(hasaudio,            hasAudio,           NULL)
    PHP_FALIAS(getframe,            getFrame,           NULL)
	{NULL, NULL, NULL}
};
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


/* {{{ _php_get_video_stream()
 */
static AVStream *_php_get_video_stream(AVFormatContext *fmt_ctx)
{
    int i = _php_get_stream_index(fmt_ctx, CODEC_TYPE_VIDEO);
    
    return i < 0 ? NULL : fmt_ctx->streams[i];
}
/* }}} */


/* {{{ _php_get_audio_stream()
 */
static AVStream *_php_get_audio_stream(AVFormatContext *fmt_ctx)
{
    int i = _php_get_stream_index(fmt_ctx, CODEC_TYPE_AUDIO);
    
    return i < 0 ? NULL : fmt_ctx->streams[i];
}
/* }}} */


/* {{{ _php_get_filename()
 */
static char* _php_get_filename(ff_movie_context *ffmovie_ctx)
{
    return ffmovie_ctx->fmt_ctx->filename;
}
/* }}} */


/* {{{ _php_alloc_ffmovie_ctx()
 */
static ff_movie_context* _php_alloc_ffmovie_ctx()
{
    ff_movie_context *ffmovie_ctx;
    
    ffmovie_ctx = emalloc(sizeof(ff_movie_context));
    ffmovie_ctx->fmt_ctx = NULL;
    ffmovie_ctx->codec_ctx = NULL;
    return ffmovie_ctx;
}
/* }}} */


/* {{{ _php_open_movie_file()
 */
static void _php_open_movie_file(ff_movie_context *ffmovie_ctx, 
        char* filename)
{
    AVFormatParameters params;

    /* open the file with generic libav function */
    if (av_open_input_file(&(ffmovie_ctx->fmt_ctx), filename, NULL, 0, 
                &params)) {
        zend_error(E_ERROR, "Can't open movie file %s", filename);
    }
    
    /* If not enough info to get the stream parameters, we decode the
       first frames to get it. */
    if (av_find_stream_info(ffmovie_ctx->fmt_ctx)) {
        /* This is not a problem for some formats like .mov */
        /*zend_error(E_WARNING, "Can't find codec parameters for %s", filename); */
    }
}
/* }}} */


/* {{{ proto object ffmpeg_movie(string filename) 
   Constructor for ffmpeg_movie objects
 */
PHP_FUNCTION(ffmpeg_movie)
{
    int argc, ret;
    zval **argv[1];
    ff_movie_context *ffmovie_ctx;
    
    /* get the number of arguments */
    argc = ZEND_NUM_ARGS();

    if (argc != 1) {
        WRONG_PARAM_COUNT;
    }

    /* argument count is correct, now retrieve arguments */
    if (zend_get_parameters_array_ex(argc, argv) != SUCCESS) {
        WRONG_PARAM_COUNT;
    }
  
	ffmovie_ctx = _php_alloc_ffmovie_ctx();
    
    convert_to_string_ex(argv[0]);
    
    _php_open_movie_file(ffmovie_ctx, Z_STRVAL_PP(argv[0]));

    /* pass NULL for resource result since we're not returning the resource
     directly, but adding it to the returned object. */
	ret = ZEND_REGISTER_RESOURCE(NULL, ffmovie_ctx, le_ffmpeg_movie);
    
    object_init_ex(getThis(), ffmpeg_movie_class_entry_ptr);
    add_property_resource(getThis(), "ffmpeg_movie", ret);
}
/* }}} */


/* {{{ _php_free_ffmpeg_movie
 */
static void _php_free_ffmpeg_movie(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    ff_movie_context *ffmovie_ctx = (ff_movie_context*)rsrc->ptr;    

    if (ffmovie_ctx->codec_ctx) {
        avcodec_close(ffmovie_ctx->codec_ctx);
    }

    av_close_input_file(ffmovie_ctx->fmt_ctx);

    efree(ffmovie_ctx);
}
/* }}} */


/* {{{ register_ffmpeg_movie_class()
 */
void register_ffmpeg_movie_class(int module_number) {
    le_ffmpeg_movie = zend_register_list_destructors_ex(_php_free_ffmpeg_movie,
            NULL, "ffmpeg_movie", module_number);

   
    INIT_CLASS_ENTRY(ffmpeg_movie_class_entry, "ffmpeg_movie", 
            ffmpeg_movie_class_methods);
    
    /* register ffmpeg movie class */
    ffmpeg_movie_class_entry_ptr = 
        zend_register_internal_class(&ffmpeg_movie_class_entry TSRMLS_CC);
}
/* }}} */


/* {{{ _php_get_codec_ctx() 
   Opens codecs and gets codec context. Always call this to get a pointer to 
   the codec context. This allows to postpone codec init until a function
   that requires it is called.
 */
static AVCodecContext* _php_get_decoder_context(ff_movie_context *ffmovie_ctx,
        int stream_type)
{
    AVCodec *decoder;
    int stream_index;

    stream_index = _php_get_stream_index(ffmovie_ctx->fmt_ctx, stream_type);
    if (stream_index < 0) {
        // FIXME: Find a way to do this without the if statement
        if (stream_type = CODEC_TYPE_VIDEO) {
            php_error_docref(NULL TSRMLS_CC, E_ERROR,
                    "Can't find video stream in %s", 
                    _php_get_filename(ffmovie_ctx));
        } else {
            php_error_docref(NULL TSRMLS_CC, E_ERROR,
                    "Can't find audio stream in %s", 
                    _php_get_filename(ffmovie_ctx));
        }
    }

    if (!ffmovie_ctx->codec_ctx) {
   
        ffmovie_ctx->codec_ctx = 
            &ffmovie_ctx->fmt_ctx->streams[stream_index]->codec;
        
        /* find the decoder */
        decoder = avcodec_find_decoder(ffmovie_ctx->codec_ctx->codec_id);
        if (!decoder) {
            zend_error(E_ERROR, "Could not find decoder for %s", 
                    _php_get_filename(ffmovie_ctx));
        }

        /* open the decoder */
        if (avcodec_open(ffmovie_ctx->codec_ctx, decoder) < 0) {
            zend_error(E_ERROR, "Could not open codec for %s",
                    _php_get_filename(ffmovie_ctx));
        }
    }
    return ffmovie_ctx->codec_ctx;
}
/* }}} */


/* {{{ proto string getComment()
 */
PHP_FUNCTION(getComment)
{
    ff_movie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);
    
    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->comment,
            strlen(ffmovie_ctx->fmt_ctx->comment), 1);
}
/* }}} */


/* {{{ proto string getTitle()
 */
PHP_FUNCTION(getTitle)
{
    ff_movie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->title,
            strlen(ffmovie_ctx->fmt_ctx->title), 1);
}
/* }}} */


/* {{{ proto string getAuthor()
 */
PHP_FUNCTION(getAuthor)
{
    ff_movie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->author,
            strlen(ffmovie_ctx->fmt_ctx->author), 1);
}
/* }}} */


/* {{{ proto string getCopyright()
 */
PHP_FUNCTION(getCopyright)
{
    ff_movie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->copyright,
            strlen(ffmovie_ctx->fmt_ctx->copyright), 1);
}
/* }}} */


/* {{{ _php_get_duration()
 */
static float _php_get_duration(ff_movie_context *ffmovie_ctx)
{
    float duration;

    duration = (float)ffmovie_ctx->fmt_ctx->duration / AV_TIME_BASE;

    if (duration < 0) {
        duration = 0.0f;
    }
    return duration;
}
/* }}} */


/* {{{ proto int getDuration()
 */
PHP_FUNCTION(getDuration)
{
    ff_movie_context *ffmovie_ctx;
       
    GET_MOVIE_RESOURCE(ffmovie_ctx);
    
    RETURN_DOUBLE(_php_get_duration(ffmovie_ctx));
}
/* }}} */


/* {{{ _php_get_framecount()
 */
static long _php_get_framecount(ff_movie_context *ffmovie_ctx)
{
    float duration = 0.0, frame_rate = 0.0;
    AVStream *st = _php_get_video_stream(ffmovie_ctx->fmt_ctx);

    if (!st) {
      return 0;
    }
    
    duration = _php_get_duration(ffmovie_ctx);
    frame_rate = (float)st->codec.frame_rate / st->codec.frame_rate_base;

    return lrint(frame_rate * duration);
}
/* }}} */


/* {{{ proto int getFrameCount()
 */
PHP_FUNCTION(getFrameCount)
{
    ff_movie_context *ffmovie_ctx;
    GET_MOVIE_RESOURCE(ffmovie_ctx);
    RETURN_LONG(_php_get_framecount(ffmovie_ctx));
}
/* }}} */


/* {{{ _php_get_framerate()
 */
static float _php_get_framerate(ff_movie_context *ffmovie_ctx)
{
    AVStream *st = _php_get_video_stream(ffmovie_ctx->fmt_ctx);

    if (!st) {
      return 0.0f;
    }

    /*
    if (st->codec.frame_rate > 1000 && st->codec.frame_rate_base==1) {
        st->codec.frame_rate_base = 1000;
    }
     */

    return (float)st->codec.frame_rate / st->codec.frame_rate_base;
}
/* }}} */


/* {{{ proto int getFrameRate()
 */
PHP_FUNCTION(getFrameRate)
{
    ff_movie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_DOUBLE(_php_get_framerate(ffmovie_ctx));
}
/* }}} */


/* {{{ proto string getFileName()
 */
PHP_FUNCTION(getFileName)
{
    ff_movie_context *ffmovie_ctx;
    char* filename;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    filename = _php_get_filename(ffmovie_ctx);
    RETURN_STRINGL(filename, strlen(filename), 1);
}
/* }}} */


/* {{{ _php_get_framewidth()
 */
static int _php_get_framewidth(ff_movie_context *ffmovie_ctx)
{
    AVStream *st = _php_get_video_stream(ffmovie_ctx->fmt_ctx);

    if (!st) {
      return 0;
    }
 
    return st->codec.width;
}
/* }}} */


/* {{{ proto int getFrameWidth()
 */
PHP_FUNCTION(getFrameWidth)
{
    ff_movie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);
    
    RETURN_LONG(_php_get_framewidth(ffmovie_ctx));
}
/* }}} */


/* {{{ _php_get_frameheight()
 */
static int _php_get_frameheight(ff_movie_context *ffmovie_ctx)
{
    AVStream *st = _php_get_video_stream(ffmovie_ctx->fmt_ctx);

    if (!st) {
      return 0;
    }
 
    return st->codec.height;
}
/* }}} */


/* {{{ proto int getFrameHeight()
 */
PHP_FUNCTION(getFrameHeight)
{
    ff_movie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_LONG(_php_get_frameheight(ffmovie_ctx));
}
/* }}} */


/* {{{ _php_get_framenumber()
 */
static long _php_get_framenumber(ff_movie_context *ffmovie_ctx) 
{
    AVCodecContext *decoder_ctx;

    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, CODEC_TYPE_VIDEO);

    if (decoder_ctx->frame_number <= 0) {
        return 1; /* no frames read yet so return the first */
    } else {
        return decoder_ctx->frame_number;
    }
}
/* }}} */


/* {{{ proto resource getFrameNumber()
 */
PHP_FUNCTION(getFrameNumber)
{
    ff_movie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_LONG(_php_get_framenumber(ffmovie_ctx));
}
/* }}} */


/* {{{ _php_get_pixelformat()
 */
static int _php_get_pixelformat(ff_movie_context *ffmovie_ctx)
{
    AVCodecContext *decoder_ctx;
    
    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, CODEC_TYPE_VIDEO);

    return decoder_ctx->pix_fmt;
}
/* }}} */


/* {{{ proto int getPixelFormat()
 */
PHP_FUNCTION(getPixelFormat)
{
    int pix_fmt;
    const char *fmt;
    ff_movie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    pix_fmt = _php_get_pixelformat(ffmovie_ctx);
    fmt = avcodec_get_pix_fmt_name(pix_fmt);
    
    if (fmt) {
        /* cast const to non-const to keep compiler from complaining, 
           RETURN_STRINGL just copies so the string won't get overwritten
           */
        RETURN_STRINGL((char *)fmt, strlen(fmt), 1);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ _php_get_bitrate()
 */
static int _php_get_bitrate(ff_movie_context *ffmovie_ctx)
{
    return ffmovie_ctx->fmt_ctx->bit_rate;
}
/* }}} */


/* {{{ proto int getBitrate()
 */
PHP_FUNCTION(getBitRate)
{
    int bitrate;
    ff_movie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);
   
    /* convert to kb/s */
    bitrate = _php_get_bitrate(ffmovie_ctx) / 1000; 

    RETURN_LONG(bitrate);
}
/* }}} */


/* {{{ proto int hasAudio()
 */
PHP_FUNCTION(hasAudio)
{
    ff_movie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_BOOL(_php_get_audio_stream(ffmovie_ctx->fmt_ctx));
}
/* }}} */


/* {{{ _php_getframe()
   Returns a frame from the movie.
 */
static AVFrame* _php_getframe(ff_movie_context *ffmovie_ctx, int wanted_frame)
{
    AVCodecContext *decoder_ctx;
    AVPacket packet;
    AVFrame *frame = NULL;
    int got_frame; 
    int video_stream;

    video_stream = _php_get_stream_index(ffmovie_ctx->fmt_ctx, CODEC_TYPE_VIDEO);
    if (video_stream < 0) {
        return NULL;
    }
 
    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, CODEC_TYPE_VIDEO);

    /* Rewind to the beginning of the stream if wanted frame already passed */
    if (wanted_frame && wanted_frame <= ffmovie_ctx->codec_ctx->frame_number) {
        if (av_seek_frame(ffmovie_ctx->fmt_ctx, -1, 0) < 0) {
            zend_error(E_ERROR, "Error seeking to begining of video stream");
        }
        /* close decoder */
        avcodec_close(ffmovie_ctx->codec_ctx);
        ffmovie_ctx->codec_ctx = NULL;

        /* re-open decoder */
        decoder_ctx = _php_get_decoder_context(ffmovie_ctx, CODEC_TYPE_VIDEO);
    }

    frame = avcodec_alloc_frame();
    
    /* read frames looking for wanted_frame */ 
    while (av_read_frame(ffmovie_ctx->fmt_ctx, &packet) >= 0) {

        if (packet.stream_index == video_stream) {
        
            avcodec_decode_video(decoder_ctx, frame, &got_frame,
                    packet.data, packet.size);

            if (got_frame) {
                if (!wanted_frame || decoder_ctx->frame_number == wanted_frame) {
                    /* free wanted frame packet */
                    av_free_packet(&packet);
                    break; 
                }
            }
        }

        /* free the packet allocated by av_read_frame */
        av_free_packet(&packet);
    }
    return frame;
}
/* }}} */


/* {{{ proto resource getFrame([int frame])
 */
PHP_FUNCTION(getFrame)
{
    zval **argv[1];
    int wanted_frame = 0; 
    AVFrame *frame = NULL;
    ff_frame_context *ff_frame;
    ff_movie_context *ffmovie_ctx;

    if (ZEND_NUM_ARGS() > 1) {
        WRONG_PARAM_COUNT;
    }
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    if (ZEND_NUM_ARGS()) {

        /* retrieve arguments */ 
        if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), argv) != SUCCESS) {
            php_error_docref(NULL TSRMLS_CC, E_ERROR,
                    "Error parsing arguments");
        }

        convert_to_long_ex(argv[0]);
        wanted_frame = Z_LVAL_PP(argv[0]);

        /* bounds check wanted frame */
        if (wanted_frame < 1) {
            php_error_docref(NULL TSRMLS_CC, E_ERROR,
                    "Frame number must be greater than zero");
        }
    } 

    frame = _php_getframe(ffmovie_ctx, wanted_frame);
    if (frame) { 
        /* 
         * _php_create_ffmpeg_frame sets PHP return_value to a ffmpeg_frame 
         * object via INTERNAL_FUNCTION_PARAM_PASSTHRU, the returned ff_frame
         * is just for conveniently setting its fields. 
         */
        ff_frame = _php_create_ffmpeg_frame(INTERNAL_FUNCTION_PARAM_PASSTHRU);

        if (!ff_frame) {
            php_error_docref(NULL TSRMLS_CC, E_ERROR,
                    "Error allocating ffmpeg_frame resource");
        }

        /* TODO: Provide function(s) for setting these */
        ff_frame->width = _php_get_framewidth(ffmovie_ctx);
        ff_frame->height = _php_get_frameheight(ffmovie_ctx);
        ff_frame->pixel_format = _php_get_pixelformat(ffmovie_ctx);
        
        ff_frame->av_frame = avcodec_alloc_frame();
        avpicture_alloc((AVPicture*)ff_frame->av_frame, ff_frame->pixel_format,
            ff_frame->width, ff_frame->height);
 
        /* FIXME: temporary hack until I figure out how to pass new buffers to the decoder */
        img_copy((AVPicture*)ff_frame->av_frame, 
                (AVPicture *)frame, ff_frame->pixel_format, 
                ff_frame->width, ff_frame->height);

        //ff_frame->av_frame = frame;
    } else {
        RETURN_FALSE;
    }
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
