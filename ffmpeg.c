/*
   ffmpeg-php - a php module for accessing video info from movie files.
   
   Copyright (C) 2004  Todd Kirby (doubleshot at pacbell dot net)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <assert.h>
#include <avcodec.h>
#include <avformat.h>
#include <inttypes.h>
#include <math.h>

#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"

#include "php_ffmpeg.h"

/* 
   include gd header from local include dir. This is a copy of gd.h that is 
   distributed with php-4.3.9. It is distributed along with ffmpeg-php to
   allow ffmpeg-php to be built without needing access to the php sources
 */
#include "include/gd.h" 

#define GET_MOVIE_RESOURCE(im) {\
	zval **_tmp_zval;\
    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmpeg_movie",\
                sizeof("ffmpeg_movie"), (void **)&_tmp_zval) == FAILURE) {\
        zend_error(E_ERROR, "Unable to find ffmpeg_movie property");\
        RETURN_FALSE;\
    }\
\
    ZEND_FETCH_RESOURCE(im, ffmovie_context*, _tmp_zval, -1,\
            "ffmpeg_movie", le_ffmpeg_movie);\
}\


static int le_ffmpeg_movie;

static int le_gd;

static zend_class_entry *ffmpeg_movie_class_entry_ptr;

zend_class_entry ffmpeg_movie_class_entry;

typedef struct {
    AVFrame *frame;
    int width;
    int height;
    int pixel_format;
} ffmovie_frame;

typedef struct {
    AVFormatContext *fmt_ctx;
    AVCodecContext *codec_ctx;
    ffmovie_frame yuv_ctx_frame;
    ffmovie_frame resample_ctx_frame;
    ffmovie_frame rgba_ctx_frame;
} ffmovie_context;


/* {{{ ffmpeg_movie methods[]
    Methods of the ffmpeg_movie class 
*/
zend_function_entry ffmpeg_movie_class_methods[] = {
   
	PHP_FE(ffmpeg_movie, NULL)

	PHP_FE(getDuration, NULL)
    PHP_FALIAS(getduration, getDuration, NULL)

	PHP_FE(getFrameCount, NULL)
    PHP_FALIAS(getframecount, getFrameCount, NULL)

	PHP_FE(getFrameRate, NULL)
    PHP_FALIAS(getframerate, getFrameRate, NULL)
    
	PHP_FE(getFileName, NULL)
    PHP_FALIAS(getfilename, getFileName, NULL)
    
	PHP_FE(getComment, NULL)
    PHP_FALIAS(getcomment, getComment, NULL)
 
	PHP_FE(getTitle, NULL)
    PHP_FALIAS(gettitle, getTitle, NULL)

    PHP_FE(getAuthor, NULL)
    PHP_FALIAS(getauthor, getAuthor, NULL)
 
	PHP_FE(getCopyright, NULL)
    PHP_FALIAS(getcopyright, getCopyright, NULL)

    PHP_FE(getFrameWidth, NULL)
    PHP_FALIAS(getframewidth, getFrameWidth, NULL)

    PHP_FE(getFrameHeight, NULL)
    PHP_FALIAS(getframeheight, getFrameHeight, NULL)

    PHP_FE(getFrameNumber, NULL)
    PHP_FALIAS(getframenumber, getFrameNumber, NULL)

    PHP_FE(getPixelFormat, NULL)
    PHP_FALIAS(getpixelformat, getPixelFormat, NULL)

    PHP_FE(getVideoBitRate, NULL)
    PHP_FALIAS(getvideobitrate, getVideoBitRate, NULL)
/*
    PHP_FE(getAudioBitRate, NULL)
    PHP_FALIAS(getaudiobitrate, getAudioBitRate, NULL)

    PHP_FE(getCodecName, NULL)
    PHP_FALIAS(getcodecname, getCodecName, NULL)
*/
    PHP_FE(hasAudio, NULL)
    PHP_FALIAS(hasaudio, hasAudio, NULL)

#if HAVE_LIBGD20
    PHP_FE(getFrame, NULL)
    PHP_FALIAS(getframe, getFrame, NULL)

    PHP_FE(getFrameResampled, NULL)
    PHP_FALIAS(getframeresampled, getFrameResampled, NULL)
#endif /* HAVE_LIBGD20 */

	{NULL, NULL, NULL}
};
/* }}} */


