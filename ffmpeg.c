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

// FIXME: make this work without the ../../ when compiling standalone 
#if HAVE_GD_BUNDLED
  #include "../../ext/gd/libgd/gd.h"
#else
  #include "gd.h"
#endif

#define INBUF_SIZE 4096

#define GET_MOVIE_RESOURCE(im) {\
	zval **_tmp_zval;\
    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmpeg_movie", sizeof("ffmpeg_movie"),\
                (void **)&_tmp_zval) == FAILURE) {\
        zend_error(E_ERROR, "Unable to find ffmpeg_movie property");\
        RETURN_FALSE;\
    }\
\
    ZEND_FETCH_RESOURCE(im, ffmpeg_movie_context*, _tmp_zval, -1, "ffmpeg_movie",\
            le_ffmpeg_movie);\
}\


static int le_ffmpeg_movie;

static int le_gd;

static zend_class_entry *ffmpeg_movie_class_entry_ptr;

zend_class_entry ffmpeg_movie_class_entry;

typedef struct {
    AVFormatContext* ic;
} ffmpeg_movie_context;


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

#if HAVE_LIBGD20
    PHP_FE(getFrame, NULL)
    PHP_FALIAS(getframe, getFrame, NULL)
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
	"0.2", /* version number for ffmpeg-php */
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
    ffmpeg_movie_context *input = (ffmpeg_movie_context*)rsrc->ptr;    
    
    av_close_input_file(input->ic);
	efree(input);
}
/* }}} */


/* {{{ _php_get_stream_index()
 */
static int _php_get_stream_index(AVFormatContext *ic, int type)
{
    int i;
    
    for (i = 0; i < ic->nb_streams; i++) {
        if (ic->streams[i] && ic->streams[i]->codec.codec_type == type) {
            return i;
        }
    }
    /* stream not found */
    return -1;
}
/* }}} */


/* {{{ _php_get_video_stream()
 */
static AVStream *_php_get_video_stream(AVFormatContext *ic)
{
    int i = _php_get_stream_index(ic, CODEC_TYPE_VIDEO);
    
    return i < 0 ? NULL : ic->streams[i];
}
/* }}} */


/* {{{ php module init function
 */
