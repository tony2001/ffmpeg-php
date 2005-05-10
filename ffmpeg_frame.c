#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"

#include "php_ffmpeg.h"

#include "ffmpeg_frame.h"

/* 
   include gd header from local include dir. This is a copy of gd.h that is 
   distributed with php-4.3.9. It is distributed along with ffmpeg-php to
   allow ffmpeg-php to be built without access to the php sources
 */
#if HAVE_LIBGD20
#include "include/gd.h" 

#define FFMPEG_PHP_FETCH_IMAGE_RESOURCE(gd_img, ret) { \
    ZEND_GET_RESOURCE_TYPE_ID(le_gd, "gd"); \
    ZEND_FETCH_RESOURCE(gd_img, gdImagePtr, ret, -1, "Image", le_gd); \
}

static int le_gd; // this is only valid after calling FFMPEG_PHP_FETCH_IMAGE_RESOURCE() 

#endif // HAVE_LIBGD20

int le_ffmpeg_frame; // not static since it is used in ffmpeg_output_movie

static zend_class_entry *ffmpeg_frame_class_entry_ptr;
zend_class_entry ffmpeg_frame_class_entry;
 
/* {{{ ffmpeg_frame methods[]
    Methods of the ffmpeg_frame class 
*/
zend_function_entry ffmpeg_frame_class_methods[] = {
   
#if HAVE_LIBGD20
    /* contructor */
    PHP_FE(ffmpeg_frame, NULL)

    /* gd methods */
    PHP_FALIAS(togdimage,      toGDImage,     NULL)
    
#endif // HAVE_LIBGD20

    /* methods */
    PHP_FALIAS(getwidth,                    getWidth,                   NULL)
    PHP_FALIAS(getheight,                   getHeight,                  NULL)
    PHP_FALIAS(resize,                      resize,                     NULL)
    PHP_FALIAS(crop,                        crop,                       NULL)
    PHP_FALIAS(iskeyframe,                  isKeyFrame,                 NULL)
    PHP_FALIAS(getpresentationtimestamp,    getPresentationTimestamp,   NULL)
    PHP_FALIAS(getpts,                      getPresentationTimestamp,   NULL)
	{NULL, NULL, NULL}
};
/* }}} */


/* {{{ _php_alloc_ff_frame()
 */
static ff_frame_context* _php_alloc_ff_frame()
{
    ff_frame_context *ff_frame = NULL;

    ff_frame = emalloc(sizeof(ff_frame_context));
    
    if (!ff_frame) {
        zend_error(E_ERROR, "Error allocating ffmpeg_frame");
    }

    ff_frame->av_frame = NULL;
    ff_frame->width = 0;
    ff_frame->height = 0;
    ff_frame->pixel_format = 0;

    return ff_frame;
}
/* }}} */


/* {{{ proto object ffmpeg_frame() 
   creates an ffmpeg_frame object, adds a ffmpeg_frame resource to the
   object, registers the resource and returns a direct pointer to the 
   resource.
 */
ff_frame_context* _php_create_ffmpeg_frame(INTERNAL_FUNCTION_PARAMETERS)
{
    int ret;
	ff_frame_context *ff_frame;
    
    ff_frame = _php_alloc_ff_frame();
    
	ret = ZEND_REGISTER_RESOURCE(NULL, ff_frame, le_ffmpeg_frame);
    
    object_init_ex(return_value, ffmpeg_frame_class_entry_ptr);
    add_property_resource(return_value, "ffmpeg_frame", ret);
    return ff_frame;
}
/* }}} */

/* {{{ _php_free_av_frame
 */
static void _php_free_av_frame(AVFrame *av_frame)
{
    if (av_frame) {
        if (av_frame->data[0]) {
            av_free(av_frame->data[0]);
            av_frame->data[0] = NULL;
        }
        av_free(av_frame);
    }
}

/* {{{ _php_free_ffmpeg_frame
 */
static void _php_free_ffmpeg_frame(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    ff_frame_context *ff_frame = (ff_frame_context*)rsrc->ptr;    
    _php_free_av_frame(ff_frame->av_frame);
    efree(ff_frame);
}
/* }}} */


