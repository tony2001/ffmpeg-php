#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"

#include "php_ffmpeg.h"

#include "ffmpeg_frame.h"
#include "ffmpeg_movie.h"
   
#define GET_MOVIE_RESOURCE(ff_movie_ctx) {\
    zval **_tmp_zval;\
    if (zend_hash_find(Z_OBJPROP_P(getThis()), "ffmpeg_movie",\
                sizeof("ffmpeg_movie"), (void **)&_tmp_zval) == FAILURE) {\
        zend_error(E_WARNING, "Invalid ffmpeg_movie object");\
        RETURN_FALSE;\
    }\
\
    ZEND_FETCH_RESOURCE2(ff_movie_ctx, ff_movie_context*, _tmp_zval, -1,\
            "ffmpeg_movie", le_ffmpeg_movie, le_ffmpeg_pmovie);\
}\

#define LRINT(x) ((long) ((x)+0.5))

#if LIBAVFORMAT_BUILD > 4628
#define GET_CODEC_FIELD(codec, field) codec->field
#define GET_CODEC_PTR(codec) codec 
#else
#define GET_CODEC_FIELD(codec, field) codec.field
#define GET_CODEC_PTR(codec) &codec
#endif

static zend_class_entry *ffmpeg_movie_class_entry_ptr;
zend_class_entry ffmpeg_movie_class_entry;

static int le_ffmpeg_movie;
static int le_ffmpeg_pmovie;

