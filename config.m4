

PHP_ARG_ENABLE(ffmpeg-rpath, hard code full path to ffmpeg libs,
[  --enable-ffmpeg-rpath, hardcode library paths, no,no   ])


PHP_ARG_WITH(ffmpeg,for ffmpeg support, 
[  --with-ffmpeg[=DIR]       Include ffmpeg support (requires ffmpeg >= 0.49.0).])

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
    AC_MSG_RESULT(Cannot find ffmpeg)
  else
    dnl For debugging
    AC_MSG_RESULT(...ffmpeg libs found in $FFMPEG_DIR)
  fi

  FFMPEG_LIBDIR=$FFMPEG_DIR/lib

  PHP_ADD_LIBRARY_WITH_PATH(avcodec, $FFMPEG_LIBDIR, FFMPEG_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(avformat, $FFMPEG_LIBDIR, FFMPEG_SHARED_LIBADD)
  PHP_ADD_INCLUDE($FFMPEG_INCDIR)

  dnl Make path to ffmpeg libs absolute if the user specified a path.
  dnl This allows to specify a lib path outside the default lib dirs.
  if test -z "$PHP_FFMPEG_RPATH"; then
    PHP_RPATHS=$FFMPEG_LIBDIR
    AC_MSG_RESULT(...adding rpath for $FFMPEG_DIR to ffmpeg extension)
  fi
  
  PHP_NEW_EXTENSION(ffmpeg, ffmpeg.c, $ext_shared)
  PHP_SUBST(FFMPEG_SHARED_LIBADD)

dnl PHP_DEBUG_MACRO(test.dbg)
fi