PHP_MINIT_FUNCTION(ffmpeg)
{
	le_ffmpeg_movie = zend_register_list_destructors_ex(_php_free_ffmpeg_movie,
            NULL, "ffmpeg_movie", module_number);

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
static void _php_open_movie_file(ffmpeg_movie_context *im, char* filename)
{
    AVFormatParameters params;

    /* open the file with generic libav function */
    if (av_open_input_file(&(im->ic), filename, NULL, 0, &params)) {
        zend_error(E_ERROR, "Can't open movie file %s", filename);
    }
    
    /* If not enough info to get the stream parameters, we decode the
       first frames to get it. */
    if (av_find_stream_info(im->ic)) {
        zend_error(E_ERROR, "Can't find codec parameters for %s", filename);
    }
}
/* }}} */


/* {{{ proto object ffmpeg_movie(string filename) 
   Constructor for ffmpeg_movie objects
 */
PHP_FUNCTION(ffmpeg_movie)
{
    int argc, ret;
    zval **argv[0];
    ffmpeg_movie_context *im;
    
    /* get the number of arguments */
    argc = ZEND_NUM_ARGS();

    if(argc != 1) {
        WRONG_PARAM_COUNT;
    }

    /* argument count is correct, now retrieve arguments */
    if(zend_get_parameters_array_ex(argc, argv) != SUCCESS) {
        WRONG_PARAM_COUNT;
    }
  
	im = emalloc(sizeof(ffmpeg_movie_context));
    
    convert_to_string_ex(argv[0]);
    
    _php_open_movie_file(im, Z_STRVAL_PP(argv[0]));

    /* pass NULL for resource result since we're not returning the resource
     directly, but adding it to the returned object. */
	ret = ZEND_REGISTER_RESOURCE(NULL, im, le_ffmpeg_movie);
    
    object_init_ex(getThis(), &ffmpeg_movie_class_entry);
    add_property_resource(getThis(), "ffmpeg_movie", ret);
}
/* }}} */


/* {{{ proto int getDuration()
 */
PHP_FUNCTION(getDuration)
{
    ffmpeg_movie_context *im;
       
    GET_MOVIE_RESOURCE(im);
    
    RETURN_DOUBLE((float)im->ic->duration / AV_TIME_BASE);
}
/* }}} */


/* {{{ _php_get_framecount()
 */
static long _php_get_framecount(ffmpeg_movie_context *im)
{
    float duration = 0.0, frame_rate = 0.0;
    AVStream *st = _php_get_video_stream(im->ic);

    duration = (float)im->ic->duration / AV_TIME_BASE;
    frame_rate = (float)st->codec.frame_rate / st->codec.frame_rate_base;

    return lrint(frame_rate * duration);
}
/* }}} */


/* {{{ proto int getFrameCount()
 */
PHP_FUNCTION(getFrameCount)
{
    ffmpeg_movie_context *im;
    GET_MOVIE_RESOURCE(im);
    RETURN_LONG(_php_get_framecount(im));
}
/* }}} */


/* {{{ _php_get_framerate()
 */
static float _php_get_framerate(ffmpeg_movie_context *im)
{
    AVStream *st = _php_get_video_stream(im->ic);

    return (float)st->codec.frame_rate / st->codec.frame_rate_base;
}
/* }}} */


/* {{{ proto int getFrameRate()
 */
PHP_FUNCTION(getFrameRate)
{
    ffmpeg_movie_context *im;
    GET_MOVIE_RESOURCE(im);
    RETURN_DOUBLE(_php_get_framerate(im));
}
/* }}} */


/* {{{ _php_get_filename()
 */
static char* _php_get_filename(ffmpeg_movie_context *im)
{
    return im->ic->filename;
}
/* }}} */


/* {{{ proto string getFileName()
 */
PHP_FUNCTION(getFileName)
{
    ffmpeg_movie_context *im;
    char* filename;
    
    GET_MOVIE_RESOURCE(im);

    filename = _php_get_filename(im);
    RETURN_STRINGL(filename, strlen(filename), 1);
}
/* }}} */


/* {{{ proto string getComment()
 */
PHP_FUNCTION(getComment)
{
    ffmpeg_movie_context *im;

    GET_MOVIE_RESOURCE(im);
    
    RETURN_STRINGL(im->ic->comment, strlen(im->ic->comment), 1);
}
/* }}} */


/* {{{ proto string getTitle()
 */
PHP_FUNCTION(getTitle)
{
    ffmpeg_movie_context *im;
    
    GET_MOVIE_RESOURCE(im);

    RETURN_STRINGL(im->ic->title, strlen(im->ic->title), 1);
}
/* }}} */


/* {{{ proto string getAuthor()
 */
PHP_FUNCTION(getAuthor)
{
    ffmpeg_movie_context *im;
    
    GET_MOVIE_RESOURCE(im);

    RETURN_STRINGL(im->ic->author, strlen(im->ic->author), 1);
}
/* }}} */


/* {{{ proto string getCopyright()
 */
PHP_FUNCTION(getCopyright)
{
    ffmpeg_movie_context *im;
    
    GET_MOVIE_RESOURCE(im);

    RETURN_STRINGL(im->ic->copyright, strlen(im->ic->author), 1);
}
/* }}} */

/* {{{ _php_get_framewidth()
 */
static float _php_get_framewidth(ffmpeg_movie_context *im)
{
    AVStream *st = _php_get_video_stream(im->ic);

    return st->codec.width;
}
/* }}} */


/* {{{ proto int getFrameWidth()
 */
PHP_FUNCTION(getFrameWidth) {
    ffmpeg_movie_context *im;
    GET_MOVIE_RESOURCE(im);
    RETURN_LONG(_php_get_framewidth(im));
}


/* {{{ _php_get_frameheight()
 */
static float _php_get_frameheight(ffmpeg_movie_context *im)
{
    AVStream *st = _php_get_video_stream(im->ic);

    return st->codec.height;
}
/* }}} */


/* {{{ proto int getFrameHeight()
 */
PHP_FUNCTION(getFrameHeight) {
    ffmpeg_movie_context *im;
    GET_MOVIE_RESOURCE(im);
    RETURN_LONG(_php_get_frameheight(im));
}


/* {{{ _php_get_pixelformat()
 */
/*
   static float _php_get_pixelformat(ffmpeg_movie_context *im)
{
    AVStream *st = _php_get_video_stream(im->ic);
    zend_printf("pix fmt = %s\n", avcodec_get_pix_fmt_name(c->pix_fmt)); 
    return st->codec.height;
}
*/
/* }}} */


/* {{{ proto int getPixelFormat()
 */
/*
PHP_FUNCTION(getPixelFormat) {
    ffmpeg_movie_context *im;
    char *fmt;
    GET_MOVIE_RESOURCE(im);
   
    fmt = _php_get_pixelformat(im);
    RETURN_STRINGL(fmt, strlen(fmt), 1);
}
*/


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
    
    if(call_user_function_ex(EG(function_table), NULL, function_name, 
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
                dest->tpixels[y][x] = src[x];
            } else {
                return -1;
            }
        }
        src += width;
    }
    return 0;
}
/* }}} */