zend_module_entry ffmpeg_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"ffmpeg",
	NULL,
	PHP_MINIT(ffmpeg),
	PHP_MSHUTDOWN(ffmpeg),
	NULL,
	NULL,
	PHP_MINFO(ffmpeg),
#if ZEND_MODULE_API_NO >= 20010901
	"0.3.6a", /* version number for ffmpeg-php */
#endif
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_FFMPEG
ZEND_GET_MODULE(ffmpeg)
#endif

/* {{{ _php_free_ffmpeg_movie
 */
static void _php_free_ffmpeg_movie(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    ffmovie_context *ffmovie_ctx = (ffmovie_context*)rsrc->ptr;    

    if (ffmovie_ctx->codec_ctx) {
        avcodec_close(ffmovie_ctx->codec_ctx);
    }

    av_close_input_file(ffmovie_ctx->fmt_ctx);

    /* if format conversion was done in getFrame or getFrameResmpled
       context frames may need to be freed */
    if (ffmovie_ctx->rgba_ctx_frame.frame) { 
       avpicture_free((AVPicture *)ffmovie_ctx->rgba_ctx_frame.frame);
       av_free(ffmovie_ctx->rgba_ctx_frame.frame);
    }
    if (ffmovie_ctx->yuv_ctx_frame.frame) { 
       avpicture_free((AVPicture *)ffmovie_ctx->yuv_ctx_frame.frame);
       av_free(ffmovie_ctx->yuv_ctx_frame.frame);
    }
    if (ffmovie_ctx->resample_ctx_frame.frame) { 
       avpicture_free((AVPicture *)ffmovie_ctx->resample_ctx_frame.frame);
       av_free(ffmovie_ctx->resample_ctx_frame.frame);
    }

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


/* {{{ php module init function
 */
PHP_MINIT_FUNCTION(ffmpeg)
{
    le_ffmpeg_movie = zend_register_list_destructors_ex(_php_free_ffmpeg_movie,
            NULL, "ffmpeg_movie", module_number);

    REGISTER_LONG_CONSTANT("LIBAVCODEC_VERSION_NUMBER", avcodec_version(), CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("LIBAVCODEC_BUILD_NUMBER", avcodec_build(), CONST_CS | CONST_PERSISTENT);
    
    INIT_CLASS_ENTRY(ffmpeg_movie_class_entry, "ffmpeg_movie", 
            ffmpeg_movie_class_methods);
    
    /* register ffmpeg movie class */
    ffmpeg_movie_class_entry_ptr = 
        zend_register_internal_class(&ffmpeg_movie_class_entry TSRMLS_CC);

    /* must be called before using avcodec libraries. */ 
    avcodec_init();

    /* register all codecs */
    av_register_all();
    return SUCCESS;
}
/* }}} */


/* {{{ php module shutdown function
 */
PHP_MSHUTDOWN_FUNCTION(ffmpeg)
{
    av_free_static();
	return SUCCESS;
}
/* }}} */


/* {{{ php info function
   Add an entry for ffmpeg support in phpinfo() */
PHP_MINFO_FUNCTION(ffmpeg)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ffmpeg support", "enabled");
	php_info_print_table_end();
}
/* }}} */


/* {{{ _php_open_movie_file()
 */
static void _php_open_movie_file(ffmovie_context *ffmovie_ctx, 
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


/* {{{ _php_alloc_ffmovie_ctx()
 */
static ffmovie_context* _php_alloc_ffmovie_ctx()
{
    ffmovie_context *ffmovie_ctx;
    
    ffmovie_ctx = emalloc(sizeof(ffmovie_context));
    ffmovie_ctx->fmt_ctx = NULL;
    ffmovie_ctx->codec_ctx = NULL;
    ffmovie_ctx->rgba_ctx_frame.frame = NULL;
    ffmovie_ctx->yuv_ctx_frame.frame = NULL;
    ffmovie_ctx->resample_ctx_frame.frame = NULL;
    return ffmovie_ctx;
}
/* }}} */


/* {{{ proto object ffmpeg_movie(string filename) 
   Constructor for ffmpeg_movie objects
 */
PHP_FUNCTION(ffmpeg_movie)
{
    int argc, ret;
    zval **argv[0];
    ffmovie_context *ffmovie_ctx;
    
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
    
    object_init_ex(getThis(), &ffmpeg_movie_class_entry);
    add_property_resource(getThis(), "ffmpeg_movie", ret);
}
/* }}} */


/* {{{ _php_get_duration()
 */
static float _php_get_duration(ffmovie_context *ffmovie_ctx)
{
    float duration;

    duration = ffmovie_ctx->fmt_ctx->duration / AV_TIME_BASE;

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
    ffmovie_context *ffmovie_ctx;
       
    GET_MOVIE_RESOURCE(ffmovie_ctx);
    
    RETURN_DOUBLE(_php_get_duration(ffmovie_ctx));
}
/* }}} */


/* {{{ _php_get_framecount()
 */
static long _php_get_framecount(ffmovie_context *ffmovie_ctx)
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
    ffmovie_context *ffmovie_ctx;
    GET_MOVIE_RESOURCE(ffmovie_ctx);
    RETURN_LONG(_php_get_framecount(ffmovie_ctx));
}
/* }}} */


/* {{{ _php_get_framerate()
 */
static float _php_get_framerate(ffmovie_context *ffmovie_ctx)
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
    ffmovie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_DOUBLE(_php_get_framerate(ffmovie_ctx));
}
/* }}} */


