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

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_ffmpeg.h"


static int le_ffmpeg_movie;

typedef struct {
    AVFormatContext* ic;
} ffmpegInputMovie;


// Every user visible function must have an entry in ffmpeg_php_functions
zend_function_entry ffmpeg_php_functions[] = {
	ZEND_FE(ffmpeg_movie_open, NULL)
    ZEND_FALIAS(ffmpeg_movie, ffmpeg_movie_open, NULL)
	{NULL, NULL, NULL}
};


// Methods of the ffmpeg_movie class 
zend_function_entry ffmpeg_movie_class_methods[] = {
   
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
    ZEND_FE(getFrame, NULL)
    ZEND_FALIAS(getframe, getFrame, NULL)
*/
	{NULL, NULL, NULL}
};

zend_module_entry ffmpeg_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"ffmpeg",
	ffmpeg_php_functions,
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


static void php_free_ffmpeg_movie(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    ffmpegInputMovie *input = (ffmpegInputMovie*)rsrc->ptr;    
    
    av_close_input_file(input->ic);
	efree(input);
}


static int get_video_stream_index(AVStream *st[])
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


static AVStream *get_video_stream(AVStream *st[])
{
    int i = get_video_stream_index(st);
    
    if (i != -1) {
        return st[i];
    }
    return NULL;
}


ZEND_MINIT_FUNCTION(ffmpeg)
{
	le_ffmpeg_movie = zend_register_list_destructors_ex(php_free_ffmpeg_movie,
            NULL, "ffmovie", module_number);
    
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


ZEND_FUNCTION(ffmpeg_movie_open)
{
    int argc, ret;
    zval **argv[0];
    ffmpegInputMovie *im;
    zend_class_entry ffmpeg_movie_class_entry;
    zend_class_entry *ce;

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
    
    INIT_CLASS_ENTRY(ffmpeg_movie_class_entry, "ffmpeg_movie", 
            ffmpeg_movie_class_methods);

    ce = zend_register_internal_class(&ffmpeg_movie_class_entry TSRMLS_CC);
    
    object_init_ex(return_value, ce);
    add_property_resource(return_value, "ffmovie", ret);
}


ZEND_FUNCTION(getDuration)
{
	zval **tmp;
    ffmpegInputMovie *im;
       
    if (ZEND_NUM_ARGS() != 0)  {
		ZEND_WRONG_PARAM_COUNT();
	}
 
    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmovie", sizeof("ffmovie"), 
                (void **)&tmp) == FAILURE) {
        zend_error(E_ERROR, "Unable to find movie property");
        RETURN_FALSE; 
    }
    
	ZEND_FETCH_RESOURCE(im, ffmpegInputMovie*, tmp, -1, "ffmovie", 
            le_ffmpeg_movie);

    RETURN_DOUBLE((float)im->ic->duration / AV_TIME_BASE);
}


ZEND_FUNCTION(getFrameCount)
{
	zval **tmp;
    ffmpegInputMovie *im;
    AVStream *st;
    AVCodecContext *enc;
    float duration = 0.0, frame_rate = 0.0;

    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmovie", sizeof("ffmovie"), 
                (void **)&tmp) == FAILURE) {
        zend_error(E_ERROR, "Unable to find movie property");
        RETURN_FALSE;
    }
    
	ZEND_FETCH_RESOURCE(im, ffmpegInputMovie*, tmp, -1, "ffmpeg_movie",
            le_ffmpeg_movie);

    st = get_video_stream(im->ic->streams);
    enc = &st->codec;

    duration = (float)im->ic->duration / AV_TIME_BASE;
    frame_rate = (float)enc->frame_rate / enc->frame_rate_base;

    RETURN_LONG(lrint(frame_rate * duration));
}


ZEND_FUNCTION(getFrameRate)
{
	zval **tmp;
    ffmpegInputMovie *im;
    AVStream *st;
    AVCodecContext *enc;
    
    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmovie", sizeof("ffmovie"), 
                (void **)&tmp) == FAILURE) {
        zend_error(E_ERROR, "Unable to find movie property");
        RETURN_FALSE;
    }
    
	ZEND_FETCH_RESOURCE(im, ffmpegInputMovie*, tmp, -1, "ffmpeg_movie",
            le_ffmpeg_movie);

    st = get_video_stream(im->ic->streams);
    enc = &st->codec;

    RETURN_DOUBLE((float)enc->frame_rate / enc->frame_rate_base);
}


ZEND_FUNCTION(getFileName)
{
	zval **tmp;
    ffmpegInputMovie *im;
    
    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmovie", sizeof("ffmovie"), 
                (void **)&tmp) == FAILURE) {
        zend_error(E_ERROR, "Unable to find movie property");
        RETURN_FALSE;
    }
    
	ZEND_FETCH_RESOURCE(im, ffmpegInputMovie*, tmp, -1, "ffmpeg_movie",
            le_ffmpeg_movie);

    RETURN_STRINGL(im->ic->filename, strlen(im->ic->filename), 1);
}


ZEND_FUNCTION(getComment)
{
	zval **tmp;
    ffmpegInputMovie *im;

    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmovie", sizeof("ffmovie"), 
                (void **)&tmp) == FAILURE) {
        zend_error(E_ERROR, "Unable to find movie property");
        RETURN_FALSE;
    }
    
	ZEND_FETCH_RESOURCE(im, ffmpegInputMovie*, tmp, -1, "ffmpeg_movie",
            le_ffmpeg_movie);

    RETURN_STRINGL(im->ic->comment, strlen(im->ic->comment), 1);
}