/* {{{ ffmpeg_movie methods[]
    Methods of the ffmpeg_movie class 
*/
zend_function_entry ffmpeg_movie_class_methods[] = {
   
    /* contructor */
    PHP_FE(ffmpeg_movie, NULL)

    /* methods */
    PHP_FALIAS(getduration,         getDuration,         NULL)
    PHP_FALIAS(getframecount,       getFrameCount,       NULL)
    PHP_FALIAS(getframerate,        getFrameRate,        NULL)
    PHP_FALIAS(getfilename,         getFileName,         NULL)
    PHP_FALIAS(getcomment,          getComment,          NULL)
    PHP_FALIAS(gettitle,            getTitle,            NULL)
    PHP_FALIAS(getauthor,           getAuthor,           NULL)
    PHP_FALIAS(getartist,           getAuthor,           NULL)
    PHP_FALIAS(getcopyright,        getCopyright,        NULL)
    PHP_FALIAS(getalbum,            getAlbum,            NULL)
    PHP_FALIAS(getgenre,            getGenre,            NULL)
    PHP_FALIAS(getyear,             getYear,             NULL)
    PHP_FALIAS(gettracknumber,      getTrackNumber,      NULL)
    PHP_FALIAS(getframewidth,       getFrameWidth,       NULL)
    PHP_FALIAS(getframeheight,      getFrameHeight,      NULL)
    PHP_FALIAS(getframenumber,      getFrameNumber,      NULL)
    PHP_FALIAS(getpixelformat,      getPixelFormat,      NULL)
    PHP_FALIAS(getbitrate,          getBitRate,          NULL)
    PHP_FALIAS(hasaudio,            hasAudio,            NULL)
    PHP_FALIAS(getnextkeyframe,     getNextKeyFrame,     NULL)
    PHP_FALIAS(getframe,            getFrame,            NULL)
    PHP_FALIAS(getvideocodec,       getVideoCodec,       NULL)
    PHP_FALIAS(getaudiocodec,       getAudioCodec,       NULL)
    PHP_FALIAS(getaudiochannels,    getAudioChannels,    NULL)
    PHP_FALIAS(getpixelaspectratio, getPixelAspectRatio, NULL)
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
                GET_CODEC_FIELD(fmt_ctx->streams[i]->codec, codec_type) == type) {
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
 * TODO: Some containers can have multiple audio streams, so this
 *       will eventually need to be replaced by something smarter
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
static ff_movie_context* _php_alloc_ffmovie_ctx(int persistent)
{
    int i;
    ff_movie_context *ffmovie_ctx;
    
    ffmovie_ctx = persistent ? malloc(sizeof(ff_movie_context)) : 
                               emalloc(sizeof(ff_movie_context));
    ffmovie_ctx->fmt_ctx = NULL;
    ffmovie_ctx->frame_number = 0;

    for (i = 0; i < MAX_STREAMS; i++) {
        ffmovie_ctx->codec_ctx[i] = NULL;
    }

    return ffmovie_ctx;
}
/* }}} */


/* {{{ _php_open_movie_file()
 */
static int _php_open_movie_file(ff_movie_context *ffmovie_ctx, 
        char* filename)
{
    AVFormatParameters params;

    if (ffmovie_ctx->fmt_ctx) {
        av_close_input_file(ffmovie_ctx->fmt_ctx);
    }
    
    /* open the file with generic libav function */
    if (av_open_input_file(&(ffmovie_ctx->fmt_ctx), filename, NULL, 0, 
                &params)) {
        return -1;
    }
    
    /* If not enough info to get the stream parameters, we decode the
       first frames to get it. */
    if (av_find_stream_info(ffmovie_ctx->fmt_ctx)) {
        /* Don't fail here since this is not a problem for formats like .mov */
        /*zend_error(E_WARNING, "Can't find codec params for %s", filename); */
    }
    return 0;
}
/* }}} */


/* {{{ proto object ffmpeg_movie(string filename) 
   Constructor for ffmpeg_movie objects
 */
PHP_FUNCTION(ffmpeg_movie)
{
    int persistent = 0, hashkey_length = 0;
    char *filename = NULL, *fullpath = NULL, *hashkey = NULL;
    zval ***argv;
    ff_movie_context *ffmovie_ctx = NULL;

    /* retrieve arguments */ 
    argv = (zval ***) safe_emalloc(sizeof(zval **), ZEND_NUM_ARGS(), 0);

    if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), argv) != SUCCESS) {
        efree(argv);
        php_error_docref(NULL TSRMLS_CC, E_ERROR,
                "Error parsing arguments");
    }

    switch (ZEND_NUM_ARGS()) {
        case 2:
            convert_to_boolean_ex(argv[1]);

            if (! INI_BOOL("ffmpeg.allow_persistent") && Z_LVAL_PP(argv[1])) {
                zend_error(E_WARNING, 
                        "Persistent movies have been disabled in php.ini");
                break;
            } 

            persistent = Z_LVAL_PP(argv[1]);

            /* fallthru */
        case 1:
            convert_to_string_ex(argv[0]);
            filename = Z_STRVAL_PP(argv[0]);
            break;
        default:
            WRONG_PARAM_COUNT;
    } 

    if (persistent) {
        list_entry *le;
        /* resolve the fully-qualified path name to use as the hash key */
        fullpath = expand_filepath(filename, NULL TSRMLS_CC);

        hashkey_length = sizeof("ffmpeg-php_")-1 + 
            strlen(SAFE_STRING(filename));
        hashkey = (char *) emalloc(hashkey_length+1);
        snprintf(hashkey, hashkey_length, "ffmpeg-php_%s",
			SAFE_STRING(filename));

        
        /* do we have an existing persistent movie? */
        if (SUCCESS == zend_hash_find(&EG(persistent_list), hashkey, 
                    hashkey_length+1, (void**)&le)) {
            int type;
            
            if (Z_TYPE_P(le) != le_ffmpeg_pmovie) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR, 
                        "Failed to retrieve persistent resource");
            }
            ffmovie_ctx = (ff_movie_context*)le->ptr;
           
            /* sanity check to ensure that the resource is still a valid 
             * regular resource number */
            if (zend_list_find(ffmovie_ctx->rsrc_id, &type) == ffmovie_ctx) {
                /* add a reference to the persistent movie */
                zend_list_addref(ffmovie_ctx->rsrc_id);
            } else {
                //php_error_docref(NULL TSRMLS_CC, E_ERROR, 
                //"Not a valid persistent movie resource");
                ffmovie_ctx->rsrc_id = ZEND_REGISTER_RESOURCE(NULL, 
                        ffmovie_ctx, le_ffmpeg_pmovie);
            }
            
        } else { /* no existing persistant movie, create one */
            list_entry new_le;
            ffmovie_ctx = _php_alloc_ffmovie_ctx(1);

            if (_php_open_movie_file(ffmovie_ctx, filename)) {
                zend_error(E_WARNING, "Can't open movie file %s", filename);
                efree(argv);
                ZVAL_BOOL(getThis(), 0);
                RETURN_FALSE;
            }

            Z_TYPE(new_le) = le_ffmpeg_pmovie;
            new_le.ptr = ffmovie_ctx;

            if (FAILURE == zend_hash_update(&EG(persistent_list), hashkey, 
                        hashkey_length+1, (void *)&new_le, sizeof(list_entry),
                        NULL)) {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, 
                        "Failed to register persistent resource");
            }
            
            ffmovie_ctx->rsrc_id = ZEND_REGISTER_RESOURCE(NULL, ffmovie_ctx, 
                    le_ffmpeg_pmovie);
        }
        
    } else {
        ffmovie_ctx = _php_alloc_ffmovie_ctx(0);
        
        if (_php_open_movie_file(ffmovie_ctx, Z_STRVAL_PP(argv[0]))) {
            zend_error(E_WARNING, "Can't open movie file %s", 
                    Z_STRVAL_PP(argv[0]));
            efree(argv);
            ZVAL_BOOL(getThis(), 0);
            RETURN_FALSE;
        }
        
        /* pass NULL for resource result since we're not returning the resource
           directly, but adding it to the returned object. */
        ffmovie_ctx->rsrc_id = ZEND_REGISTER_RESOURCE(NULL, ffmovie_ctx, 
                le_ffmpeg_movie);
    }

    object_init_ex(getThis(), ffmpeg_movie_class_entry_ptr);
    add_property_resource(getThis(), "ffmpeg_movie", ffmovie_ctx->rsrc_id);

    efree(argv);
    if (fullpath) {
        efree(fullpath);
    }
    if (hashkey) {
        efree(hashkey);
    }
}
/* }}} */


