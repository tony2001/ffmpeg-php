
PHP_ARG_WITH(ffmpeg,for ffmpeg support, 
[  --with-ffmpeg[=DIR]        Include ffmpeg support (requires ffmpeg >= 0.49.0).])

dnl Determine path to ffmpeg libs
if test "$PHP_FFMPEG" != "no"; then
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
    AC_MSG_ERROR(Cannot find ffmpeg)
  fi

  FFMPEG_LIBDIR=$FFMPEG_DIR/lib

  PHP_ADD_LIBRARY_WITH_PATH(avcodec, $FFMPEG_LIBDIR, FFMPEG_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(avformat, $FFMPEG_LIBDIR, FFMPEG_SHARED_LIBADD)
  PHP_ADD_INCLUDE($FFMPEG_INCDIR)

  dnl Make library paths to libavformat/libavcodec absolute.
  PHP_RPATHS=$FFMPEG_LIBDIR
  
  PHP_NEW_EXTENSION(ffmpeg, ffmpeg.c, $ext_shared)
  PHP_SUBST(FFMPEG_SHARED_LIBADD)

dnl PHP_DEBUG_MACRO(test.dbg)
fi