ZEND_FUNCTION(getTitle)
{
	zval **tmp;
    ffmpegInputMovie *im;
    
    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmovie", sizeof("ffmovie"), 
                (void **)&tmp) == FAILURE) {
        zend_error(E_ERROR, "Unable to find movie property");
        RETURN_FALSE;
    }
    
	ZEND_FETCH_RESOURCE(im, ffmpegInputMovie*, tmp, -1, "ffmpeg_movie",
            le_ffmpeg_movie);

    RETURN_STRINGL(im->ic->title, strlen(im->ic->title), 1);
}


ZEND_FUNCTION(getAuthor)
{
	zval **tmp;
    ffmpegInputMovie *im;
    
    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmovie", sizeof("ffmovie"), 
                (void **)&tmp) == FAILURE) {
        zend_error(E_ERROR, "Unable to find movie property");
        RETURN_FALSE;
    }
    
	ZEND_FETCH_RESOURCE(im, ffmpegInputMovie*, tmp, -1, "ffmpeg_movie",
            le_ffmpeg_movie);

    RETURN_STRINGL(im->ic->author, strlen(im->ic->author), 1);
}


ZEND_FUNCTION(getCopyright)
{
	zval **tmp;
    ffmpegInputMovie *im;
    
    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmovie", sizeof("ffmovie"), 
                (void **)&tmp) == FAILURE) {
        zend_error(E_ERROR, "Unable to find movie property");
        RETURN_FALSE;
    }
    
	ZEND_FETCH_RESOURCE(im, ffmpegInputMovie*, tmp, -1, "ffmpeg_movie",
            le_ffmpeg_movie);

    RETURN_STRINGL(im->ic->copyright, strlen(im->ic->author), 1);
}


static int seek_video(AVFormatContext *fc, int64_t timestamp)
{
    int result;

    result = av_seek_frame(fc, -1, timestamp);

    if (result < 0) {
        return ERROR("av_seek_frame(%lld) failed %d", timestamp, result);
    }

    return SUCCESS;
}


static int64_t calculate_timestamp(AVFormatContext *fc, AVStream *stream, 
        int64_t frame_number)
{
    int64_t timestamp;
    
    timestamp =  av_rescale((int64_t) frame_number * stream->r_frame_rate_base,
            AV_TIME_BASE,
            stream->r_frame_rate);

    // multiply time base
    timestamp = av_rescale(timestamp, stream->time_base.num,
            stream->time_base.den);

    return timestamp;
}

/*
ZEND_FUNCTION(getFrame)
{
	zval *this, **tmp, **argv[0];
    int argc, video_index;
    int64_t timestamp = 0; 
    AVCodec *codec;
    AVStream *st;
    AVFrame *picture;
    AVPacket *pkt;
    AVCodecContext *enc;
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
   
    this = getThis();
    
    if (zend_hash_find(Z_OBJPROP_P(this), "ffmovie", sizeof("ffmovie"), 
                (void **)&tmp) == FAILURE) {
        zend_error(E_ERROR, "Unable to find movie property");
        RETURN_FALSE;
    }
   
	ZEND_FETCH_RESOURCE(im, ffmpegInputMovie*, tmp, -1, "ffmpeg_movie",
            le_ffmpeg_movie);

    // TODO: change all get_video_stream() to get_video_stream_index
    //       and add check for negative return value
    video_index = get_video_stream_index(im->ic->streams);
    
    if (video_index < 0) {
        zend_error(E_ERROR, "Can't find video stream");
        RETURN_FALSE;
    }
    
    st = im->ic->streams[video_index];
    enc = &st->codec;

    // TODO: add check for frame > frame count
    
    // convert frame number to time stamp
    convert_to_long_ex(argv[0]);
    timestamp = calculate_timestamp(im->ic, st, Z_LVAL_PP(argv[0]));

//    zend_printf("timestamp =  %0.3ld\n", timestamp);
    // add the stream start time 
    //if (im->ic->start_time != AV_NOPTS_VALUE)
    //    timestamp += im->ic->start_time;

    if (av_seek_frame(im->ic, -1, timestamp) < 0) {
        zend_error(E_ERROR, "%s: could not seek to position %0.3f\n",
                im->ic->filename, (double)timestamp);
        RETURN_FALSE;
    }

    zend_printf("av read frame = %d\n" , av_read_frame(im->ic, pkt));
    //zend_printf("pkt stream index = %d\n" , pkt->stream_index);
    //zend_printf("video index = %d\n" , video_index);

    // keep reading frames until we get a packet from the right stream
    
    do {
        int result = av_read_frame(im->ic, pkt);
        if(result < 0) {
            zend_error(E_ERROR, "av_read_frame() failed %d", result);
            RETURN_FALSE;
        }
    } while (pkt->stream_index != video_index);

    
    // find the video decoder 
    codec = avcodec_find_decoder(st->codec.codec_id);
    if (!codec) {
        zend_error(E_ERROR, "codec not found\n");
        RETURN_FALSE;
    }

    c = avcodec_alloc_context();

    // open it
    if (avcodec_open(c, codec) < 0) {
        zend_error(E_ERROR, "could not open codec\n");
        RETURN_FALSE;
    }

    picture = avcodec_alloc_frame();

    RETURN_LONG(timestamp);
}
*/

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
