#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"

#include "php_ffmpeg.h"

#include "quadrupel/qp_frame.h"
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

static int le_gd; // this is only valid after calling 
                  // FFMPEG_PHP_FETCH_IMAGE_RESOURCE() 

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


/* {{{ proto object _php_create_ffmpeg_frame() 
   creates an ffmpeg_frame object, adds a ffmpeg_frame resource to the
   object, registers the resource and returns a direct pointer to the 
   resource.
 */
qp_frame_context* _php_create_ffmpeg_frame(INTERNAL_FUNCTION_PARAMETERS)
{
    int ret;
	qp_frame_context *qp_frame;
    
    qp_frame = qp_alloc_frame_ctx(&_emalloc);
    
	ret = ZEND_REGISTER_RESOURCE(NULL, qp_frame, le_ffmpeg_frame);
    
    object_init_ex(return_value, ffmpeg_frame_class_entry_ptr);
    add_property_resource(return_value, "ffmpeg_frame", ret);
    return qp_frame;
}
/* }}} */


/* {{{ _php_free_ffmpeg_frame()
 */
static void _php_free_ffmpeg_frame(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    qp_frame_context *qp_frame = (qp_frame_context*)rsrc->ptr;    
    qp_free_frame_ctx(qp_frame, _efree);
}
/* }}} */


/* {{{ register_ffmpeg_frame_class()
 */
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
static int _php_avframe_to_gd_image(AVFrame *frame, gdImage *dest, int width, 
        int height)
{
    int x, y;
    int *src = (int*)frame->data[0];

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
		
			// FIXME: gdImageBoundsSafe is implemented as a macro in the gd library
            //        bundled with php and as a function in other gd libraries.
            //        The macro and the function are incompatible so disable this
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
static int _php_gd_image_to_avframe(gdImage *src, AVFrame *frame, int width, 
        int height) 
{
    int x, y;
    int *dest = (int*)frame->data[0];

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
			// FIXME: don't do bounds check until we fix the gd.h 
            // incompatiblity problem 
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
    qp_frame_context *qp_frame;
    gdImage *gd_img;

    GET_FRAME_RESOURCE(getThis(), qp_frame);

    qp_convert_frame(qp_frame, PIX_FMT_RGBA32);

    return_value->value.lval = _php_get_gd_image(qp_frame->width, 
            qp_frame->height);

    return_value->type = IS_RESOURCE;

    FFMPEG_PHP_FETCH_IMAGE_RESOURCE(gd_img, &return_value);

    if (_php_avframe_to_gd_image(qp_frame->av_frame, gd_img,
            qp_frame->width, qp_frame->height)) {
        // don't error until we fix the gdImageBounds problem with older GD
        //zend_error(E_ERROR, "failed to convert frame to gd image");
    }
}
/* }}} */


/* {{{ proto object _php_read_frame_from_file(mixed)
 */
/*_php_read_frame_from_file(qp_frame_context *qp_frame, char* filename)
{
    AVFrame *frame = NULL;
    AVFormatContext *ic;
    AVFormatParameters *ap;
    int err;

    // open the input file with generic libav function
    err = av_open_input_file(&ic, filename, NULL, 0, ap);
    if (err < 0) {
        zend_error(E_ERROR, "Can't open image file %d, %d", err, 
        AVERROR_NOFMT);
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
    qp_frame_context *qp_frame;
    int width, height, ret;

    if (ZEND_NUM_ARGS() != 1) {
        WRONG_PARAM_COUNT;
    }

    /* retrieve argument */
    if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), argv) != SUCCESS) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR,
                "Error parsing arguments");
    }

    qp_frame = qp_alloc_frame_ctx(&_emalloc);
    
	ret = ZEND_REGISTER_RESOURCE(NULL, qp_frame, le_ffmpeg_frame);
    
    object_init_ex(getThis(), ffmpeg_frame_class_entry_ptr);
    add_property_resource(getThis(), "ffmpeg_frame", ret);
    
    switch (Z_TYPE_PP(argv[0])) {
        case IS_STRING:
            convert_to_string_ex(argv[0]);
            zend_error(E_ERROR, 
                  "Creating an ffmpeg_frame from a file is not implemented\n");
            //_php_read_frame_from_file(qp_frame, Z_STRVAL_PP(argv[0]));
            break;
        case IS_RESOURCE:
            FFMPEG_PHP_FETCH_IMAGE_RESOURCE(gd_img, argv[0]);

            if (!gd_img->trueColor) {
                php_error_docref(NULL TSRMLS_CC, E_ERROR,
                        "First parameter must be a truecolor gd image.");
            }

            width = gdImageSX(gd_img);
            height = gdImageSY(gd_img);

            /* create a an av_frame and allocate space for it */
            frame = avcodec_alloc_frame();
            avpicture_alloc((AVPicture*)frame, PIX_FMT_RGBA32, width, height);

            /* copy the gd image to the av_frame */
            _php_gd_image_to_avframe(gd_img, frame, width, height);
            
            /* set the ffmepg_frame to point to this av_frame */
            qp_frame->av_frame = frame;
            
            /* set the ffpmeg_frame's properties */
            qp_frame->width = width;
            qp_frame->height = height;
            qp_frame->pixel_format = PIX_FMT_RGBA32;
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
    qp_frame_context *qp_frame;

    GET_FRAME_RESOURCE(getThis(), qp_frame);
    
    RETURN_DOUBLE(qp_get_pts(qp_frame));
}
/* }}} */


/* {{{ proto int isKeyFrame()
 */
PHP_FUNCTION(isKeyFrame)
{
    qp_frame_context *qp_frame;

    GET_FRAME_RESOURCE(getThis(), qp_frame);
    
    RETURN_LONG(qp_frame_is_keyframe(qp_frame));
}
/* }}} */


/* {{{ proto int getWidth()
 */
PHP_FUNCTION(getWidth)
{
    qp_frame_context *qp_frame;

    GET_FRAME_RESOURCE(getThis(), qp_frame);
    
    RETURN_LONG(qp_frame->width);
}
/* }}} */


/* {{{ proto int getHeight()
 */
PHP_FUNCTION(getHeight)
{
    qp_frame_context *qp_frame;

    GET_FRAME_RESOURCE(getThis(), qp_frame);
    
    RETURN_LONG(qp_frame->height);
}
/* }}} */


/* {{{ proto boolean crop([, int crop_top [, int crop_bottom [, int crop_left [, int crop_right ]]]])
*/
PHP_FUNCTION(crop)
{
    zval ***argv;
    qp_frame_context *qp_frame;
    int crop_top = 0, crop_bottom = 0, crop_left = 0, crop_right = 0;

    GET_FRAME_RESOURCE(getThis(), qp_frame);

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
    qp_crop_frame(qp_frame, crop_top, crop_bottom, crop_left, crop_right);

    RETURN_TRUE;
}
/* }}} */


/* {{{ proto boolean resize(int width, int height [, int crop_top [, int crop_bottom [, int crop_left [, int crop_right ]]]])
*/
PHP_FUNCTION(resize)
{
    zval ***argv;
    qp_frame_context *qp_frame;
    int wanted_width = 0, wanted_height = 0;
    int crop_top = 0, crop_bottom = 0, crop_left = 0, crop_right = 0;

    GET_FRAME_RESOURCE(getThis(), qp_frame);

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
    qp_resample_frame(qp_frame, wanted_width, wanted_height, 
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
