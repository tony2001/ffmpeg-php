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

#include <php.h>

#include "ffmpeg_tools.h"

#ifdef HAVE_SWSCALER
#include <swscale.h>
#endif

/* {{{ ffmpeg_img_convert() 
 * wrapper around ffmpeg image conversion routines
 */
int ffmpeg_img_convert(AVPicture *dst, int dst_pix_fmt,
        AVPicture *src, int src_pix_fmt,
        int src_width, int src_height)
{
#ifndef HAVE_SWSCALER // No SWSCALER so just use img_convert
    return img_convert(dst, dst_pix_fmt, 
            src, src_pix_fmt, src_width, src_height);
#else // Do swscale convert
    int result = 0;
    struct SwsContext *sws_ctx = NULL;

    if (src_pix_fmt == dst_pix_fmt) {
        return 0;
    }

    // TODO: Try to get cached sws_context first
    sws_ctx = sws_getContext(
            src_width, src_height, src_pix_fmt, 
            src_width, src_height, dst_pix_fmt, 
            SWS_BICUBIC, NULL, NULL, NULL);

    if (sws_ctx == NULL){
        return 1;
    }

    result = sws_scale(sws_ctx, 
            src->data, src->linesize,
            0, src_height,
            dst->data, dst->linesize);

    sws_freeContext(sws_ctx);

    if (result == 0){
        return 2;
    }
#endif // NOT HAVE_SWSCALER
    return 0;
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
