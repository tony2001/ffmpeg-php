#ifndef PHP_FFMPEG_H
#define PHP_FFMPEG_H

extern zend_module_entry ffmpeg_module_entry;
#define phpext_ffmpeg_ptr &ffmpeg_module_entry

#ifdef ZEND_WIN32
#define PHP_FFMPEG_API __declspec(dllexport)
#else
#define PHP_FFMPEG_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_MINIT_FUNCTION(ffmpeg);
ZEND_MSHUTDOWN_FUNCTION(ffmpeg);
ZEND_RINIT_FUNCTION(ffmpeg);
ZEND_RSHUTDOWN_FUNCTION(ffmpeg);
ZEND_MINFO_FUNCTION(ffmpeg);

ZEND_FUNCTION(ffmpeg_movie);

ZEND_FUNCTION(getDuration);
ZEND_FUNCTION(getFrameCount);
ZEND_FUNCTION(getFrameRate);
ZEND_FUNCTION(getFileName);
ZEND_FUNCTION(getComment);
ZEND_FUNCTION(getTitle);
ZEND_FUNCTION(getAuthor);
ZEND_FUNCTION(getCopyright);
//ZEND_FUNCTION(getFrame);

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
