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

#ifndef FFMPEG_MOVIE_H
#define FFMPEG_MOVIE_H

#include <avcodec.h>
#include <avformat.h>

/* movie constructor */
PHP_METHOD(ffmpeg_movie, __construct);

/* movie methods */
PHP_METHOD(ffmpeg_movie, getDuration);
PHP_METHOD(ffmpeg_movie, getFrameCount);
PHP_METHOD(ffmpeg_movie, getFrameRate);
PHP_METHOD(ffmpeg_movie, getFileName);
PHP_METHOD(ffmpeg_movie, getComment);
PHP_METHOD(ffmpeg_movie, getTitle);
PHP_METHOD(ffmpeg_movie, getAuthor);
PHP_METHOD(ffmpeg_movie, getCopyright);
PHP_METHOD(ffmpeg_movie, getAlbum);
PHP_METHOD(ffmpeg_movie, getGenre);
PHP_METHOD(ffmpeg_movie, getTrackNumber);
PHP_METHOD(ffmpeg_movie, getYear);
PHP_METHOD(ffmpeg_movie, getFrameWidth);
PHP_METHOD(ffmpeg_movie, getFrameHeight);
PHP_METHOD(ffmpeg_movie, getFrameNumber);
PHP_METHOD(ffmpeg_movie, getPixelFormat);
PHP_METHOD(ffmpeg_movie, getBitRate);
PHP_METHOD(ffmpeg_movie, hasAudio);
PHP_METHOD(ffmpeg_movie, hasVideo);
PHP_METHOD(ffmpeg_movie, getNextKeyFrame);
PHP_METHOD(ffmpeg_movie, getFrame);
PHP_METHOD(ffmpeg_movie, getVideoCodec);
PHP_METHOD(ffmpeg_movie, getAudioCodec);
PHP_METHOD(ffmpeg_movie, getAudioChannels);
PHP_METHOD(ffmpeg_movie, getAudioSampleRate);
PHP_METHOD(ffmpeg_movie, getAudioBitRate);
PHP_METHOD(ffmpeg_movie, getVideoBitRate);
PHP_METHOD(ffmpeg_movie, getPixelAspectRatio);
PHP_METHOD(ffmpeg_movie, getVideoStreamId);
PHP_METHOD(ffmpeg_movie, getAudioStreamId);

typedef struct {
    AVFormatContext *fmt_ctx;
    AVCodecContext *codec_ctx[MAX_STREAMS];
    int64_t last_pts;
    int frame_number;
    long rsrc_id;
} ff_movie_context;

#endif // FFMPEG_MOVIE_H


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4
 * vim<600: noet sw=4 ts=4
 */
