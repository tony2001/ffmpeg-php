#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"

#include "php_ffmpeg.h"

#include "qp_frame.h"
#include "qp_movie.h"
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
    ZEND_FETCH_RESOURCE2(ff_movie_ctx, qp_movie_context*, _tmp_zval, -1,\
            "ffmpeg_movie", le_ffmpeg_movie, le_ffmpeg_pmovie);\
}\

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
    PHP_FALIAS(getaudiosamplerate,  getAudioSampleRate,  NULL)
    PHP_FALIAS(getaudiobitrate,     getAudioBitRate,     NULL)
    PHP_FALIAS(getvideobitrate,     getVideoBitRate,     NULL)
    PHP_FALIAS(getpixelaspectratio, getPixelAspectRatio, NULL)
    {NULL, NULL, NULL}
};
/* }}} */


/* {{{ _php_alloc_movie_ctx()
 */
static qp_movie_context* _php_alloc_movie_ctx(int persistent)
{
    return qp_alloc_movie_ctx(persistent ? NULL : _emalloc);
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
    qp_movie_context *movie_ctx = NULL;

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
            movie_ctx = (qp_movie_context*)le->ptr;
           
            /* sanity check to ensure that the resource is still a valid 
             * regular resource number */
            if (zend_list_find(movie_ctx->udef_long, &type) == movie_ctx) {
                /* add a reference to the persistent movie */
                zend_list_addref(movie_ctx->udef_long);
            } else {
                //php_error_docref(NULL TSRMLS_CC, E_ERROR, 
                //"Not a valid persistent movie resource");
                movie_ctx->udef_long = ZEND_REGISTER_RESOURCE(NULL, 
                        movie_ctx, le_ffmpeg_pmovie);
            }
            
        } else { /* no existing persistant movie, create one */
            list_entry new_le;
            movie_ctx = _php_alloc_movie_ctx(1);

            if (qp_open_movie_file(movie_ctx, filename)) {
                zend_error(E_WARNING, "Can't open movie file %s", filename);
                efree(argv);
                ZVAL_BOOL(getThis(), 0);
                RETURN_FALSE;
            }

            Z_TYPE(new_le) = le_ffmpeg_pmovie;
            new_le.ptr = movie_ctx;

            if (FAILURE == zend_hash_update(&EG(persistent_list), hashkey, 
                        hashkey_length+1, (void *)&new_le, sizeof(list_entry),
                        NULL)) {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, 
                        "Failed to register persistent resource");
            }
            
            movie_ctx->udef_long = ZEND_REGISTER_RESOURCE(NULL, movie_ctx, 
                    le_ffmpeg_pmovie);
        }
        
    } else {
        movie_ctx = _php_alloc_movie_ctx(0);
        
        if (qp_open_movie_file(movie_ctx, Z_STRVAL_PP(argv[0]))) {
            zend_error(E_WARNING, "Can't open movie file %s", 
                    Z_STRVAL_PP(argv[0]));
            efree(argv);
            ZVAL_BOOL(getThis(), 0);
            RETURN_FALSE;
        }
        
        /* pass NULL for resource result since we're not returning the resource
           directly, but adding it to the returned object. */
        movie_ctx->udef_long = ZEND_REGISTER_RESOURCE(NULL, movie_ctx, 
                le_ffmpeg_movie);
    }

    object_init_ex(getThis(), ffmpeg_movie_class_entry_ptr);
    add_property_resource(getThis(), "ffmpeg_movie", movie_ctx->udef_long);

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
    qp_free_movie_ctx((qp_movie_context*)rsrc->ptr, _efree);
}
/* }}} */


/* {{{ _php_free_ffmpeg_movie
 */
