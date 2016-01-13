/*
   This file is part of ffmpeg-php

   Copyright (C) 2004-2008 Todd Kirby (ffmpeg.php AT gmail.com)

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

   In addition, as a special exception, the copyright holders of ffmpeg-php
   give you permission to combine ffmpeg-php with code included in the
   standard release of PHP under the PHP license (or modified versions of
   such code, with unchanged license). You may copy and distribute such a
   system following the terms of the GNU GPL for ffmpeg-php and the licenses
   of the other code concerned, provided that you include the source code of
   that other code when and as the GNU GPL requires distribution of source code.

   You must obey the GNU General Public License in all respects for all of the
   code used other than standard release of PHP. If you modify this file, you
   may extend this exception to your version of the file, but you are not
   obligated to do so. If you do not wish to do so, delete this exception
   statement from your version.

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"

#include "php_ffmpeg.h"

#include "ffmpeg_frame.h"
#include "ffmpeg_tools.h"

/*
   include gd header from local include dir. This is a copy of gd.h that is
   distributed with php-5.2.5. It is distributed along with ffmpeg-php to
   allow ffmpeg-php to be built without access to the php sources
   */
#if HAVE_LIBGD20
#include "gd.h"

#define FFMPEG_PHP_FETCH_IMAGE_RESOURCE(gd_img, ret) { \
    ZEND_GET_RESOURCE_TYPE_ID(le_gd, "gd"); \
    ZEND_FETCH_RESOURCE(gd_img, gdImagePtr, ret, -1, "Image", le_gd); \
}

#if PIX_FMT_RGBA32
#define FFMPEG_PHP_FFMPEG_RGB_PIX_FORMAT PIX_FMT_RGBA32
#else
#define FFMPEG_PHP_FFMPEG_RGB_PIX_FORMAT PIX_FMT_RGB32
#endif

#define gdImageBoundsSafeMacro(im, x, y) (!((((y) < (im)->cy1) || ((y) > (im)->cy2)) || (((x) < (im)->cx1) || ((x) > (im)->cx2))))

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

    /* object can't be created from user space so no PHP constructor */

#if HAVE_LIBGD20
    /* gd methods */
    FFMPEG_PHP_MALIAS(ffmpeg_frame, togdimage,      toGDImage,     NULL, 0)
    PHP_ME(ffmpeg_frame, __construct, NULL, 0)
#endif // HAVE_LIBGD20

    /* methods */
    FFMPEG_PHP_MALIAS(ffmpeg_frame, getwidth,                    getWidth,                   NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_frame, getheight,                   getHeight,                  NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_frame, resize,                      resize,                     NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_frame, iskeyframe,                  isKeyFrame,                 NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_frame, getpresentationtimestamp,    getPresentationTimestamp,   NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_frame, getpts,                      getPresentationTimestamp,   NULL, 0)
    FFMPEG_PHP_END_METHODS
};
/* }}} */


/* {{{ _php_alloc_ff_frame()
*/
static ff_frame_context* _php_alloc_ff_frame()
{
    ff_frame_context *ff_frame = NULL;

    ff_frame = emalloc(sizeof(ff_frame_context));

    ff_frame->av_frame = NULL;
    ff_frame->width = 0;
    ff_frame->height = 0;
    ff_frame->pixel_format = 0;

    return ff_frame;
}
/* }}} */


/* {{{ proto object _php_create_ffmpeg_frame()
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


/* {{{ _php_free_av_frame()
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
/* }}} */


/* {{{ _php_free_ffmpeg_frame()
*/
static void _php_free_ffmpeg_frame(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    ff_frame_context *ff_frame = (ff_frame_context*)rsrc->ptr;
    _php_free_av_frame(ff_frame->av_frame);
    efree(ff_frame);
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
/* }}} */


/* {{{ _php_convert_frame()
*/
int _php_convert_frame(ff_frame_context *ff_frame, int dst_fmt) {
   TSRMLS_FETCH();
   
    AVFrame *dst_frame;
    int result = 0;

    if (!ff_frame->av_frame) {
        return -1;
    }

    if (dst_fmt == ff_frame->pixel_format) {
        return 0; // NOP
    }

    dst_frame = avcodec_alloc_frame();
    avpicture_alloc((AVPicture*)dst_frame, dst_fmt, ff_frame->width,
            ff_frame->height);

    result = img_convert( (AVPicture *)dst_frame, dst_fmt,
            (AVPicture *) ff_frame->av_frame, ff_frame->pixel_format, ff_frame->width, ff_frame->height);

    if (result) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error converting frame");
        _php_free_av_frame(dst_frame);
    } else {
        ff_frame->av_frame = dst_frame;
        ff_frame->pixel_format = dst_fmt;
    }

    return result;
}
/* }}} */

#if HAVE_LIBGD20