void register_ffmpeg_frame_class(int module_number)
{
    TSRMLS_FETCH();

    le_ffmpeg_frame = zend_register_list_destructors_ex(_php_free_ffmpeg_frame,
            NULL, "ffmpeg_frame", module_number);

    INIT_CLASS_ENTRY(ffmpeg_frame_class_entry, "ffmpeg_frame", 
            ffmpeg_frame_class_methods);

    /* register ffmpeg frame class */
    ffmpeg_frame_class_entry_ptr = 
        zend_register_internal_class(&ffmpeg_frame_class_entry TSRMLS_CC);
}


/* {{{ _php_convert_frame()
 */
static int _php_convert_frame(ff_frame_context *ff_frame, int new_fmt) {
    AVFrame *new_fmt_frame;

    if (!ff_frame->av_frame) {
        return -1;
    }

    if (ff_frame->pixel_format == new_fmt) {
        return 0;
    }

    new_fmt_frame = avcodec_alloc_frame();
    avpicture_alloc((AVPicture*)new_fmt_frame, new_fmt, ff_frame->width,
                            ff_frame->height);
    if (img_convert((AVPicture*)new_fmt_frame, new_fmt, 
                (AVPicture *)ff_frame->av_frame, 
                ff_frame->pixel_format, ff_frame->width, 
                ff_frame->height) < 0) {
        zend_error(E_ERROR, "Error converting frame");
    }

    _php_free_av_frame(ff_frame->av_frame);

    ff_frame->av_frame = new_fmt_frame;
    ff_frame->pixel_format = new_fmt;
    return 0;
}
/* }}} */


/* {{{ _php_crop_frame()
 */
static int _php_crop_frame(ff_frame_context *ff_frame, 
        int crop_top, int crop_bottom, int crop_left, int crop_right) {
    AVFrame *cropped_frame, *tmp_src;
    AVFrame crop_temp;
    int cropped_width, cropped_height;

    if (!ff_frame->av_frame) {
        return -1;
    }

    tmp_src =ff_frame->av_frame;
    
    crop_temp.data[0] = tmp_src->data[0] +
        (crop_top * tmp_src->linesize[0]) + crop_left;

    crop_temp.data[1] = tmp_src->data[1] +
        ((crop_top >> 1) * tmp_src->linesize[1]) +
        (crop_left >> 1);

    crop_temp.data[2] = tmp_src->data[2] +
        ((crop_top >> 1) * tmp_src->linesize[2]) +
        (crop_left >> 1);

    crop_temp.linesize[0] = tmp_src->linesize[0];
    crop_temp.linesize[1] = tmp_src->linesize[1];
    crop_temp.linesize[2] = tmp_src->linesize[2];

    cropped_frame = avcodec_alloc_frame();

    cropped_width = ff_frame->width - (crop_left + crop_right);
    cropped_height = ff_frame->height - (crop_top + crop_bottom);
    
    avpicture_alloc((AVPicture*)cropped_frame, ff_frame->pixel_format,
            cropped_width, cropped_height);
    
    img_copy((AVPicture*)cropped_frame, 
                (AVPicture *)&crop_temp, ff_frame->pixel_format, 
                cropped_width, cropped_height);

    /* free non cropped frame */
    _php_free_av_frame(ff_frame->av_frame);

    ff_frame->av_frame = cropped_frame;
    ff_frame->width = cropped_width;
    ff_frame->height = cropped_height;
    return 0;
}
/* }}} */


/* {{{ _php_resample_frame()
 */
static int _php_resample_frame(ff_frame_context *ff_frame,
        int wanted_width, int wanted_height, int crop_top, int crop_bottom,
        int crop_left, int crop_right)
{
    AVFrame *resampled_frame;
    ImgReSampleContext *img_resample_ctx = NULL;
 
    if (!ff_frame->av_frame) {
        return -1;
    }

    /* just crop if wanted dimensions - crop bands = zero resampling */
    if (wanted_width == ff_frame->width - (crop_left + crop_right) && 
            wanted_height == ff_frame->height - (crop_left + crop_right)) {
        _php_crop_frame(ff_frame, crop_top, crop_bottom, crop_left, crop_right);
        return 0;
    } 
    
    /* convert to PIX_FMT_YUV420P required for resampling */
    _php_convert_frame(ff_frame, PIX_FMT_YUV420P);

    img_resample_ctx = img_resample_full_init(
            wanted_width, wanted_height,
            ff_frame->width, ff_frame->height,
            crop_top, crop_bottom, crop_left, crop_right,
            0, 0, 0, 0);
    if (!img_resample_ctx) {
        return -1;
    }

    resampled_frame = avcodec_alloc_frame();
    avpicture_alloc((AVPicture*)resampled_frame, PIX_FMT_YUV420P, 
            wanted_width, wanted_height);

    img_resample(img_resample_ctx, (AVPicture*)resampled_frame, 
            (AVPicture*)ff_frame->av_frame);

    _php_free_av_frame(ff_frame->av_frame);

    img_resample_close(img_resample_ctx);

    ff_frame->av_frame = resampled_frame;
    ff_frame->width = wanted_width;
    ff_frame->height = wanted_height;

    return 0;
}
/* }}} */

