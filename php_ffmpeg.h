#ifndef PHP_FFMPEG_H
#define PHP_FFMPEG_H

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

PHP_FUNCTION(ffmpeg_movie);

PHP_FUNCTION(getDuration);
PHP_FUNCTION(getFrameCount);
PHP_FUNCTION(getFrameRate);
PHP_FUNCTION(getFileName);
PHP_FUNCTION(getComment);
PHP_FUNCTION(getTitle);
PHP_FUNCTION(getAuthor);
PHP_FUNCTION(getCopyright);
PHP_FUNCTION(getFrameWidth);
PHP_FUNCTION(getFrameHeight);
PHP_FUNCTION(getFrameNumber);
PHP_FUNCTION(getPixelFormat);

#if HAVE_LIBGD20
PHP_FUNCTION(getFrame);
#endif /* HAVE_LIBGD20 */

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
