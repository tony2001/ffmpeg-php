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

#ifndef FFMPEG_TOOLS_H
#define FFMPEG_TOOLS_H

#include <avcodec.h>

#if LIBAVCODEC_VERSION_MAJOR >= 52

#include <swscale.h>

// Starting from this version, ImgReSampleContext doesn't exist anymore.
// This code implements the previous functions with a similar interface
typedef struct ImgReSampleContext {
    /** The context used for resizing */
    struct SwsContext *context;
    /** The source's width */
    int width;
    /** The source's height */
    int height;
    /** The banding used */
    int bandLeft;
    /** The banding used */
    int bandRight;
    /** The banding used */
    int bandTop;
    /** The banding used */
    int bandBottom;
    /** The padding used */
    int padLeft;
    /** The padding used */
    int padRight;
    /** The padding used */
    int padTop;
    /** The padding used */
    int padBottom;
    /** The output width */
    int outWidth;
    /** The output height */
    int outHeight;
} ImgReSampleContext;

int img_convert(AVPicture *dst, int dst_pix_fmt,
        AVPicture *src, int src_pix_fmt, int src_width, int src_height);

void img_resample_close(ImgReSampleContext *s);

void img_resample(ImgReSampleContext * context, AVPicture * out, const AVPicture * in);

ImgReSampleContext * img_resample_full_init (int owidth, int oheight, 
        int iwidth, int iheight,
        int topBand, int 
        bottomBand, int leftBand, int rightBand,
        int padtop, int 
        padbottom, int padleft, int padright);

ImgReSampleContext * img_resample_init (int owidth, int oheight, int iwidth, int iheight);
#endif

#endif // FFMPEG_TOOLS_H

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4
 * vim<600: noet sw=4 ts=4
 */