#if HAVE_LIBGD20

/* {{{ _php_get_gd_image()
 */
static int _php_get_gd_image(int w, int h)
{
    zval *function_name, *width, *height;
    zval **argv[2];
    zend_function *func;
    zval *retval;
    char *function_cname = "imagecreatetruecolor";
    int ret;
    TSRMLS_FETCH();
   
    if (zend_hash_find(EG(function_table), function_cname, 
                strlen(function_cname) + 1, (void **)&func) == FAILURE) {
        zend_error(E_ERROR, "Error can't find %s function", function_cname);
    }

    MAKE_STD_ZVAL(function_name);
    MAKE_STD_ZVAL(width);
    MAKE_STD_ZVAL(height);

    ZVAL_STRING(function_name, function_cname, 0);
    ZVAL_LONG(width, w);
    ZVAL_LONG(height, h);

    argv[0] = &width;
    argv[1] = &height;

    if (call_user_function_ex(EG(function_table), NULL, function_name, 
                &retval, 2, argv, 0, NULL TSRMLS_CC) == FAILURE) {
        zend_error(E_ERROR, "Error calling %s function", function_cname);
    }
    
    FREE_ZVAL(function_name); 
    FREE_ZVAL(width); 
    FREE_ZVAL(height); 
    
    if (!retval || retval->type != IS_RESOURCE) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR,
                "Error creating GD Image");
    }

    ret = retval->value.lval;
    zend_list_addref(ret); 
    if (retval) {
        zval_ptr_dtor(&retval);
    }

    return ret;
}
/* }}} */


/* {{{ _php_avframe_to_gd_image()
 */
static int _php_avframe_to_gd_image(AVFrame *frame, gdImage *dest, int width, int height)
{
    int x, y;
    int *src = (int*)frame->data[0];

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
		
			// FIXME: gdImageBoundsSafe is implemented as a macro in the gd library
            //        bundled with php and as a function in other gd libraries.
            //        The macro and the function are incompatible so diable this
            //        until I can come up with a way to work around the problem.
			//if (gdImageBoundsSafe(dest, x, y)) {
                /* copy pixel to gdimage buffer zeroing the alpha channel */
                dest->tpixels[y][x] = src[x] & 0x00ffffff;
            //} else {
            //    return -1;
            //}
        }
        src += width;
    }
    return 0;
}
/* }}} */


/* {{{ _php_gd_image_to_avframe()
 */
static int _php_gd_image_to_avframe(gdImage *src, AVFrame *frame, int width, int height) 
{
    int x, y;
    int *dest = (int*)frame->data[0];

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
			// FIXME: don't do bounds check until we fix the gd.h incompatiblity problem 
            //if (gdImageBoundsSafe(src, x, y)) {
                dest[x] = src->tpixels[y][x];
            /*} else {
                return -1;
            }*/
        }
        dest += width;
    }
    return 0;
}
/* }}} */


/* {{{ proto resource toGDImage()
 */
PHP_FUNCTION(toGDImage)
{
    ff_frame_context *ff_frame;
    gdImage *gd_img;

    GET_FRAME_RESOURCE(getThis(), ff_frame);

    _php_convert_frame(ff_frame, PIX_FMT_RGBA32);

    return_value->value.lval = _php_get_gd_image(ff_frame->width, 
            ff_frame->height);

    return_value->type = IS_RESOURCE;

    FFMPEG_PHP_FETCH_IMAGE_RESOURCE(gd_img, &return_value);

    if (_php_avframe_to_gd_image(ff_frame->av_frame, gd_img,
            ff_frame->width, ff_frame->height)) {
        // don't error until we fix the gdImageBounds problem with older GD
        //zend_error(E_ERROR, "failed to convert frame to gd image");
    }
}
/* }}} */


/* {{{ proto object _php_read_frame_from_file(mixed)
 */