/* {{{ _php_get_filename()
 */
static char* _php_get_filename(ffmovie_context *ffmovie_ctx)
{
    return ffmovie_ctx->fmt_ctx->filename;
}
/* }}} */


/* {{{ proto string getFileName()
 */
PHP_FUNCTION(getFileName)
{
    ffmovie_context *ffmovie_ctx;
    char* filename;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    filename = _php_get_filename(ffmovie_ctx);
    RETURN_STRINGL(filename, strlen(filename), 1);
}
/* }}} */


/* {{{ proto string getComment()
 */
PHP_FUNCTION(getComment)
{
    ffmovie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);
    
    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->comment,
            strlen(ffmovie_ctx->fmt_ctx->comment), 1);
}
/* }}} */


/* {{{ proto string getTitle()
 */
PHP_FUNCTION(getTitle)
{
    ffmovie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->title,
            strlen(ffmovie_ctx->fmt_ctx->title), 1);
}
/* }}} */


/* {{{ proto string getAuthor()
 */
PHP_FUNCTION(getAuthor)
{
    ffmovie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->author,
            strlen(ffmovie_ctx->fmt_ctx->author), 1);
}
/* }}} */


/* {{{ proto string getCopyright()
 */
PHP_FUNCTION(getCopyright)
{
    ffmovie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->copyright,
            strlen(ffmovie_ctx->fmt_ctx->copyright), 1);
}
/* }}} */

/* {{{ _php_get_framewidth()
 */
static int _php_get_framewidth(ffmovie_context *ffmovie_ctx)
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
    ffmovie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);
    
    RETURN_LONG(_php_get_framewidth(ffmovie_ctx));
}
/* }}} */


/* {{{ _php_get_frameheight()
 */
static int _php_get_frameheight(ffmovie_context *ffmovie_ctx)
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
    ffmovie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_LONG(_php_get_frameheight(ffmovie_ctx));
}
/* }}} */


/* {{{ _php_get_codec_ctx() 
   Opens codecs and gets codec context. Always call this to get a pointer to 
   the codec context. This allows to postpone codec init until a function
   that requires it is called.
 */