/* {{{ _php_get_gd_image()
*/
static int _php_get_gd_image(int w, int h TSRMLS_DC)
{
    zval *function_name, *width, *height;
    zval **argv[2];
    zend_function *func;
    zval *retval;
    char *function_cname = "imagecreatetruecolor";
    int ret = -1;

    if (zend_hash_find(EG(function_table), function_cname, strlen(function_cname) + 1, (void **)&func) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error can't find %s function", function_cname);
        return -1;
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
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error calling %s function", function_cname);
        goto out;
    }

    if (!retval || Z_TYPE_P(retval) != IS_RESOURCE) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error creating GD Image");
        goto out;
    }

    ret = Z_LVAL_P(retval);
    zend_list_addref(ret);
    zval_ptr_dtor(&retval);

out:
    FREE_ZVAL(function_name);
    FREE_ZVAL(width);
    FREE_ZVAL(height);

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

    if (width > dest->sx || height > dest->sy) {
        return -1;
    }

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            /* copy pixel to gdimage buffer zeroing the alpha channel */
            dest->tpixels[y][x] = src[x] & 0x00ffffff;
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
            dest[x] = src->tpixels[y][x];
        }
        dest += width;
    }
    return 0;
}
/* }}} */


/* {{{ proto resource toGDImage()
*/
FFMPEG_PHP_METHOD(ffmpeg_frame, toGDImage)
{
    ff_frame_context *ff_frame;
    gdImage *gd_img;
    int rsrc_id;

    GET_FRAME_RESOURCE(getThis(), ff_frame);

    _php_convert_frame(ff_frame, FFMPEG_PHP_FFMPEG_RGB_PIX_FORMAT);

    rsrc_id = _php_get_gd_image(ff_frame->width, ff_frame->height TSRMLS_CC);

    if (rsrc_id < 0) {
        RETURN_FALSE;
    }

    RETVAL_RESOURCE(rsrc_id);

    FFMPEG_PHP_FETCH_IMAGE_RESOURCE(gd_img, &return_value);

    if (_php_avframe_to_gd_image(ff_frame->av_frame, gd_img, ff_frame->width, ff_frame->height)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to convert frame to gd image");
        zend_list_delete(rsrc_id);
        RETURN_FALSE;
    }
    /* return the resource */
}
/* }}} */

/* {{{ proto object ffmpeg_frame(mixed)
*/
FFMPEG_PHP_METHOD(ffmpeg_frame, __construct)
{
    AVFrame *frame;
    gdImage *gd_img;
    ff_frame_context *ff_frame;
    int width, height, ret;
    zval *param;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &param) != SUCCESS) {
        return;
    }

    switch (Z_TYPE_P(param)) {
        case IS_STRING:
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Creating an ffmpeg_frame from a file is not implemented");
            RETURN_FALSE;
            //_php_read_frame_from_file(ff_frame, Z_STRVAL_PP(argv[0]));
            break;
        case IS_RESOURCE:
            FFMPEG_PHP_FETCH_IMAGE_RESOURCE(gd_img, &param);

            if (!gd_img->trueColor) {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "First parameter must be a truecolor GD image");
                RETURN_FALSE;
            }

            ff_frame = _php_alloc_ff_frame();

            ret = ZEND_REGISTER_RESOURCE(NULL, ff_frame, le_ffmpeg_frame);

            object_init_ex(getThis(), ffmpeg_frame_class_entry_ptr);
            add_property_resource(getThis(), "ffmpeg_frame", ret);

            width = gdImageSX(gd_img);
            height = gdImageSY(gd_img);

            /* create a an av_frame and allocate space for it */
            frame = avcodec_alloc_frame();
            avpicture_alloc((AVPicture*)frame, FFMPEG_PHP_FFMPEG_RGB_PIX_FORMAT, width, height);

            /* copy the gd image to the av_frame */
            _php_gd_image_to_avframe(gd_img, frame, width, height);

            /* set the ffmepg_frame to point to this av_frame */
            ff_frame->av_frame = frame;

            /* set the ffpmeg_frame's properties */
            ff_frame->width = width;
            ff_frame->height = height;
            ff_frame->pixel_format = FFMPEG_PHP_FFMPEG_RGB_PIX_FORMAT;
            break;
        default:
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid argument passed");
            RETURN_FALSE;
    }
}
/* }}} */

#endif /* HAVE_LIBGD20 */


/* {{{ proto int getPresentationTimestamp()
*/
FFMPEG_PHP_METHOD(ffmpeg_frame, getPresentationTimestamp)
{
    ff_frame_context *ff_frame;

    GET_FRAME_RESOURCE(getThis(), ff_frame);

    RETURN_DOUBLE((double)ff_frame->pts / AV_TIME_BASE);
}
/* }}} */


/* {{{ proto int isKeyFrame()
*/
FFMPEG_PHP_METHOD(ffmpeg_frame, isKeyFrame)
{
    ff_frame_context *ff_frame;

    GET_FRAME_RESOURCE(getThis(), ff_frame);

    RETURN_LONG(ff_frame->keyframe);
}
/* }}} */