/* {{{ _php_free_ffmpeg_movie
 */
static void _php_free_ffmpeg_movie(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    int i;
    ff_movie_context *ffmovie_ctx = (ff_movie_context*)rsrc->ptr;    

    if (ffmovie_ctx->codec_ctx) {
        for (i = 0; i < MAX_STREAMS; i++) {
            if(ffmovie_ctx->codec_ctx[i]) {
                avcodec_close(ffmovie_ctx->codec_ctx[i]);
            }
            ffmovie_ctx->codec_ctx[i] = NULL;
        }
    }

    av_close_input_file(ffmovie_ctx->fmt_ctx);

    efree(ffmovie_ctx);
}
/* }}} */


/* {{{ _php_free_ffmpeg_pmovie
 */
static void _php_free_ffmpeg_pmovie(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    /* TODO: Factor into a single free function for pmovie and movie */
    int i;
    ff_movie_context *ffmovie_ctx = (ff_movie_context*)rsrc->ptr;    
    
    if (ffmovie_ctx->codec_ctx) {
        for (i = 0; i < MAX_STREAMS; i++) {
            if(ffmovie_ctx->codec_ctx[i]) {
                avcodec_close(ffmovie_ctx->codec_ctx[i]);
            }
            ffmovie_ctx->codec_ctx[i] = NULL;
        }
    }

    av_close_input_file(ffmovie_ctx->fmt_ctx);

    free(ffmovie_ctx);
}
/* }}} */


/* {{{ register_ffmpeg_movie_class()
 */
void register_ffmpeg_movie_class(int module_number)
{
    TSRMLS_FETCH();
    
    le_ffmpeg_movie = zend_register_list_destructors_ex(_php_free_ffmpeg_movie,
            NULL, "ffmpeg_movie", module_number);

    le_ffmpeg_pmovie = zend_register_list_destructors_ex(NULL, 
            _php_free_ffmpeg_pmovie, "ffmpeg_pmovie", module_number);
   
    INIT_CLASS_ENTRY(ffmpeg_movie_class_entry, "ffmpeg_movie", 
            ffmpeg_movie_class_methods);
    
    /* register ffmpeg movie class */
    ffmpeg_movie_class_entry_ptr = 
        zend_register_internal_class(&ffmpeg_movie_class_entry TSRMLS_CC);
}
/* }}} */


/* {{{ __php_get_decoder_context() 
   Opens decoders and gets codec context. Always call this to get a pointer to
   the codec context. This allows to postpone codec init until a function that 
   requires it is called.
 */