static AVCodecContext* _php_get_decoder_context(ffmovie_context *ffmovie_ctx,
        int stream_index)
{
    AVCodec *decoder;
    
    if (!ffmovie_ctx->codec_ctx) {
   
        ffmovie_ctx->codec_ctx = 
            &ffmovie_ctx->fmt_ctx->streams[stream_index]->codec;
        
        /* find the decoder */
        decoder = avcodec_find_decoder(ffmovie_ctx->codec_ctx->codec_id);
        if (!decoder) {
            zend_error(E_ERROR, "Codec not found for %s", 
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


/* {{{ _php_get_frame_number()
 */
static long _php_get_frame_number(ffmovie_context *ffmovie_ctx) 
{
    AVCodecContext *decoder_ctx;
    int video_stream;

    video_stream = _php_get_stream_index(ffmovie_ctx->fmt_ctx, 
            CODEC_TYPE_VIDEO);
    if (video_stream < 0) {
        return 0;
    }

    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, video_stream);
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
    ffmovie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_LONG(_php_get_frame_number(ffmovie_ctx));
}
/* }}} */


/* {{{ _php_get_pixelformat()
 */
static const char* _php_get_pixelformat(ffmovie_context *ffmovie_ctx)
{
    AVCodecContext *decoder_ctx;
    int video_stream;

    video_stream = _php_get_stream_index(ffmovie_ctx->fmt_ctx, 
            CODEC_TYPE_VIDEO);
    if (video_stream < 0) {
        return NULL;
    }

    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, video_stream);
    return avcodec_get_pix_fmt_name(decoder_ctx->pix_fmt);
}
/* }}} */


/* {{{ proto int getPixelFormat()
 */
PHP_FUNCTION(getPixelFormat)
{
    const char *fmt;
    ffmovie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);
   
    fmt = _php_get_pixelformat(ffmovie_ctx);

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
static int _php_get_bitrate(ffmovie_context *ffmovie_ctx)
{
    return ffmovie_ctx->fmt_ctx->bit_rate;
}
/* }}} */


/* {{{ proto int getVideoBitrate()
 */
PHP_FUNCTION(getVideoBitRate)
{
    int bitrate;
    ffmovie_context *ffmovie_ctx;
    
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
    ffmovie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_BOOL(_php_get_audio_stream(ffmovie_ctx->fmt_ctx));
}
/* }}} */


/* {{{ proto int get_audio_sample_rate()
 */
/*
PHP_FUNCTION(getAudioBitRate)
{
    ffmovie_context *ffmovie_ctx;
    AVStream *audio_stream;
    AVCodec audio_codec;

    GET_MOVIE_RESOURCE(ffmovie_ctx);
    
    // TODO: test if this is an audio only filetype (mp3 ogg...)
    //       and just return same as getVideoBitRate if it is. 
    audio_stream = _php_get_audio_stream(ffmovie_ctx->fmt_ctx);
    
    audio_codec = avcodec_find_encoder(audio_stream->codec_id);
    
    RETURN_BOOL(_php_get_audio_stream(ffmovie_ctx->fmt_ctx));
}*/
/* }}} */


/* {{{ _php_get_context_frame()
 */
static AVFrame *_php_get_context_frame(ffmovie_frame *ff_frame, int width, 
        int height, int pixel_format) 
{
    /* free frame to force reallocation if width or height have changed */
    if (ff_frame->frame && 
            (width != ff_frame->width ||
             height != ff_frame->height ||
             pixel_format != ff_frame->pixel_format)) {
        avpicture_free((AVPicture *)ff_frame->frame);
        av_free(ff_frame->frame);
    }

    /* (re)allocate frame */
    if (!ff_frame->frame) {
        
        ff_frame->frame = avcodec_alloc_frame();
        avpicture_alloc((AVPicture*)ff_frame->frame, pixel_format, width, height);

        ff_frame->width = width;
        ff_frame->height = height;
        ff_frame->pixel_format = pixel_format;
    }

    return ff_frame->frame;
}
/* }}} */

static void _php_free_av_frame(ffmovie_context *ffmovie_ctx, AVFrame *frame) {
    /* don't free context frames, they get freed when the script exits. */
    if (frame != ffmovie_ctx->rgba_ctx_frame.frame &&
        frame != ffmovie_ctx->yuv_ctx_frame.frame &&
        frame != ffmovie_ctx->resample_ctx_frame.frame) {
        avpicture_free((AVPicture*)frame);
        av_free(frame);
    }
}


/* {{{ dump_img_to_sgi()
 * For debugging frame conversions
 * TODO: add img conversion to allow other pix_fmts than PIX_FMT_RGBA32
 */
void dump_img_to_sgi(AVFrame *frame, int width, int height, char *filename)
{
    int err;
    AVImageFormat *image_fmt;
    AVImageInfo img_info;
    ByteIOContext pb;
    for (image_fmt = first_image_format; image_fmt != NULL;
            image_fmt = image_fmt->next) {
        if (strncmp(image_fmt->name, "sgi", 3) == 0) {
            break;
        }
    }

    img_info.pict.data[0] = frame->data[0];
    img_info.pict.linesize[0] = frame->linesize[0];
    img_info.pix_fmt = PIX_FMT_RGBA32;
    img_info.width = width;
    img_info.height = height;
    img_info.interleaved = 0;

    /* open the file */
    err = url_fopen(&pb, filename, URL_RDWR);
    if (err < 0) {
        fprintf(stderr, "Could not open %s %d\n", filename, err);
        exit(1);
    }
    url_setbufsize(&pb, 4096);
    image_fmt->img_write(&pb, &img_info);
    url_fclose(&pb);
}
/* }}} */

#if HAVE_LIBGD20

/* {{{ _php_get_gd_image()
 */
zval* _php_get_gd_image(int w, int h)
{
    zval *function_name, *width, *height;
    zval **params[2];
    zval *return_value;
    zend_function *func;
    char *function_cname = "imagecreatetruecolor";

    if (zend_hash_find(EG(function_table), function_cname, 
                strlen(function_cname) + 1, (void **)&func) == FAILURE) {
        zend_error(E_ERROR, "Error can't find %s function", function_cname);
    }

    MAKE_STD_ZVAL(function_name);
    MAKE_STD_ZVAL(width);
    MAKE_STD_ZVAL(height);

    ZVAL_STRING(function_name, function_cname, 1);
    ZVAL_LONG(width, w);
    ZVAL_LONG(height, h);

    params[0] = &width;
    params[1] = &height;

    if (call_user_function_ex(EG(function_table), NULL, function_name, 
                &return_value, 2, params, 0, NULL TSRMLS_CC) == FAILURE) {
        zend_error(E_ERROR, "Error calling %s function", function_cname);
    }

    FREE_ZVAL(function_name); 
    FREE_ZVAL(width); 
    FREE_ZVAL(height); 

    return return_value;
}
/* }}} */


/* {{{ _php_rgba32_to_gd_image()
 */
int _php_rgba32_to_gd_image(int *src, gdImage *dest, int width, int height) 
{
    int x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            if (gdImageBoundsSafe(dest, x, y)) {
                /* copy frame to gdimage buffer zeroing the alpha channel */
                dest->tpixels[y][x] = src[x] & 0x00ffffff;
            } else {
                return -1;
            }
        }
        src += width;
    }
    return 0;
}
/* }}} */