static void _php_free_ffmpeg_pmovie(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    qp_free_movie_ctx((qp_movie_context*)rsrc->ptr, NULL);
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



/* {{{ proto string getComment()
 */
PHP_FUNCTION(getComment)
{
    qp_movie_context *movie_ctx;

    GET_MOVIE_RESOURCE(movie_ctx);
    
    RETURN_STRING(qp_get_comment(movie_ctx), 1);
}
/* }}} */


/* {{{ proto string getTitle()
 * Return title field from movie or title ID3 tag from an MP3 file.
 */
PHP_FUNCTION(getTitle)
{
    qp_movie_context *movie_ctx;
    
    GET_MOVIE_RESOURCE(movie_ctx);

    RETURN_STRING(qp_get_title(movie_ctx), 1);
}
/* }}} */


/* {{{ proto string getAuthor() or getArtist()
 * Return author field from a movie or artist ID3 tag from am MP3 files.
 */
PHP_FUNCTION(getAuthor)
{
    qp_movie_context *movie_ctx;
    
    GET_MOVIE_RESOURCE(movie_ctx);

    RETURN_STRING(qp_get_author(movie_ctx), 1);
}
/* }}} */


/* {{{ proto string getCopyright()
 */
PHP_FUNCTION(getCopyright)
{
    qp_movie_context *movie_ctx;
    
    GET_MOVIE_RESOURCE(movie_ctx);

    RETURN_STRING(qp_get_copyright(movie_ctx), 1);
}
/* }}} */


/* {{{ proto string getAlbum()
 *  Return ID3 album field from an mp3 file
 */
PHP_FUNCTION(getAlbum)
{
    qp_movie_context *movie_ctx;
    
    GET_MOVIE_RESOURCE(movie_ctx);

    RETURN_STRING(qp_get_album(movie_ctx), 1);
}
/* }}} */

/* {{{ proto string getGenre()
 *  Return ID3 genre field from an mp3 file
 */
PHP_FUNCTION(getGenre)
{
    qp_movie_context *movie_ctx;
    
    GET_MOVIE_RESOURCE(movie_ctx);

    RETURN_STRING(qp_get_genre(movie_ctx), 1);
}
/* }}} */


/* {{{ proto int getTrackNumber()
 *  Return ID3 track field from an mp3 file
 */
PHP_FUNCTION(getTrackNumber)
{
    qp_movie_context *movie_ctx;
    
    GET_MOVIE_RESOURCE(movie_ctx);
    
    RETURN_LONG(qp_get_track(movie_ctx));
}
/* }}} */

/* {{{ proto int getYear()
 *  Return ID3 year field from an mp3 file
 */
PHP_FUNCTION(getYear)
{
    qp_movie_context *movie_ctx;
    
    GET_MOVIE_RESOURCE(movie_ctx);
    
    RETURN_LONG(qp_get_year(movie_ctx));
}
/* }}} */


/* {{{ proto int getDuration()
 */
PHP_FUNCTION(getDuration)
{
    qp_movie_context *movie_ctx;
       
    GET_MOVIE_RESOURCE(movie_ctx);
    
    RETURN_DOUBLE(qp_get_duration(movie_ctx));
}
/* }}} */


/* {{{ proto int getFrameCount()
 */
PHP_FUNCTION(getFrameCount)
{
    qp_movie_context *movie_ctx;
    GET_MOVIE_RESOURCE(movie_ctx);
    RETURN_LONG(qp_get_frame_count(movie_ctx));
}
/* }}} */


/* {{{ proto int getFrameRate()
 */
PHP_FUNCTION(getFrameRate)
{
    qp_movie_context *movie_ctx;
    
    GET_MOVIE_RESOURCE(movie_ctx);

    RETURN_DOUBLE(qp_get_frame_rate(movie_ctx));
}
/* }}} */


/* {{{ proto string getFileName()
 */
PHP_FUNCTION(getFileName)
{
    qp_movie_context *movie_ctx;
    char* filename;
    
    GET_MOVIE_RESOURCE(movie_ctx);

    filename = qp_get_file_name(movie_ctx);

    RETURN_STRINGL(filename, strlen(filename), 1);
}
/* }}} */


/* {{{ proto int getFrameWidth()
 */
PHP_FUNCTION(getFrameWidth)
{
    qp_movie_context *movie_ctx;
    
    GET_MOVIE_RESOURCE(movie_ctx);
    
    RETURN_LONG(qp_get_movie_width(movie_ctx));
}
/* }}} */


/* {{{ proto int getFrameHeight()
 */
PHP_FUNCTION(getFrameHeight)
{
    qp_movie_context *movie_ctx;

    GET_MOVIE_RESOURCE(movie_ctx);

    RETURN_LONG(qp_get_movie_height(movie_ctx));
}
/* }}} */


/* {{{ proto resource getFrameNumber()
 */
PHP_FUNCTION(getFrameNumber)
{
    qp_movie_context *movie_ctx;
    int frame_number = 0;
    
    GET_MOVIE_RESOURCE(movie_ctx);

    frame_number = qp_get_current_frame_number(movie_ctx);
   
    if (frame_number) {
        RETURN_LONG(frame_number);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto int getPixelFormat()
 */
PHP_FUNCTION(getPixelFormat)
{
    const char *fmt;
    qp_movie_context *movie_ctx;

    GET_MOVIE_RESOURCE(movie_ctx);

    fmt = qp_get_movie_pixel_format_name(movie_ctx);
    
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


/* {{{ proto int getBitrate()
 */
PHP_FUNCTION(getBitRate)
{
    qp_movie_context *movie_ctx;
    
    GET_MOVIE_RESOURCE(movie_ctx);
   
    RETURN_LONG(qp_get_bit_rate(movie_ctx));
}
/* }}} */


/* {{{ proto int hasAudio()
 */
PHP_FUNCTION(hasAudio)
{
    qp_movie_context *movie_ctx;

    GET_MOVIE_RESOURCE(movie_ctx);

    RETURN_BOOL(qp_has_audio(movie_ctx));
}
/* }}} */


/* {{{ proto int getVideoCodec()
 */
PHP_FUNCTION(getVideoCodec)
{
    qp_movie_context *movie_ctx;
    char *codec_name;

    GET_MOVIE_RESOURCE(movie_ctx);

    codec_name = (char*)qp_get_codec_name(movie_ctx, CODEC_TYPE_VIDEO);
 
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
    qp_movie_context *movie_ctx;
    char *codec_name;

    GET_MOVIE_RESOURCE(movie_ctx);

    codec_name = (char*)qp_get_codec_name(movie_ctx, CODEC_TYPE_AUDIO);
 
    if (codec_name) {
        RETURN_STRINGL(codec_name, strlen(codec_name), 1);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto int getAudioChannels()
 */
PHP_FUNCTION(getAudioChannels)
{
    qp_movie_context *movie_ctx;
    int channels;

    GET_MOVIE_RESOURCE(movie_ctx);

    channels = qp_get_num_audio_channels(movie_ctx);
 
    if (channels) {
        RETURN_LONG(channels);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto int getAudioSampleRate()
 */
PHP_FUNCTION(getAudioSampleRate)
{
    qp_movie_context *movie_ctx = NULL;
    int sample_rate = 0;

    GET_MOVIE_RESOURCE(movie_ctx);

    sample_rate = qp_get_codec_sample_rate(movie_ctx, CODEC_TYPE_AUDIO);
 
    if (sample_rate) {
        RETURN_LONG(sample_rate);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto int getAudioBitRate()
 */
PHP_FUNCTION(getAudioBitRate)
{
    qp_movie_context *movie_ctx = NULL;
    int bit_rate = 0;

    GET_MOVIE_RESOURCE(movie_ctx);

    bit_rate = qp_get_codec_bit_rate(movie_ctx, CODEC_TYPE_AUDIO);
 
    if (bit_rate) {
        RETURN_LONG(bit_rate);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto int getVideoBitRate()
 */
PHP_FUNCTION(getVideoBitRate)
{
    qp_movie_context *movie_ctx = NULL;
    int bit_rate = 0;

    GET_MOVIE_RESOURCE(movie_ctx);

    bit_rate = qp_get_codec_bit_rate(movie_ctx, CODEC_TYPE_VIDEO);
 
    if (bit_rate) {
        RETURN_LONG(bit_rate);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto resource getNextKeyFrame()
 */
PHP_FUNCTION(getNextKeyFrame)
{
    qp_movie_context *movie_ctx;
    qp_frame_context *frame_ctx;

    if (ZEND_NUM_ARGS()) {
        WRONG_PARAM_COUNT;
    }
    
    GET_MOVIE_RESOURCE(movie_ctx);
    
    // TODO: split _php_create_ffmpeg_frame so we can wait to alloc
    // the frame until we're sure we get a frame back from qp_get_frame
    frame_ctx =  _php_create_ffmpeg_frame(INTERNAL_FUNCTION_PARAM_PASSTHRU);

    if (qp_get_frame(movie_ctx, QP_GETFRAME_KEYFRAME, &frame_ctx) != 0) {
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
    qp_movie_context *movie_ctx;
    qp_frame_context *frame_ctx;

    if (ZEND_NUM_ARGS() > 1) {
        WRONG_PARAM_COUNT;
    }
    
    GET_MOVIE_RESOURCE(movie_ctx);

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

    frame_ctx =  _php_create_ffmpeg_frame(INTERNAL_FUNCTION_PARAM_PASSTHRU);

    if (qp_get_frame(movie_ctx, wanted_frame, &frame_ctx) != 0) {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto double getPixelAspectRatio()
 */
PHP_FUNCTION(getPixelAspectRatio)
{
    double aspect;
    qp_movie_context *movie_ctx;
    
    GET_MOVIE_RESOURCE(movie_ctx);
   
    aspect = qp_get_pixel_aspect_ratio(movie_ctx); 

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
