PHP_ARG_WITH(ffmpeg,for ffmpeg support, 
[  --with-ffmpeg[=DIR]       Include ffmpeg support (requires ffmpeg >= 0.49.0).])

dnl Determine path to ffmpeg libs
if test "$PHP_FFMPEG" != "no"; then

  dnl check php version and bail if it's 5 
  dnl TODO: This should be a function
  php_vers=`$PHP_CONFIG --version 2>/dev/null|head -n 1|sed -e 's/^[^0-9]*//' -e 's/[a-z]* *$//'`
  
  if test -z "$php_vers"; then
    echo "Error: php-config not found."
    exit 1
  fi
  IFS=.; set $php_vers; IFS=' '
  if test "$1" = "5"; then
    echo "*** ERROR: ffmpeg-php doesn't work with php version $php_vers yet. ***"
    exit 1
  fi

  AC_MSG_CHECKING(for ffmpeg libraries)
  for i in $PHP_FFMPEG /usr/local /usr ; do
    if test -f $i/include/ffmpeg/avcodec.h; then
      FFMPEG_DIR=$i
      FFMPEG_INCDIR=$i/include/ffmpeg
    elif test -f $i/include/avcodec.h; then
      FFMPEG_DIR=$i
      FFMPEG_INCDIR=$i/include
    fi
  done

  if test -z "$FFMPEG_DIR"; then
    AC_MSG_RESULT(not found)
  else
    dnl For debugging
    AC_MSG_RESULT(...found in $FFMPEG_DIR)
  fi

  FFMPEG_LIBDIR=$FFMPEG_DIR/lib

  PHP_ADD_LIBRARY_WITH_PATH(avcodec, $FFMPEG_LIBDIR, FFMPEG_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(avformat, $FFMPEG_LIBDIR, FFMPEG_SHARED_LIBADD)
  PHP_ADD_INCLUDE($FFMPEG_INCDIR)

  PHP_NEW_EXTENSION(ffmpeg, ffmpeg.c, $ext_shared)
  PHP_SUBST(FFMPEG_SHARED_LIBADD)
  AC_DEFINE(HAVE_FFMPEG_PHP,1,[ ])
    

dnl PHP_DEBUG_MACRO(test.dbg)
fi
