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

/* movie constructor */
FFMPEG_PHP_CONSTRUCTOR(ffmpeg_movie, __construct);

/* movie methods */
FFMPEG_PHP_METHOD(ffmpeg_movie, getDuration);
FFMPEG_PHP_METHOD(ffmpeg_movie, getFrameCount);
FFMPEG_PHP_METHOD(ffmpeg_movie, getFrameRate);
FFMPEG_PHP_METHOD(ffmpeg_movie, getFileName);
FFMPEG_PHP_METHOD(ffmpeg_movie, getComment);
FFMPEG_PHP_METHOD(ffmpeg_movie, getTitle);
FFMPEG_PHP_METHOD(ffmpeg_movie, getAuthor);
FFMPEG_PHP_METHOD(ffmpeg_movie, getCopyright);
FFMPEG_PHP_METHOD(ffmpeg_movie, getAlbum);
FFMPEG_PHP_METHOD(ffmpeg_movie, getGenre);
FFMPEG_PHP_METHOD(ffmpeg_movie, getTrackNumber);
FFMPEG_PHP_METHOD(ffmpeg_movie, getYear);
FFMPEG_PHP_METHOD(ffmpeg_movie, getFrameWidth);
FFMPEG_PHP_METHOD(ffmpeg_movie, getFrameHeight);
FFMPEG_PHP_METHOD(ffmpeg_movie, getFrameNumber);
FFMPEG_PHP_METHOD(ffmpeg_movie, getPixelFormat);
FFMPEG_PHP_METHOD(ffmpeg_movie, getBitRate);
FFMPEG_PHP_METHOD(ffmpeg_movie, hasAudio);
FFMPEG_PHP_METHOD(ffmpeg_movie, hasVideo);
FFMPEG_PHP_METHOD(ffmpeg_movie, getNextKeyFrame);
FFMPEG_PHP_METHOD(ffmpeg_movie, getFrame);
FFMPEG_PHP_METHOD(ffmpeg_movie, getVideoCodec);
FFMPEG_PHP_METHOD(ffmpeg_movie, getAudioCodec);
FFMPEG_PHP_METHOD(ffmpeg_movie, getAudioChannels);
FFMPEG_PHP_METHOD(ffmpeg_movie, getAudioSampleRate);
FFMPEG_PHP_METHOD(ffmpeg_movie, getAudioBitRate);
FFMPEG_PHP_METHOD(ffmpeg_movie, getVideoBitRate);
FFMPEG_PHP_METHOD(ffmpeg_movie, getPixelAspectRatio);
FFMPEG_PHP_METHOD(ffmpeg_movie, getVideoStreamId);
FFMPEG_PHP_METHOD(ffmpeg_movie, getAudioStreamId);

#endif // FFMPEG_MOVIE_H

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4
 * vim<600: noet sw=4 ts=4
 */
