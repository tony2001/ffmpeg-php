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

#include <avcodec.h>
#include <avformat.h>
#include <inttypes.h>
#include <math.h>

#include "gd.h"
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_ffmpeg.h"

#define INBUF_SIZE 4096

#define GET_MOVIE_RESOURCE(im) {\
	zval **tmp;\
    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmpeg_movie", sizeof("ffmpeg_movie"),\
                (void **)&tmp) == FAILURE) {\
        zend_error(E_ERROR, "Unable to find movie property");\
        RETURN_FALSE;\
    }\
\
    ZEND_FETCH_RESOURCE(im, ffmpegInputMovie*, tmp, -1, "ffmpeg_movie",\
            le_ffmpeg_movie);\
}\


static int le_ffmpeg_movie;

static zend_class_entry *ffmpeg_movie_class_entry_ptr;

zend_class_entry ffmpeg_movie_class_entry;

typedef struct {
    AVFormatContext* ic;
} ffmpegInputMovie;


// Methods of the ffmpeg_movie class 
zend_function_entry ffmpeg_movie_class_methods[] = {
   
	ZEND_FE(ffmpeg_movie, NULL)

	ZEND_FE(getDuration, NULL)
    ZEND_FALIAS(getduration, getDuration, NULL)

	ZEND_FE(getFrameCount, NULL)
    ZEND_FALIAS(getframecount, getFrameCount, NULL)

	ZEND_FE(getFrameRate, NULL)
    ZEND_FALIAS(getframerate, getFrameRate, NULL)
    
	ZEND_FE(getFileName, NULL)
    ZEND_FALIAS(getfilename, getFileName, NULL)
    
	ZEND_FE(getComment, NULL)
    ZEND_FALIAS(getcomment, getComment, NULL)
 
	ZEND_FE(getTitle, NULL)
    ZEND_FALIAS(gettitle, getTitle, NULL)

    ZEND_FE(getAuthor, NULL)
    ZEND_FALIAS(getauthor, getAuthor, NULL)
 
	ZEND_FE(getCopyright, NULL)
    ZEND_FALIAS(getcopyright, getCopyright, NULL)

/*
    ZEND_FE(hasVideo, NULL)
    ZEND_FALIAS(hasvideo, hasVideo, NULL)

    ZEND_FE(hasAudio, NULL)
    ZEND_FALIAS(hasaudio, hasAudio, NULL)
*/
    
    ZEND_FE(getFrame, NULL)
    ZEND_FALIAS(getframe, getFrame, NULL)

	{NULL, NULL, NULL}
};

zend_module_entry ffmpeg_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"ffmpeg",
	NULL,
	ZEND_MINIT(ffmpeg),
	ZEND_MSHUTDOWN(ffmpeg),
	NULL,
	NULL,
	ZEND_MINFO(ffmpeg),
#if ZEND_MODULE_API_NO >= 20010901
	"0.2", // version number for ffmpeg-php 
#endif
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_FFMPEG
ZEND_GET_MODULE(ffmpeg)
#endif


static void _php_free_ffmpeg_movie(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    ffmpegInputMovie *input = (ffmpegInputMovie*)rsrc->ptr;    
    
    av_close_input_file(input->ic);
	efree(input);
}


static int _php__php_get_video_stream_index(AVStream *st[])
{
    int i;
    for (i = 0; i < MAX_STREAMS; i++) {
        if (st[i]->codec.codec_type == CODEC_TYPE_VIDEO) {
            return i;
        }
    }
    // no video found
    return -1;
}

static int _php_get_audio_stream_index(AVStream *st[])
{
    int i;
    for (i = 0; i < MAX_STREAMS; i++) {
        if (st[i]->codec.codec_type == CODEC_TYPE_AUDIO) {
            zend_printf("found audio\n");
            return i;
        }
    }
    // no audio found
    zend_printf("no found audio\n");
    return -1;
}

static AVStream *_php_get_video_stream(AVStream *st[])
{
    int i = _php__php_get_video_stream_index(st);
    
    if (i != -1) {
        return st[i];
    }
    return NULL;
}


ZEND_MINIT_FUNCTION(ffmpeg)
{
	le_ffmpeg_movie = zend_register_list_destructors_ex(_php_free_ffmpeg_movie,
            NULL, "ffmpeg_movie", module_number);

    INIT_CLASS_ENTRY(ffmpeg_movie_class_entry, "ffmpeg_movie", 
            ffmpeg_movie_class_methods);
    
    // register ffmpeg movie class
    ffmpeg_movie_class_entry_ptr = 
        zend_register_internal_class(&ffmpeg_movie_class_entry TSRMLS_CC);
    

    // must be called before using avcodec libraries. 
    avcodec_init();

    // register all codecs
    av_register_all();
    return SUCCESS;
}