/* {{{ _php_getframe()
   Returns a frame from the movie. Caller is reponsible for freeing the frame
 */
AVFrame* _php_getframe(ffmovie_context *ffmovie_ctx, int wanted_frame, 
        int wanted_width, int wanted_height, int crop_top, int crop_bottom,
        int crop_left, int crop_right)
{
    int got_frame, video_stream;
    enum PixelFormat target_pixfmt;
    AVPacket packet;
    AVCodecContext *decoder_ctx;
    ImgReSampleContext *img_resample_ctx;
    AVFrame *decoded_frame, *final_frame = NULL;
    AVFrame *yuv_frame, *resampled_frame;

    video_stream = _php_get_stream_index(ffmovie_ctx->fmt_ctx, 
            CODEC_TYPE_VIDEO);
    if (video_stream < 0) {
        return NULL; /* FIXME: Should probably error here */
    }

    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, video_stream);

    decoded_frame = avcodec_alloc_frame();

    /* Rewind to the beginning of the stream if wanted frame already passed */
    if (wanted_frame && wanted_frame < decoder_ctx->frame_number) {
        if (av_seek_frame(ffmovie_ctx->fmt_ctx, -1, 0) < 0) {
            zend_error(E_ERROR, "Error seeking to begining of video stream");
        }
        /* close decoder */
        avcodec_close(ffmovie_ctx->codec_ctx);
        ffmovie_ctx->codec_ctx = NULL;

        /* re-open decoder */
        decoder_ctx = _php_get_decoder_context(ffmovie_ctx, video_stream);
    }

    /* read frames looking for wanted_frame */ 
    while (av_read_frame(ffmovie_ctx->fmt_ctx, &packet) >= 0) {

        if (packet.stream_index == video_stream) {
            avcodec_decode_video(decoder_ctx, decoded_frame, &got_frame,
                    packet.data, packet.size);

            if (got_frame) {

                /* get wanted frame or next frame in stream if called without
                   a frame number */
                if (!wanted_frame || decoder_ctx->frame_number == wanted_frame) {

                    /* is resampling needed */
                    if (wanted_height != decoder_ctx->height ||
                            wanted_width != decoder_ctx->width) {
                        
                        /* convert to PIX_FMT_YUV420P required for resampling */
                        if (decoder_ctx->pix_fmt != PIX_FMT_YUV420P) {

                            yuv_frame = _php_get_context_frame(
                                    &ffmovie_ctx->yuv_ctx_frame,
                                    decoder_ctx->width, decoder_ctx->height, 
                                    PIX_FMT_YUV420P);

                            if (img_convert((AVPicture*)yuv_frame, PIX_FMT_YUV420P, 
                                        (AVPicture *)decoded_frame, decoder_ctx->pix_fmt, 
                                        decoder_ctx->width, decoder_ctx->height) < 0) {
                                zend_error(E_ERROR, "Error converting for resampling");
                            }
                            
                        } else {
                            yuv_frame = decoded_frame;
                        }

                        img_resample_ctx = img_resample_full_init(
                                wanted_width, wanted_height,
                                decoder_ctx->width, decoder_ctx->height,
                                crop_top, crop_bottom, crop_left, crop_right,
                                0, 0, 0, 0);
                        
                        resampled_frame = _php_get_context_frame(
                                &ffmovie_ctx->resample_ctx_frame,
                                wanted_width, wanted_height, PIX_FMT_YUV420P);
                        
                        img_resample(img_resample_ctx, 
                                (AVPicture*)resampled_frame, (AVPicture*)yuv_frame);

                        target_pixfmt = PIX_FMT_YUV420P;
                    } else {
                        target_pixfmt = decoder_ctx->pix_fmt;
                        resampled_frame = decoded_frame;
                    }

                    if (target_pixfmt != PIX_FMT_RGBA32) {

                        final_frame = _php_get_context_frame(
                                &ffmovie_ctx->rgba_ctx_frame,
                                wanted_width, wanted_height, PIX_FMT_RGBA32); 
                            
                        if (img_convert((AVPicture*)final_frame, PIX_FMT_RGBA32,
                                    (AVPicture*)resampled_frame, target_pixfmt,
                                    wanted_width, wanted_height) < 0) {
                            zend_error(E_ERROR, "Can't convert frame");
                        }

                    } else {
                        final_frame = resampled_frame;
                    }
                    
                    /* free wanted frame packet */
                    av_free_packet(&packet);
                    break; 
                }
            }
        }

        /* free the packet allocated by av_read_frame */
        av_free_packet(&packet);
    }

    if (final_frame) {
        return final_frame;
    } else {
        av_free(decoded_frame);
        return NULL;
    }

    return final_frame;
}
/* }}} */