/*_php_read_frame_from_file(ff_frame_context *ff_frame, char* filename)
{
    AVFrame *frame = NULL;
    AVFormatContext *ic;
    AVFormatParameters *ap;
    int err;

    // open the input file with generic libav function
    err = av_open_input_file(&ic, filename, NULL, 0, ap);
    if (err < 0) {
        zend_error(E_ERROR, "Can't open image file %d, %d", err, AVERROR_NOFMT);
    }

    
}*/
/* }}} */


/* {{{ proto object ffmpeg_frame(mixed)
 */
PHP_FUNCTION(ffmpeg_frame)
{
    zval **argv[1];
    AVFrame *frame;
    gdImage *gd_img;
    ff_frame_context *ff_frame;
    int width, height, ret;

    if (ZEND_NUM_ARGS() != 1) {
        WRONG_PARAM_COUNT;
    }

    /* retrieve argument */
    if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), argv) != SUCCESS) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR,
                "Error parsing arguments");
    }

    ff_frame = _php_alloc_ff_frame();
    
	ret = ZEND_REGISTER_RESOURCE(NULL, ff_frame, le_ffmpeg_frame);
    
    object_init_ex(getThis(), ffmpeg_frame_class_entry_ptr);
    add_property_resource(getThis(), "ffmpeg_frame", ret);
    
    switch (Z_TYPE_PP(argv[0])) {
        case IS_STRING:
            /* TODO: test for resource or string */
            convert_to_string_ex(argv[0]);
            zend_error(E_ERROR, "Creating an ffmpeg_frame from a file is not implemented yet\n");
            //_php_read_frame_from_file(ff_frame, Z_STRVAL_PP(argv[0]));
            break;
        case IS_RESOURCE:
            FFMPEG_PHP_FETCH_IMAGE_RESOURCE(gd_img, &return_value);

            if (!gd_img->trueColor) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "First parameter must be a truecolor gd image.");
            }

            width = gdImageSX(gd_img);
            height = gdImageSY(gd_img);

            frame = avcodec_alloc_frame();
            avpicture_alloc((AVPicture*)frame, PIX_FMT_RGBA32, width, height);
            _php_gd_image_to_avframe(gd_img, frame, width, height);
            
            ff_frame->av_frame = frame;
            ff_frame->width = width;
            ff_frame->height = height;
            ff_frame->pixel_format = PIX_FMT_RGBA32;
            break;
        default:
            zend_error(E_ERROR, "Invalid argument\n");
            
    }
}
/* }}} */

#endif /* HAVE_LIBGD20 */


/* {{{ proto int getPresentationTimestamp()
 */
PHP_FUNCTION(getPresentationTimestamp)
{
    ff_frame_context *ff_frame;

    GET_FRAME_RESOURCE(getThis(), ff_frame);
    
    RETURN_DOUBLE((double)ff_frame->pts / AV_TIME_BASE);
}
/* }}} */


/* {{{ proto int isKeyFrame()
 */
PHP_FUNCTION(isKeyFrame)
{
    ff_frame_context *ff_frame;

    GET_FRAME_RESOURCE(getThis(), ff_frame);
    
    RETURN_LONG(ff_frame->keyframe);
}
/* }}} */


/* {{{ proto int getWidth()
 */
PHP_FUNCTION(getWidth)
{
    ff_frame_context *ff_frame;

    GET_FRAME_RESOURCE(getThis(), ff_frame);
    
    RETURN_LONG(ff_frame->width);
}
/* }}} */


/* {{{ proto int getHeight()
 */
PHP_FUNCTION(getHeight)
{
    ff_frame_context *ff_frame;

    GET_FRAME_RESOURCE(getThis(), ff_frame);
    
    RETURN_LONG(ff_frame->height);
}
/* }}} */