ZEND_MSHUTDOWN_FUNCTION(ffmpeg)
{
    av_free_static();
	return SUCCESS;
}


// Add an entry for ffmpeg support in phpinfo() 
ZEND_MINFO_FUNCTION(ffmpeg)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ffmpeg support", "enabled");
	php_info_print_table_end();
}


ZEND_FUNCTION(ffmpeg_movie)
{
    int argc, ret;
    zval **argv[0];
    ffmpegInputMovie *im;

    AVFormatParameters params, *ap = &params;
    
    /* get the number of arguments */
    argc = ZEND_NUM_ARGS();

    if(argc != 1) {
        WRONG_PARAM_COUNT;
    }

    /* argument count is correct, now retrieve arguments */
    if(zend_get_parameters_array_ex(argc, argv) != SUCCESS) {
        WRONG_PARAM_COUNT;
    }
  
	im = emalloc(sizeof(ffmpegInputMovie));
    
    convert_to_string_ex(argv[0]);
    
    /* open the file with generic libav function */
    if (av_open_input_file(&(im->ic), Z_STRVAL_PP(argv[0]), NULL, 0, ap)) {
        zend_error(E_ERROR, "Can't open movie file");
    }
    
    /* If not enough info to get the stream parameters, we decode the
       first frames to get it. */
    if (av_find_stream_info(im->ic)) {
        zend_error(E_ERROR, "Can't find codec parameters for movie\n");
    }

    // pass null for resource result since we're not returning the resource
    // directly, but adding it to the returned object.
	ret = ZEND_REGISTER_RESOURCE(NULL, im, le_ffmpeg_movie);
    
    object_init_ex(getThis(), &ffmpeg_movie_class_entry);
    add_property_resource(getThis(), "ffmpeg_movie", ret);
}


ZEND_FUNCTION(getDuration)
{
    ffmpegInputMovie *im;
       
    if (ZEND_NUM_ARGS() != 0)  {
		ZEND_WRONG_PARAM_COUNT();
	}
    
    GET_MOVIE_RESOURCE(im);
    
    RETURN_DOUBLE((float)im->ic->duration / AV_TIME_BASE);
}


ZEND_FUNCTION(getFrameCount)
{
    ffmpegInputMovie *im;
    AVStream *st;
    float duration = 0.0, frame_rate = 0.0;

    GET_MOVIE_RESOURCE(im);

    st = _php_get_video_stream(im->ic->streams);

    duration = (float)im->ic->duration / AV_TIME_BASE;
    frame_rate = (float)st->codec.frame_rate / st->codec.frame_rate_base;

    RETURN_LONG(lrint(frame_rate * duration));
}


ZEND_FUNCTION(getFrameRate)
{
    ffmpegInputMovie *im;
    AVStream *st;
    AVCodecContext *enc;
    
    GET_MOVIE_RESOURCE(im);

    st = _php_get_video_stream(im->ic->streams);
    enc = &st->codec;

    RETURN_DOUBLE((float)enc->frame_rate / enc->frame_rate_base);
}


static char* _php_get_filename(ffmpegInputMovie *im) {
    return im->ic->filename;
}


ZEND_FUNCTION(getFileName)
{
    ffmpegInputMovie *im;
    char* filename;
    
    GET_MOVIE_RESOURCE(im);

    filename = _php_get_filename(im);
    RETURN_STRINGL(filename, strlen(filename), 1);
}


ZEND_FUNCTION(getComment)
{
    ffmpegInputMovie *im;

    GET_MOVIE_RESOURCE(im);
    
    RETURN_STRINGL(im->ic->comment, strlen(im->ic->comment), 1);
}


ZEND_FUNCTION(getTitle)
{
    ffmpegInputMovie *im;
    
    GET_MOVIE_RESOURCE(im);

    RETURN_STRINGL(im->ic->title, strlen(im->ic->title), 1);
}


ZEND_FUNCTION(getAuthor)
{
    ffmpegInputMovie *im;
    
    GET_MOVIE_RESOURCE(im);

    RETURN_STRINGL(im->ic->author, strlen(im->ic->author), 1);
}


ZEND_FUNCTION(getCopyright)
{
    ffmpegInputMovie *im;
    
    GET_MOVIE_RESOURCE(im);

    RETURN_STRINGL(im->ic->copyright, strlen(im->ic->author), 1);
}


static void pgm_save(unsigned char *buf,int wrap, int xsize,int ysize,char *filename) 
{
    FILE *f;
    int i;

    f=fopen(filename,"w");
    fprintf(f,"P5\n%d %d\n%d\n",xsize,ysize,255);
    for(i=0;i<ysize;i++)
        fwrite(buf + i * wrap,1,xsize,f);
    fclose(f);
}