/* {{{ proto resource getFrame([int frame])
 */
PHP_FUNCTION(getFrame)
{
    zval **argv[0], *gd_img_resource;
    gdImage *gd_img;
    int argc, wanted_frame = 0, wanted_width, wanted_height; 
    AVFrame *frame = NULL;
    ffmovie_context *ffmovie_ctx;

    argc = ZEND_NUM_ARGS();

    if (argc > 1) {
        WRONG_PARAM_COUNT;
    }

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    if (argc > 0) {
        /* retrieve arguments */ 
        if (zend_get_parameters_array_ex(argc, argv) != SUCCESS) {
            WRONG_PARAM_COUNT;
        }
        
        convert_to_long_ex(argv[0]);
        wanted_frame = Z_LVAL_PP(argv[0]);

        /* bounds check wanted frame */
        if (wanted_frame < 1) {
            zend_error(E_ERROR, "Frame number must be greater than zero");
        }
    }

    wanted_width = _php_get_framewidth(ffmovie_ctx);
    wanted_height = _php_get_frameheight(ffmovie_ctx); 
    frame = _php_getframe(ffmovie_ctx, wanted_frame, 
            wanted_width, wanted_height, 0, 0, 0, 0);

    if (frame) {

		//dump_img_to_sgi(frame, , wanted_width, wanted_height, "/tmp/test.sgi")
        gd_img_resource = _php_get_gd_image(wanted_width, wanted_height);

        if (!gd_img_resource || gd_img_resource->type != IS_RESOURCE) {
            zend_error(E_ERROR, "Error creating GD Image");
        }

        ZEND_GET_RESOURCE_TYPE_ID(le_gd, "gd");
        ZEND_FETCH_RESOURCE(gd_img, gdImagePtr, &gd_img_resource, -1, "Image", le_gd);
        
        _php_rgba32_to_gd_image((int*)frame->data[0], gd_img, wanted_width, wanted_height);

        _php_free_av_frame(ffmovie_ctx, frame);

        RETURN_RESOURCE(gd_img_resource->value.lval);
    } else {
        RETURN_FALSE
    }
}
/* }}} */