#if HAVE_LIBGD20

/* {{{ proto resource getFrame(int frame)
 */
PHP_FUNCTION(getFrame)
{
	zval **argv[0], *gd_img_resource;
    gdImage *im;
    int argc, frame, size, got_picture, len;
    FILE *f;
    uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE], *inbuf_ptr;
    uint8_t *tmp_buf = NULL;
    char buf[1024];
    AVCodec *codec;
    AVStream *st;
    AVFrame *src, tmp_pict, *av_pict;
    AVCodecContext *c= NULL;
    
    ffmpeg_movie_context *ffmovie_ctx;

    /* get the number of arguments */
    argc = ZEND_NUM_ARGS();

    if(argc != 1) {
        WRONG_PARAM_COUNT;
    }

    /* retrieve arguments */ 
    if(zend_get_parameters_array_ex(argc, argv) != SUCCESS) {
        WRONG_PARAM_COUNT;
    }
   
    GET_MOVIE_RESOURCE(ffmovie_ctx);
    
    /* set end of buffer to 0 (this ensures that no overreading happens for
       damaged mpeg streams) */
    memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);

    st = _php_get_video_stream(ffmovie_ctx->ic);
    if (!st) {
        zend_error(E_ERROR, "Video stream not found in %s",
                _php_get_filename(ffmovie_ctx));
    }

    /* find the decoder */
    codec = avcodec_find_decoder(st->codec.codec_id);
    if (!codec) {
        zend_error(E_ERROR, "Codec not found for %s", 
                _php_get_filename(ffmovie_ctx));
    }

    c = avcodec_alloc_context();
    src = avcodec_alloc_frame();

    if(codec->capabilities&CODEC_CAP_TRUNCATED)
        c->flags|= CODEC_FLAG_TRUNCATED; /* we dont send complete frames */

    /* open it */
    if (avcodec_open(c, codec) < 0) {
        // TODO: must clean up before erroring
        zend_error(E_ERROR, "Could not open codec for %s", 
                _php_get_filename(ffmovie_ctx));
    }

    /* the codec gives us the frame size, in samples */
    f = fopen(ffmovie_ctx->ic->filename, "rb");
    if (!f) {
        // TODO: must clean up before erroring
        zend_error(E_ERROR, "Could not open %s", 
                _php_get_filename(ffmovie_ctx));
    }
    
    convert_to_long_ex(argv[0]);
    
    frame = 0;
    for(;;) {
        size = fread(inbuf, 1, INBUF_SIZE, f);
        if (size == 0) {
            break;
        }

        inbuf_ptr = inbuf;
        while (size > 0) {
            len = avcodec_decode_video(c, src, &got_picture, inbuf_ptr, size);
            if (len < 0) {
                // TODO: must clean up before erroring
                zend_error(E_ERROR, "Error while decoding frame %d of %s", 
                        frame, _php_get_filename(ffmovie_ctx));
            }
            if (got_picture) {
                if (frame == Z_LVAL_PP(argv[0])) {
                    goto found_frame;
                }
                frame++;
            }
            size -= len;
            inbuf_ptr += len;
        }
    }

    /* get last frame */
    len = avcodec_decode_video(c, src, &got_picture, NULL, 0);
    if (got_picture) {
        if (frame == Z_LVAL_PP(argv[0])) {
            goto found_frame;
        }
        frame++;
    }
        
