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
    PHP_FE(getFrameAsGDImage, NULL)
    PHP_FALIAS(getframeasgdimage, getFrameAsGDImage, NULL)
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
static int _php_get_stream_index(AVStream *st[], int type)
{
    int i;
    for (i = 0; i < MAX_STREAMS; i++) {
        if (st[i]->codec.codec_type == type) {
            return i;
        }
    }
    /* stream not found */
    return -1;
}
/* }}} */


/* {{{ _php_get_video_stream()
 */
static AVStream *_php_get_video_stream(AVStream *st[])
{
    int i = _php_get_stream_index(st, CODEC_TYPE_VIDEO);
    
    return i < 0 ? NULL : st[i];
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
    AVStream *st = _php_get_video_stream(im->ic->streams);

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
    AVStream *st = _php_get_video_stream(im->ic->streams);

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
    AVStream *st = _php_get_video_stream(im->ic->streams);

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
    AVStream *st = _php_get_video_stream(im->ic->streams);

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


/* {{{ _php_avpicture_to_gd_image()
 */
zval* _php_avpicture_to_gd_image(AVPicture *av_pict, gdImagePtr gd_img, 
        int width, int height) 
{
    int x, y, row_start;
    int *data = (int*)av_pict->data[0];

    for (y = 0; y < height; y++) {
        row_start = y * width;
        //zend_printf("writing %d to row %d\n", data[row_start], row_start);
        for (x = 0; x < width; x++) {
            gdImageSetPixel(gd_img, x, y, data[row_start + x]); 
        }
    }
}
/* }}} */

#if HAVE_LIBGD20

/* {{{ proto resource getFrameAsGDImage(int frame)
 */
PHP_FUNCTION(getFrameAsGDImage)
{
	zval **argv[0], *gd_img_resource;
    gdImage *im;
    int argc, frame, size, got_picture, len;
    FILE *f;
    uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE], *inbuf_ptr;
    char buf[1024];
    AVCodec *codec;
    AVStream *st;
    AVFrame *src;
    AVPicture tmp_pict, *av_pict;
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

    st = _php_get_video_stream(ffmovie_ctx->ic->streams);
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
   
    zend_printf("pix fmt = %s\n", avcodec_get_pix_fmt_name(c->pix_fmt)); 
    /* make sure frame data is RGBA32 */
    if (c->pix_fmt != PIX_FMT_RGBA32) {
        zend_printf("w: %d, h: %d\n", c->width, c->height);
        avpicture_alloc(&tmp_pict, PIX_FMT_RGBA32, c->width, c->height);
       
        if (img_convert(&tmp_pict, PIX_FMT_RGBA32,
                    (AVPicture*)src, c->pix_fmt, c->width, c->height) < 0) {
            zend_error(E_ERROR, "Error converting frame");
        }
        
        av_pict = &tmp_pict;
    } else {
        av_pict = (AVPicture*)src;
    }

    _php_avpicture_to_gd_image(av_pict, im, c->width, c->height);

    fclose(f);
    avcodec_close(c);
    av_free(c);
    
    // TODO: check if we should free using avpicture_free or not.
    av_free(src);
   
    /* FIXME
    if (tmp.data[0]) {
        avpicture_free(&tmp_pict);
    }*/

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