/* {{{ proto resource getFrameResampled(int width, int height [,int frame])
 */
PHP_FUNCTION(getFrameResampled)
{
    zval **argv[6], *gd_img_resource;
    gdImage *gd_img;
    int argc;
    int wanted_frame = 0, wanted_width = 0, wanted_height = 0; 
    int crop_top = 0, crop_bottom = 0, crop_left = 0, crop_right = 0; 
    AVFrame *frame = NULL;
    ffmovie_context *ffmovie_ctx;

    /* get the number of arguments */
    argc = ZEND_NUM_ARGS();

    if (argc > 7 || argc < 2) {
        WRONG_PARAM_COUNT;
    }

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    /* retrieve arguments */ 
    if (zend_get_parameters_array_ex(argc, argv) != SUCCESS) {
        WRONG_PARAM_COUNT;
    }

    /* width arg */
    convert_to_long_ex(argv[0]);
    wanted_width = Z_LVAL_PP(argv[0]);

    /* bounds check wanted width */
    if (wanted_width < 1) {
        zend_error(E_ERROR, "Frame width must be greater than zero");
    }

    /* wanted width must be even number for lavc resample */
    if (wanted_width % 2) {
        zend_error(E_ERROR, "Frame width must be an even number");
    }

    /* height arg */
    convert_to_long_ex(argv[1]);
    wanted_height = Z_LVAL_PP(argv[1]);

    /* bounds check wanted height */
    if (wanted_height < 1) {
        zend_error(E_ERROR, "Frame height must be greater than zero");
    }

    /* wanted height must be even number for lavc resample */
    if (wanted_height % 2) {
        zend_error(E_ERROR, "Frame height must be an even number");
    }

    /* check for optional frame number arg */
    if (argc >= 3) {
        convert_to_long_ex(argv[2]);
        wanted_frame = Z_LVAL_PP(argv[2]);

        /* bounds check wanted frame */
        if (wanted_frame < 1) {
            zend_error(E_ERROR, "Frame number must be greater than zero");
        }
    }

    /* check for optional crop top arg */
    if (argc >= 4) {
        convert_to_long_ex(argv[3]);
        crop_top = Z_LVAL_PP(argv[3]);

        /*  crop top  must be even number for lavc cropping */
        if (crop_top % 2) {
            zend_error(E_ERROR, "Crop top must be an even number");
        }
    }

    /* check for optional crop bottom arg */
    if (argc >= 5) {
        convert_to_long_ex(argv[4]);
        crop_bottom = Z_LVAL_PP(argv[4]);
        
        /*  crop bottom must be even number for lavc cropping */
        if (crop_bottom % 2) {
            zend_error(E_ERROR, "Crop bottom must be an even number");
        }
    }

    /* check for optional crop left arg */
    if (argc >= 6) {
        convert_to_long_ex(argv[5]);
        crop_left = Z_LVAL_PP(argv[5]);

        /*  crop left must be even number for lavc cropping */
        if (crop_left % 2) {
            zend_error(E_ERROR, "Crop left must be an even number");
        }
    }

    /* check for optional crop right arg */
    if (argc >= 7) {
        convert_to_long_ex(argv[6]);
        crop_right = Z_LVAL_PP(argv[6]);

        /*  crop right must be even number for lavc cropping */
        if (crop_right % 2) {
            zend_error(E_ERROR, "Crop right must be an even number");
        }
    }

    frame = _php_getframe(ffmovie_ctx, wanted_frame, 
            wanted_width, wanted_height, 
            crop_top, crop_bottom, crop_left, crop_right);

    if (frame) {

        gd_img_resource = _php_get_gd_image(wanted_width, wanted_height);

        if (!gd_img_resource || gd_img_resource->type != IS_RESOURCE) {
            zend_error(E_ERROR, "Error creating GD Image");
        }

        ZEND_GET_RESOURCE_TYPE_ID(le_gd, "gd");
        ZEND_FETCH_RESOURCE(gd_img, gdImagePtr, &gd_img_resource, -1, "Image", le_gd);

        _php_rgba32_to_gd_image((int*)frame->data[0], gd_img, wanted_width, wanted_height);

        _php_free_av_frame(ffmovie_ctx, frame);
        
        RETURN_RESOURCE(gd_img_resource->value.lval);
    } else {
        RETURN_FALSE
    }
}
/* }}} */

#endif /* HAVE_LIBGD20 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