found_frame:
    gd_img_resource = _php_get_gd_image(c->width, c->height);
    
    if (!gd_img_resource || gd_img_resource->type != IS_RESOURCE) {
       zend_error(E_ERROR, "Error creating GD Image");
    }
    
    ZEND_GET_RESOURCE_TYPE_ID(le_gd, "gd");
    ZEND_FETCH_RESOURCE(im, gdImagePtr, &gd_img_resource, -1, "Image", le_gd);
   
    /* make sure frame data is RGBA32 */
    if (c->pix_fmt != PIX_FMT_RGBA32) {
        int size;

        /* create temporary picture */
        size = avpicture_get_size(PIX_FMT_RGBA32, c->width, c->height);
        tmp_buf = av_malloc(size);
        if (!tmp_buf) {
            return;
        }

        av_pict = &tmp_pict;
        avpicture_fill((AVPicture*)av_pict, tmp_buf, PIX_FMT_RGBA32, 
                c->width, c->height);

        if (img_convert((AVPicture*)av_pict, PIX_FMT_RGBA32,
                    (AVPicture*)src, c->pix_fmt, c->width, c->height) < 0) {
            zend_error(E_ERROR, "Error converting frame");
        }
        
    } else {
        av_pict = src;
    }

    _php_rgba32_to_gd_image((int*)av_pict->data[0], im, c->width, c->height);

    /* DEBUG: This block writss an sgi image of the choosen frame to
       disk as out.sgi using libavformat instead of GD.
    {
        int err;
        char *fn = "out.sgi";
        AVImageFormat *image_fmt;
        AVImageInfo img_info;
        ByteIOContext pb;
        for(image_fmt = first_image_format; image_fmt != NULL;
                image_fmt = image_fmt->next) {
            if (strncmp(image_fmt->name, "sgi", 3) == 0) {
                break;
            }
        }

        img_info.pict.data[0] = av_pict->data[0];
        img_info.pict.linesize[0] = av_pict->linesize[0];
        img_info.pix_fmt = PIX_FMT_RGBA32;
        img_info.width = c->width;
        img_info.height = c->height;
        img_info.interleaved = 0;

        // open the file with generic libav function 
        err = url_fopen(&pb, fn, URL_RDWR);
        if (err < 0) {
            fprintf(stderr, "Could not open %s %d\n", fn, err);
            exit(-8);
        }
        url_setbufsize(&pb, 4096);
        image_fmt->img_write(&pb, &img_info);
    }
     */
    
    fclose(f);
    avcodec_close(c);
    av_free(c);
    av_free(tmp_buf);
    
    // TODO: check if we should free using avpicture_free or not.
    av_free(src);
   
    RETURN_RESOURCE(gd_img_resource->value.lval);
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
