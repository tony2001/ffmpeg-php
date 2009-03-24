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

#ifndef FFMPEG_FRAME_H
#define FFMPEG_FRAME_H

#include "php_version.h"

#include <avcodec.h>
#include <avformat.h>
#include <avutil.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* object can't be created from user space so no PHP constructor */
//FFMPEG_PHP_CONSTRUCTOR(ffmpeg_frame, __construct);

/* frame methods */
FFMPEG_PHP_METHOD(ffmpeg_frame, getWidth);
FFMPEG_PHP_METHOD(ffmpeg_frame, getHeight);
FFMPEG_PHP_METHOD(ffmpeg_frame, isKeyFrame);
FFMPEG_PHP_METHOD(ffmpeg_frame, getPresentationTimestamp);

#if HAVE_LIBGD20
FFMPEG_PHP_METHOD(ffmpeg_frame, toGDImage);
#endif // HAVE_LIBGD20

typedef struct {
    AVFrame *av_frame;
    int width;
    int height;
    int pixel_format;
    int keyframe;
    int64_t pts;
} ff_frame_context;

ff_frame_context* _php_create_ffmpeg_frame(INTERNAL_FUNCTION_PARAMETERS);

int _php_convert_frame(ff_frame_context *ff_frame, int new_fmt);

#define GET_FRAME_RESOURCE(ffmpeg_frame_object, ffmpeg_frame) {\
	zval **_tmp_zval;\
    if (zend_hash_find(Z_OBJPROP_P(ffmpeg_frame_object), "ffmpeg_frame",\
                sizeof("ffmpeg_frame"), (void **)&_tmp_zval) == FAILURE) {\
        zend_error(E_ERROR, "Unable to locate ffmpeg_frame resource in this object.");\
        RETURN_FALSE;\
    }\
\
    ZEND_FETCH_RESOURCE(ffmpeg_frame, ff_frame_context*, _tmp_zval, -1,\
            "ffmpeg_frame", le_ffmpeg_frame);\
}\

#endif // FFMPEG_FRAME_H

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4
 * vim<600: noet sw=4 ts=4
 */
