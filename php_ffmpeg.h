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

#ifndef PHP_FFMPEG_H
#define PHP_FFMPEG_H

#define SAFE_STRING(s) ((s)?(s):"")

#ifndef safe_emalloc
    # define safe_emalloc(a,b,c) emalloc((a)*(b)+(c))
#endif


extern zend_module_entry ffmpeg_module_entry;
#define phpext_ffmpeg_ptr &ffmpeg_module_entry

#ifdef PHP_WIN32
#define PHP_FFMPEG_API __declspec(dllexport)
#else
#define PHP_FFMPEG_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(ffmpeg);
PHP_MSHUTDOWN_FUNCTION(ffmpeg);
PHP_RINIT_FUNCTION(ffmpeg);
PHP_RSHUTDOWN_FUNCTION(ffmpeg);
PHP_MINFO_FUNCTION(ffmpeg);


#ifdef ZTS
#define FFMPEG_G(v) TSRMG(ffmpeg_globals_id, zend_ffmpeg_globals *, v)
#else
#define FFMPEG_G(v) (ffmpeg_globals.v)
#endif

#endif	/* PHP_FFMPEG_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
