/*
   This file is part of ffmpeg-php

   Copyright (C) 2004-2007 Todd Kirby (ffmpeg.php@gmail.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

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

#ifndef FFMPEG_ANIMATED_GIF_H
#define FFMPEG_ANIMATED_GIF_H

#include <avcodec.h>
#include <avformat.h>

/* animated gif constructor */
PHP_FUNCTION(ffmpeg_animated_gif);

/* animated gif methods */
PHP_FUNCTION(addFrame);

typedef struct {
    AVStream *video_st;
    AVOutputFormat *fmt;
    AVFormatContext *fmt_ctx;
    AVCodecContext *codec_ctx;

    uint8_t *video_outbuf;
    int video_outbuf_size;
} ff_animated_gif_context;

#endif // FFMPEG_ANIMATED_GIF_H
