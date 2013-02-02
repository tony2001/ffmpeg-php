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

//#include <php.h>

#include "ffmpeg_tools.h"

#if LIBAVCODEC_VERSION_MAJOR >= 52
#include <libswscale/swscale.h>

/* {{{ ffmpeg_img_convert() 
 * wrapper around ffmpeg image conversion routines
 */
int img_convert(AVPicture *dst, int dst_pix_fmt,
        AVPicture *src, int src_pix_fmt, int src_width, int src_height)
{
    struct SwsContext *sws_ctx = NULL;

    // TODO: Try to get cached sws_context first
    sws_ctx = sws_getContext(src_width, src_height, 0, 
            src_width, src_height, dst_pix_fmt, 
            SWS_BICUBIC, NULL, NULL, NULL);

    if (sws_ctx == NULL){
        return 2;
    }

    sws_scale(sws_ctx, src->data, src->linesize, 0, src_height, dst->data, dst->linesize);
    sws_freeContext(sws_ctx);

    return 0;
}
/* }}} */



void img_resample(ImgReSampleContext * context, AVPicture * pxOut, const AVPicture * pxIn)
{
    if (context != NULL && context->context != NULL) {
        AVPicture shiftedInput; // = {0};
        shiftedInput.data[0] = pxIn->data[0] + pxIn->linesize[0] * 
            context->bandTop + context->bandLeft;
        shiftedInput.data[1] = pxIn->data[1] + (pxIn->linesize[1] * 
                (context->bandTop / 2)) + (context->bandLeft+1) / 2;
        shiftedInput.data[2] = pxIn->data[2] + (pxIn->linesize[2] * 
                (context->bandTop / 2)) + (context->bandLeft+1) / 2;
        shiftedInput.linesize[0] = pxIn->linesize[0];
        shiftedInput.linesize[1] = pxIn->linesize[1];
        shiftedInput.linesize[2] = pxIn->linesize[2];
        sws_scale(context->context, (uint8_t**)shiftedInput.data, 
                (int*)shiftedInput.linesize, 0, context->height - context->bandBottom - 
                context->bandTop, pxOut->data, pxOut->linesize);
    }
}

ImgReSampleContext * img_resample_full_init (int owidth, int oheight, int iwidth, int iheight, int topBand, int bottomBand, int leftBand, int rightBand, int padtop, int padbottom, int padleft, int padright)
{
    ImgReSampleContext * s = (ImgReSampleContext *)av_malloc(sizeof(ImgReSampleContext));
    if (s == NULL) {
        return NULL;
    }
    int srcSurface = (iwidth - rightBand - leftBand)* (iheight - topBand - bottomBand);
    // We use bilinear when the source surface is big, and bicubic when the number of pixels to handle is less than 1 MPixels
    s->context = sws_getContext(iwidth - rightBand - leftBand, 
            iheight - topBand - bottomBand, PIX_FMT_YUV420P, owidth, oheight, 
            PIX_FMT_YUV420P, srcSurface > 1024000 ? SWS_FAST_BILINEAR : SWS_BICUBIC, 
            NULL, NULL, NULL);
    if (s->context == NULL) {
        av_free(s);
        return NULL; }
        s->bandLeft = leftBand;
        s->bandRight = rightBand;
        s->bandTop = topBand;
        s->bandBottom = bottomBand;

        s->padLeft = padleft;
        s->padRight = padright;
        s->padTop = padtop;
        s->padBottom = padbottom;

        s->width = iwidth;
        s->height = iheight;

        s->outWidth = owidth;
        s->outHeight = oheight;

        return s;
}

ImgReSampleContext * img_resample_init (int owidth, int oheight, int iwidth, int iheight)
{
    return img_resample_full_init(owidth, oheight, iwidth, iheight, 0, 0, 0, 0, 0, 0, 0, 0);
}

void img_resample_close(ImgReSampleContext * s)
{
    if (s == NULL) return;
    sws_freeContext(s->context);

    av_free(s);   
}

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4
 * vim<600: noet sw=4 ts=4
 */
