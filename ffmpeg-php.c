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

#include <avcodec.h>
#include <avformat.h>

#if HAVE_SWSCALER
#include <swscale.h>
#endif

#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"

#include "php_ffmpeg.h"

#define FFMPEG_PHP_VERSION "0.6.0-svn"

zend_module_entry ffmpeg_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "ffmpeg",
    NULL,
    PHP_MINIT(ffmpeg),
    PHP_MSHUTDOWN(ffmpeg),
    NULL,
    NULL,
    PHP_MINFO(ffmpeg),
#if ZEND_MODULE_API_NO >= 20010901
    FFMPEG_PHP_VERSION, 
#endif
    STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_FFMPEG
ZEND_GET_MODULE(ffmpeg);
#endif

extern void register_ffmpeg_movie_class(int);
extern void register_ffmpeg_frame_class(int);
extern void ffmpeg_errorhandler(void *ptr, int level, const char *msg, va_list args);

PHP_INI_BEGIN()
    PHP_INI_ENTRY("ffmpeg.allow_persistent", "0", PHP_INI_ALL, NULL)
    PHP_INI_ENTRY("ffmpeg.show_warnings", "0", PHP_INI_ALL, NULL)
PHP_INI_END()


/* {{{ php module init function
 */
PHP_MINIT_FUNCTION(ffmpeg)
{
    /* must be called before using avcodec libraries. */ 
    avcodec_init();

    /* register all codecs */
    av_register_all();
    
    REGISTER_INI_ENTRIES();
 
    if (INI_BOOL("ffmpeg.show_warnings")) {
        av_log_set_callback(ffmpeg_errorhandler);
    } 
   
    register_ffmpeg_movie_class(module_number);
    register_ffmpeg_frame_class(module_number);

    REGISTER_STRING_CONSTANT("FFMPEG_PHP_VERSION_STRING", 
		    FFMPEG_PHP_VERSION, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("FFMPEG_PHP_BUILD_DATE_STRING", 
		    __DATE__ " " __TIME__, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("LIBAVCODEC_VERSION_NUMBER", 
		    avcodec_version(), CONST_CS | CONST_PERSISTENT);
#ifdef LIBAVCODEC_BUILD
    REGISTER_LONG_CONSTANT("LIBAVCODEC_BUILD_NUMBER", 
		    LIBAVCODEC_BUILD, CONST_CS | CONST_PERSISTENT);
#else
    REGISTER_LONG_CONSTANT("LIBAVCODEC_BUILD_NUMBER",
            avcodec_build(), CONST_CS | CONST_PERSISTENT);
#endif

    return SUCCESS;
}
/* }}} */


/* {{{ php module shutdown function
 */
PHP_MSHUTDOWN_FUNCTION(ffmpeg)
{
    // TODO: Free any remaining persistent movies here?
    
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}
/* }}} */


/* {{{ php info function
   Add an entry for ffmpeg support in phpinfo() */
PHP_MINFO_FUNCTION(ffmpeg)
{
    php_info_print_table_start();
//    php_info_print_table_header(2, "ffmpeg-php", "enabled");
    php_info_print_table_row(2, "ffmpeg-php version", FFMPEG_PHP_VERSION);
    php_info_print_table_row(2, "ffmpeg-php built on", __DATE__ " "  __TIME__);
#if HAVE_LIBGD20
    php_info_print_table_row(2, "ffmpeg-php gd support ", "enabled");
#else
    php_info_print_table_row(2, "ffmpeg-php gd support ", "disabled");
#endif // HAVE_LIBGD20
    php_info_print_table_row(2, "ffmpeg libavcodec version", LIBAVCODEC_IDENT);
    php_info_print_table_row(2, "ffmpeg libavformat version", LIBAVFORMAT_IDENT);
#if HAVE_SWSCALER
    php_info_print_table_row(2, "ffmpeg swscaler version", LIBSWSCALE_IDENT);
#else 
    php_info_print_table_row(2, "ffmpeg swscaler", "disabled");
#endif
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
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