/* {{{ proto int getWidth()
*/
FFMPEG_PHP_METHOD(ffmpeg_frame, getWidth)
{
    ff_frame_context *ff_frame;

    GET_FRAME_RESOURCE(getThis(), ff_frame);

    RETURN_LONG(ff_frame->width);
}
/* }}} */


/* {{{ proto int getHeight()
*/
FFMPEG_PHP_METHOD(ffmpeg_frame, getHeight)
{
    ff_frame_context *ff_frame;

    GET_FRAME_RESOURCE(getThis(), ff_frame);

    RETURN_LONG(ff_frame->height);
}
/* }}} */


/* {{{ _php_resample_frame()
*/
int _php_resample_frame(ff_frame_context *ff_frame,
        int wanted_width, int wanted_height, int crop_top, int crop_bottom,
        int crop_left, int crop_right)
{
    AVFrame *resampled_frame;
    ImgReSampleContext *img_resample_ctx = NULL;

    if (!ff_frame->av_frame) {
        return -1;
    }

    /*
     * do nothing if width and height are the same as the frame and no
     * cropping was specified
     * */
    if (wanted_width == ff_frame->width &&
            wanted_height == ff_frame->height &&
            (!crop_left && !crop_right && !crop_top && !crop_bottom)) {
        return 0;
    }

    /* convert to AV_PIX_FMT_YUV420P required for resampling */
    _php_convert_frame(ff_frame, AV_PIX_FMT_YUV420P);

    img_resample_ctx = img_resample_full_init(
            wanted_width, wanted_height,
            ff_frame->width, ff_frame->height,
            crop_top, crop_bottom, crop_left, crop_right,
            0, 0, 0, 0);
    if (!img_resample_ctx) {
        return -1;
    }

    resampled_frame = avcodec_alloc_frame();
    avpicture_alloc((AVPicture*)resampled_frame, AV_PIX_FMT_YUV420P,
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


/* {{{ proto boolean resize(int width, int height [, int crop_top [, int crop_bottom [, int crop_left [, int crop_right ]]]])
*/
FFMPEG_PHP_METHOD(ffmpeg_frame, resize)
{
    ff_frame_context *ff_frame = NULL;
    long wanted_width, wanted_height;
    long crop_top = 0, crop_bottom = 0, crop_left = 0, crop_right = 0;
    int res;

    GET_FRAME_RESOURCE(getThis(), ff_frame);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll|llll", &wanted_width, &wanted_height, &crop_top, &crop_bottom, &crop_left, &crop_right) != SUCCESS) {
        return;
    }

    if (crop_right % 2) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Crop right must be an even number");
        RETURN_FALSE;
    }

    if (crop_left % 2) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Crop left must be an even number");
        RETURN_FALSE;
    }

    if (crop_bottom % 2) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Crop bottom must be an even number");
        RETURN_FALSE;
    }

    if (crop_top % 2) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Crop top be an even number");
        RETURN_FALSE;
    }

    /* bounds check wanted height */
    if (wanted_height < 1) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Frame height must be greater than zero");
        RETURN_FALSE;
    }

    /* wanted height must be even number for lavc resample */
    if (wanted_height % 2) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Frame height must be an even number");
        RETURN_FALSE;
    }

    /* bounds check wanted width */
    if (wanted_width < 1) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Frame width must be greater than zero");
        RETURN_FALSE;
    }

    /* wanted width must be even number for lavc resample */
    if (wanted_width % 2) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Frame width must be an even number");
        RETURN_FALSE;
    }

    /* resize frame */
    res = _php_resample_frame(ff_frame, wanted_width, wanted_height,
            crop_top, crop_bottom, crop_left, crop_right);

    if (res < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}
/* }}} */


/* {{{ proto boolean crop([, int crop_top [, int crop_bottom [, int crop_left [, int crop_right ]]]])
*/
PHP_FUNCTION(crop)
{
    ff_frame_context *ff_frame;
    long crop_top = 0, crop_bottom = 0, crop_left = 0, crop_right = 0;
    int res;

    GET_FRAME_RESOURCE(getThis(), ff_frame);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|llll", &crop_top, &crop_bottom, &crop_left, &crop_bottom) != SUCCESS) {
        return;
    }

    if (crop_right % 2) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Crop right must be an even number");
        RETURN_FALSE;
    }

    if (crop_left % 2) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Crop left must be an even number");
        RETURN_FALSE;
    }

    if (crop_bottom % 2) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Crop bottom must be an even number");
        RETURN_FALSE;
    }

    if (crop_top % 2) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Crop top be an even number");
        RETURN_FALSE;
    }

    /* resample with same dimensions */
    res = _php_resample_frame(ff_frame, ff_frame->width, ff_frame->height,
            crop_top, crop_bottom, crop_left, crop_right);

    if (res < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 expandtab
 * vim<600: noet sw=4 ts=4
 */