static AVCodecContext* _php_get_decoder_context(ff_movie_context *ffmovie_ctx,
        int stream_type)
{
    AVCodec *decoder;
    int stream_index;

    stream_index = _php_get_stream_index(ffmovie_ctx->fmt_ctx, stream_type);
    if (stream_index < 0) {
        // FIXME: factor out the conditional.
        if (stream_type == CODEC_TYPE_VIDEO) {
            zend_error(E_WARNING, "Can't find video stream in %s", 
                    _php_get_filename(ffmovie_ctx));
            return NULL;
        } else {
            zend_error(E_WARNING, "Can't find audio stream in %s", 
                    _php_get_filename(ffmovie_ctx));
            return NULL;
        }
    }
    
    /* check if the codec for this stream is already open */
    if (!ffmovie_ctx->codec_ctx[stream_index]) {
      
        /* find the decoder */
        decoder = avcodec_find_decoder(GET_CODEC_FIELD(
                    ffmovie_ctx->fmt_ctx->streams[stream_index]->codec, 
                    codec_id));

        if (!decoder) {
            return NULL;
            /*zend_error(E_ERROR, "Could not find decoder for %s", 
                    _php_get_filename(ffmovie_ctx));*/
        }

        ffmovie_ctx->codec_ctx[stream_index] = 
            GET_CODEC_PTR(ffmovie_ctx->fmt_ctx->streams[stream_index]->codec);

       /* open the decoder */
        if (avcodec_open(ffmovie_ctx->codec_ctx[stream_index], decoder) < 0) {
            zend_error(E_ERROR, "Could not open codec for %s",
                    _php_get_filename(ffmovie_ctx));
        }
    }
    return ffmovie_ctx->codec_ctx[stream_index];
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
 * Return title field from movie or title ID3 tag from an MP3 file.
 */
PHP_FUNCTION(getTitle)
{
    ff_movie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->title,
            strlen(ffmovie_ctx->fmt_ctx->title), 1);
}
/* }}} */


/* {{{ proto string getAuthor() or getArtist()
 * Return author field from a movie or artist ID3 tag from am MP3 files.
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


/* {{{ proto string getAlbum()
 *  Return ID3 album field from an mp3 file
 */
PHP_FUNCTION(getAlbum)
{
    ff_movie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->album,
            strlen(ffmovie_ctx->fmt_ctx->album), 1);
}
/* }}} */

/* {{{ proto string getGenre()
 *  Return ID3 genre field from an mp3 file
 */
PHP_FUNCTION(getGenre)
{
    ff_movie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->genre,
            strlen(ffmovie_ctx->fmt_ctx->genre), 1);
}
/* }}} */


/* {{{ proto int getTrackNumber()
 *  Return ID3 track field from an mp3 file
 */
PHP_FUNCTION(getTrackNumber)
{
    ff_movie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);
    
    RETURN_LONG(ffmovie_ctx->fmt_ctx->track);
}
/* }}} */

/* {{{ proto int getYear()
 *  Return ID3 year field from an mp3 file
 */
PHP_FUNCTION(getYear)
{
    ff_movie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);
    
    RETURN_LONG(ffmovie_ctx->fmt_ctx->year);
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


/* {{{ _php_get_framerate()
 */
static float _php_get_framerate(ff_movie_context *ffmovie_ctx)
{
    AVStream *st = _php_get_video_stream(ffmovie_ctx->fmt_ctx);

    if (!st) {
      return 0.0f;
    }

#if LIBAVCODEC_BUILD > 4753 
    return av_q2d(GET_CODEC_FIELD(st->codec, time_base));
#else
    return (float)GET_CODEC_FIELD(st->codec, frame_rate) / 
                        GET_CODEC_FIELD(st->codec, frame_rate_base);
#endif
}
/* }}} */


/* {{{ _php_get_framecount()
 */