/* {{{ proto boolean crop([, int crop_top [, int crop_bottom [, int crop_left [, int crop_right ]]]])
*/
PHP_FUNCTION(crop)
{
    zval ***argv;
    ff_frame_context *ff_frame;
    int crop_top = 0, crop_bottom = 0, crop_left = 0, crop_right = 0;

    GET_FRAME_RESOURCE(getThis(), ff_frame);

    /* retrieve arguments */ 
    argv = (zval ***) safe_emalloc(sizeof(zval **), ZEND_NUM_ARGS(), 0);

    if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), argv) != SUCCESS) {
        efree(argv);
        php_error_docref(NULL TSRMLS_CC, E_ERROR,
                "Error parsing arguments");
    }

    switch (ZEND_NUM_ARGS()) {
        case 4:
            convert_to_long_ex(argv[3]);
            crop_right = Z_LVAL_PP(argv[3]);

            /* crop right must be even number for lavc cropping */
            if (crop_right % 2) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "Crop right must be an even number");
            }
            /* fallthru */
        case 3:
            convert_to_long_ex(argv[2]);
            crop_left = Z_LVAL_PP(argv[2]);

            /*  crop left must be even number for lavc cropping */
            if (crop_left % 2) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "Crop left must be an even number");
            }

            /* fallthru */
        case 2:
            convert_to_long_ex(argv[1]);
            crop_bottom = Z_LVAL_PP(argv[1]);

            /*  crop bottom must be even number for lavc cropping */
            if (crop_bottom % 2) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "Crop bottom must be an even number");
            }

            /* fallthru */
        case 1:
            convert_to_long_ex(argv[0]);
            crop_top = Z_LVAL_PP(argv[0]);

            /*  crop top  must be even number for lavc cropping */
            if (crop_top % 2) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "Crop top must be an even number");
            }
            break;
        default:
            WRONG_PARAM_COUNT;
    } 

    efree(argv);

    /* crop frame */
    _php_crop_frame(ff_frame, crop_top, crop_bottom, crop_left, crop_right);

    RETURN_TRUE;
}
/* }}} */


/* {{{ proto boolean resize(int width, int height [, int crop_top [, int crop_bottom [, int crop_left [, int crop_right ]]]])
*/
PHP_FUNCTION(resize)
{
    zval ***argv;
    ff_frame_context *ff_frame;
    int wanted_width = 0, wanted_height = 0;
    int crop_top = 0, crop_bottom = 0, crop_left = 0, crop_right = 0;

    GET_FRAME_RESOURCE(getThis(), ff_frame);

    /* retrieve arguments */ 
    argv = (zval ***) safe_emalloc(sizeof(zval **), ZEND_NUM_ARGS(), 0);

    if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), argv) != SUCCESS) {
        efree(argv);
        php_error_docref(NULL TSRMLS_CC, E_ERROR,
                "Error parsing arguments");
    }

    switch (ZEND_NUM_ARGS()) {
        case 6:
            convert_to_long_ex(argv[5]);
            crop_right = Z_LVAL_PP(argv[5]);

            /* crop right must be even number for lavc cropping */
            if (crop_right % 2) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "Crop right must be an even number");
            }
            /* fallthru */
        case 5:
            convert_to_long_ex(argv[4]);
            crop_left = Z_LVAL_PP(argv[4]);

            /*  crop left must be even number for lavc cropping */
            if (crop_left % 2) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "Crop left must be an even number");
            }

            /* fallthru */
        case 4:
            convert_to_long_ex(argv[3]);
            crop_bottom = Z_LVAL_PP(argv[3]);

            /*  crop bottom must be even number for lavc cropping */
            if (crop_bottom % 2) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "Crop bottom must be an even number");
            }

            /* fallthru */
        case 3:
            convert_to_long_ex(argv[2]);
            crop_top = Z_LVAL_PP(argv[2]);

            /*  crop top must be even number for lavc cropping */
            if (crop_top % 2) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "Crop top must be an even number");
            }

            /* fallthru */
        case 2:
            /* height arg */
            convert_to_long_ex(argv[1]);
            wanted_height = Z_LVAL_PP(argv[1]);

            /* bounds check wanted height */
            if (wanted_height < 1) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "Frame height must be greater than zero");
            }

            /* wanted height must be even number for lavc resample */
            if (wanted_height % 2) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "Frame height must be an even number");
            }
        case 1:
            /* width arg */
            convert_to_long_ex(argv[0]);
            wanted_width = Z_LVAL_PP(argv[0]);

            /* bounds check wanted width */
            if (wanted_width < 1) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "Frame width must be greater than zero");
            }

            /* wanted width must be even number for lavc resample */
            if (wanted_width % 2) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "Frame width must be an even number");
            }
            break;
        default:
            WRONG_PARAM_COUNT;
    } 

    efree(argv);

        /* resize frame */
    _php_resample_frame(ff_frame, wanted_width, wanted_height, 
            crop_top, crop_bottom, crop_left, crop_right);

    RETURN_TRUE;
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