_php_copy_frame_to_gd(AVFrame *pict)
{
    zval func;
    zval retval;
    zval param;

    if (!zend_hash_exists(EG(function_table), "imagecreatetruecolor", sizeof("imagecreatetruecolor"))) {
        zend_error(E_ERROR, "ffmpeg getFrame function requires GD\n");
        return NULL;
    }
    ZVAL_STRING(&func, "imagecreatetruecolor", 0);
    INIT_ZVAL(param); 


    // get imagecreatetruecolor function
    // call imagecreatetruecolor and get a gdImagePtr resource from gd
    // fill gdImagePtr with pict data
    // return gdImagePtr resource 
}


ZEND_FUNCTION(getFrame)
{
	zval **argv[0];
    int argc;
    int frame, size, got_picture, len;
    FILE *f;
    char *outfilename = "outtest.mpg";
    uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE], *inbuf_ptr;
    char buf[1024];
    AVCodec *codec;
    AVStream *st;
    AVFrame *pict;
    AVCodecContext *c= NULL;
    
    ffmpegInputMovie *im;

    // get the number of arguments 
    argc = ZEND_NUM_ARGS();

    if(argc != 1) {
        WRONG_PARAM_COUNT;
    }

    // retrieve arguments 
    if(zend_get_parameters_array_ex(argc, argv) != SUCCESS) {
        WRONG_PARAM_COUNT;
    }
   
    GET_MOVIE_RESOURCE(im);
    
    /* set end of buffer to 0 (this ensures that no overreading happens for damaged mpeg streams) */
    memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);

    printf("Video decoding\n");

    st = _php_get_video_stream(im->ic->streams);

    /* find the mpeg1 video decoder */
    codec = avcodec_find_decoder(st->codec.codec_id);
    if (!codec) {
        fprintf(stderr, "codec not found\n");
        exit(1);
    }

    c = avcodec_alloc_context();
    pict = avcodec_alloc_frame();

    if(codec->capabilities&CODEC_CAP_TRUNCATED)
        c->flags|= CODEC_FLAG_TRUNCATED; /* we dont send complete frames */

    /* for some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because these info are not available
       in the bitstream */

    /* open it */
    if (avcodec_open(c, codec) < 0) {
        fprintf(stderr, "could not open codec\n");
        exit(1);
    }
    
    /* the codec gives us the frame size, in samples */
    f = fopen(im->ic->filename, "rb");
    if (!f) {
        fprintf(stderr, "could not open %s\n", im->ic->filename);
        exit(1);
    }
    
    convert_to_long_ex(argv[0]);
    
    zend_printf("argv[0] == %ld\n", Z_LVAL_PP(argv[0]));
    
    frame = 0;
    for(;;) {
        size = fread(inbuf, 1, INBUF_SIZE, f);
        if (size == 0)
            break;

        /* NOTE1: some codecs are stream based (mpegvideo, mpegaudio)
           and this is the only method to use them because you cannot
           know the compressed data size before analysing it. 

           BUT some other codecs (msmpeg4, mpeg4) are inherently frame
           based, so you must call them with all the data for one
           frame exactly. You must also initialize 'width' and
           'height' before initializing them. */

        /* NOTE2: some codecs allow the raw parameters (frame size,
           sample rate) to be changed at any frame. We handle this, so
           you should also take care of it */

        /* here, we use a stream based decoder (mpeg1video), so we
           feed decoder and see if it could decode a frame */
        inbuf_ptr = inbuf;
        while (size > 0) {
            len = avcodec_decode_video(c, pict, &got_picture, inbuf_ptr, size);
            if (len < 0) {
                fprintf(stderr, "Error while decoding frame %d\n", frame);
                exit(1);
            }
            if (got_picture) {
                if (frame == Z_LVAL_PP(argv[0])) {
                    _php_copy_frame_to_gd(pict);
                    
                    //pgm_save(pict->data[0], pict->linesize[0], 
                    //      c->width, c->height, buf);
                    goto getframe_done;
                }
                frame++;
            }
            size -= len;
            inbuf_ptr += len;
        }
    }

    /* some codecs, such as MPEG, transmit the I and P frame with a
       latency of one frame. You must do the following to have a
       chance to get the last frame of the video */
    len = avcodec_decode_video(c, pict, &got_picture, NULL, 0);
    if (got_picture) {
        if (frame == Z_LVAL_PP(argv[0])) {
            printf("saving last frame %3d\n", frame);
            _php_copy_frame_to_gd(pict);
        }
        frame++;
    }
        

getframe_done:
    fclose(f);

    avcodec_close(c);
    free(c);
    free(pict);
    
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