static long _php_get_framecount(ff_movie_context *ffmovie_ctx)
{
    /* does this movie have a video stream?  */
    if (!_php_get_video_stream(ffmovie_ctx->fmt_ctx)) {
      return 0;
    }
    
    /* TODO: The test_framecounter.avi movie reports one more frame than it
     *       contains. Make sure this is rounding correctly or maybe use floor
     *       to be safe.
     */
    return LRINT(_php_get_framerate(ffmovie_ctx) * 
            _php_get_duration(ffmovie_ctx));
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
 
    return GET_CODEC_FIELD(st->codec, width);
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
 
    return GET_CODEC_FIELD(st->codec, height);
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
    AVCodecContext *decoder_ctx = NULL;

    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, CODEC_TYPE_VIDEO);
    if (!decoder_ctx) {
        return 0;
    }

    if (ffmovie_ctx->frame_number <= 0) {
        return 1; /* no frames read yet so return the first */
    } else {
        return ffmovie_ctx->frame_number;
    }
}
/* }}} */


/* {{{ proto resource getFrameNumber()
 */
PHP_FUNCTION(getFrameNumber)
{
    ff_movie_context *ffmovie_ctx;
    int frame_number = 0;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    frame_number =_php_get_framenumber(ffmovie_ctx);
   
    if (frame_number) {
        RETURN_LONG(frame_number);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ _php_get_pixelformat()
 */
static int _php_get_pixelformat(ff_movie_context *ffmovie_ctx)
{
    AVCodecContext *decoder_ctx;
    
    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, CODEC_TYPE_VIDEO);

    return decoder_ctx ? decoder_ctx->pix_fmt : 0;
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

    RETURN_BOOL( _php_get_audio_stream(ffmovie_ctx->fmt_ctx) > 0);
}
/* }}} */


/* {{{ _php_get_codec_name()
   Returns a frame from the movie.
 */
static const char* _php_get_codec_name(ff_movie_context *ffmovie_ctx, int type)
{
    AVCodecContext *decoder_ctx = NULL;
    AVCodec *p = NULL;
    const char *codec_name;
    char buf1[32];
    int stream;

    stream = _php_get_stream_index(ffmovie_ctx->fmt_ctx, type);
    if (stream < 0) {
        return NULL;
    }

    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, type);
    if (!decoder_ctx) {
        return NULL;
    }

    p = avcodec_find_decoder(decoder_ctx->codec_id);

    /* Copied from libavcodec/utils.c::avcodec_string */
    if (p) {
        codec_name = p->name;
        if (decoder_ctx->codec_id == CODEC_ID_MP3) {
            if (decoder_ctx->sub_id == 2)
                codec_name = "mp2";
            else if (decoder_ctx->sub_id == 1)
                codec_name = "mp1";
        }
    } else if (decoder_ctx->codec_id == CODEC_ID_MPEG2TS) {
        /* fake mpeg2 transport stream codec (currently not registered) */
        codec_name = "mpeg2ts";
    } else if (decoder_ctx->codec_name[0] != '\0') {
        codec_name = decoder_ctx->codec_name;
    } else {
        /* output avi tags */
        if (decoder_ctx->codec_type == CODEC_TYPE_VIDEO) {
            snprintf(buf1, sizeof(buf1), "%c%c%c%c",
                    decoder_ctx->codec_tag & 0xff,
                    (decoder_ctx->codec_tag >> 8) & 0xff,
                    (decoder_ctx->codec_tag >> 16) & 0xff,
                    (decoder_ctx->codec_tag >> 24) & 0xff);
        } else {
            snprintf(buf1, sizeof(buf1), "0x%04x", decoder_ctx->codec_tag);
        }
        codec_name = buf1;
    }
    
    return codec_name;
} 


/* {{{ proto int getVideoCodec()
 */
PHP_FUNCTION(getVideoCodec)
{
    ff_movie_context *ffmovie_ctx;
    char *codec_name;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    codec_name = (char*)_php_get_codec_name(ffmovie_ctx, CODEC_TYPE_VIDEO);
 
    if (codec_name) {
        RETURN_STRINGL(codec_name, strlen(codec_name), 1);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto int getAudioCodec()
 */
PHP_FUNCTION(getAudioCodec)
{
    ff_movie_context *ffmovie_ctx;
    char *codec_name;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    codec_name = (char*)_php_get_codec_name(ffmovie_ctx, CODEC_TYPE_AUDIO);
 
    if (codec_name) {
        RETURN_STRINGL(codec_name, strlen(codec_name), 1);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ _php_get_audio_channels()
   Returns a frame from the movie.
 */
static int _php_get_audio_channels(ff_movie_context *ffmovie_ctx)
{
    AVCodecContext *decoder_ctx = NULL;
    int stream;

    stream = _php_get_stream_index(ffmovie_ctx->fmt_ctx, CODEC_TYPE_AUDIO);
    if (stream < 0) {
        return 0;
    }

    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, CODEC_TYPE_AUDIO);
    if (!decoder_ctx) {
        return 0;
    }

    return decoder_ctx->channels;
} 


/* {{{ proto int getAudioChannels()
 */
PHP_FUNCTION(getAudioChannels)
{
    ff_movie_context *ffmovie_ctx;
    int channels;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    channels = _php_get_audio_channels(ffmovie_ctx);
 
    if (channels) {
        RETURN_LONG(channels);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ _php_get_av_frame()
   Returns a frame from the movie.
 */
#define GETFRAME_KEYFRAME -1
#define GETFRAME_NEXTFRAME 0
static AVFrame* _php_get_av_frame(ff_movie_context *ffmovie_ctx, 
        int wanted_frame, int *is_keyframe, int64_t *pts)
{
    AVCodecContext *decoder_ctx = NULL;
    AVPacket packet;
    AVFrame *frame = NULL;
    int got_frame; 
    int video_stream;

    video_stream = _php_get_stream_index(ffmovie_ctx->fmt_ctx, 
            CODEC_TYPE_VIDEO);
    if (video_stream < 0) {
        return NULL;
    }
 
    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, CODEC_TYPE_VIDEO);
    if (decoder_ctx == NULL) {
        return NULL;
    }

    /* Rewind to the beginning of the stream if wanted frame already passed */
    if (wanted_frame > 0 && wanted_frame <= ffmovie_ctx->frame_number) {
        if (
                
#if LIBAVFORMAT_BUILD >=  4619
                av_seek_frame(ffmovie_ctx->fmt_ctx, -1, 0, 0)
#else 
                av_seek_frame(ffmovie_ctx->fmt_ctx, -1, 0)
#endif
                < 0) {
            //zend_error(E_ERROR,"Error seeking to beginning of video stream");
            // If we can't seek, fall back to reopening the file. 
            // NOTE: This may mask locking problems in persistent movies.
            _php_open_movie_file(ffmovie_ctx, _php_get_filename(ffmovie_ctx));
        }
 
        /* flush decoder buffers here */
        avcodec_flush_buffers(decoder_ctx);
        
        ffmovie_ctx->frame_number = 0; 
    }

    frame = avcodec_alloc_frame();
    
    /* read frames looking for wanted_frame */ 
    while (av_read_frame(ffmovie_ctx->fmt_ctx, &packet) >= 0) {
       
		/* hurry up if we're still a ways from the target frame */
        if (wanted_frame != GETFRAME_KEYFRAME &&
                wanted_frame != GETFRAME_NEXTFRAME &&
                wanted_frame - ffmovie_ctx->frame_number > 
				decoder_ctx->gop_size + 1) {
           decoder_ctx->hurry_up = 1;
        } else {
           decoder_ctx->hurry_up = 0;
        }

        if (packet.stream_index == video_stream) {
        
            avcodec_decode_video(decoder_ctx, frame, &got_frame,
                    packet.data, packet.size);
        
            if (got_frame) {
                ffmovie_ctx->frame_number++; 
                /* FIXME: 
                 *        With the addition of the keyframe logic, this loop is 
                 *        getting a little too tricky. wanted_frame is way 
                 *        overloaded. Refactor to make clearer what is going on.
                 */

                /* 
                 * if caller wants next keyframe then get it and break out of 
                 * loop.
                 */
                if (wanted_frame == GETFRAME_KEYFRAME && 
                        (packet.flags & PKT_FLAG_KEY)) {
                    /* free wanted frame packet */
                    *is_keyframe = 1;
                    *pts = packet.pts;
                    av_free_packet(&packet);
                    goto found_frame; 
                }
                
                if (wanted_frame == GETFRAME_NEXTFRAME || 
                        ffmovie_ctx->frame_number == wanted_frame) {
                    /* free wanted frame packet */
                    *is_keyframe = (packet.flags & PKT_FLAG_KEY);
                    *pts = packet.pts;
                    av_free_packet(&packet);
                    goto found_frame; 
                }
            }
        }

        /* free the packet allocated by av_read_frame */
        av_free_packet(&packet);
    }

    av_free(frame);
    return NULL;

found_frame:
    return frame;
}
/* }}} */


/* {{{ _php_get_ff_frame()
   puts a ff_frame object into the php return_value variable 
   returns 1 on sucess, 0 on failure.
 */
static int _php_get_ff_frame(ff_movie_context *ffmovie_ctx, 
        int wanted_frame, INTERNAL_FUNCTION_PARAMETERS) {
    int is_keyframe = 0;
    int64_t pts;
    AVFrame *frame = NULL;
    ff_frame_context *ff_frame;
 
    frame = _php_get_av_frame(ffmovie_ctx, wanted_frame, &is_keyframe, &pts);
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

        /* TODO: Provide function(s) for setting these in ffmpeg_frame */
        ff_frame->width = _php_get_framewidth(ffmovie_ctx);
        ff_frame->height = _php_get_frameheight(ffmovie_ctx);
        ff_frame->pixel_format = _php_get_pixelformat(ffmovie_ctx);
        ff_frame->keyframe = is_keyframe;
        ff_frame->pts = pts;
        
        ff_frame->av_frame = avcodec_alloc_frame();
        avpicture_alloc((AVPicture*)ff_frame->av_frame, ff_frame->pixel_format,
            ff_frame->width, ff_frame->height);
 
        /* FIXME: temporary hack until I figure out how to pass new buffers 
         *        to the decoder 
         */
        img_copy((AVPicture*)ff_frame->av_frame, 
                (AVPicture *)frame, ff_frame->pixel_format, 
                ff_frame->width, ff_frame->height);

        return 1;
    } else {
        return 0;
    }

}
/* }}} */


/* {{{ proto resource getNextKeyFrame()
 */
PHP_FUNCTION(getNextKeyFrame)
{
    ff_movie_context *ffmovie_ctx;

    if (ZEND_NUM_ARGS()) {
        WRONG_PARAM_COUNT;
    }
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);

    if (!_php_get_ff_frame(ffmovie_ctx, GETFRAME_KEYFRAME, 
                INTERNAL_FUNCTION_PARAM_PASSTHRU)) {
        RETURN_FALSE;   
    }
}
/* }}} */


/* {{{ proto resource getFrame([int frame])
 */
PHP_FUNCTION(getFrame)
{
    zval **argv[1];
    int wanted_frame = 0; 
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

    if (! _php_get_ff_frame(ffmovie_ctx, wanted_frame,
                INTERNAL_FUNCTION_PARAM_PASSTHRU)) {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ _php_pre_read_frame()
 * ffmpeg seems not to fill in some AVCodecContext fields until at least
 * one frame is read. This function will read a frame without moving the
 * frame counter.
 */
static void _php_pre_read_frame(ff_movie_context *ffmovie_ctx) {
    AVFrame *frame = NULL;
    int is_keyframe;
    int64_t pts;

    frame = _php_get_av_frame(ffmovie_ctx,
            _php_get_framenumber(ffmovie_ctx) - 1, &is_keyframe, &pts);

    av_free(frame);
}


/* {{{ _php_get_pixelaspect()
 */
static double _php_get_sample_aspect_ratio(ff_movie_context *ffmovie_ctx)
{
    AVCodecContext *decoder_ctx;
	

    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, CODEC_TYPE_VIDEO);
    if (!decoder_ctx) {
        return 0;
    }


	if (decoder_ctx->sample_aspect_ratio.num == 0) {
		// pre read a frame so ffmpeg will fill in sample aspect ratio field.
        _php_pre_read_frame(ffmovie_ctx);
        
		if (decoder_ctx->sample_aspect_ratio.num == 0) {
			return 0;
		}
	}

    return av_q2d(decoder_ctx->sample_aspect_ratio);
}
/* }}} */


/* {{{ proto double getPixelAspectRatio()
 */
PHP_FUNCTION(getPixelAspectRatio)
{
    double aspect;
    ff_movie_context *ffmovie_ctx;
    
    GET_MOVIE_RESOURCE(ffmovie_ctx);
   
    aspect = _php_get_sample_aspect_ratio(ffmovie_ctx); 

    RETURN_DOUBLE(aspect);
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
