PHP_ARG_WITH(quadrupel,for quadrupel support, 
[  --with-quadrupel[=DIR]       Include quadrupel support (requires quadrupel >= 0.1).])

AC_MSG_RESULT($PHP_QUADRUPEL)

dnl Determine path to quadrupel libs
if test "$PHP_QUADRUPEL" != "no"; then

  AC_MSG_CHECKING(for quadrupel headers)
  for i in $PHP_QUADRUPEL /usr/local /usr ; do
    if test -f $i/include/qp_movie.h; then
      QUADRUPEL_DIR=$i
      QUADRUPEL_INCDIR=$i/include/
      break
    fi
  done

  if test -z "$QUADRUPEL_DIR"; then
    QUADRUPEL_INCDIR=quadrupel/
    AC_MSG_RESULT(...Using bundled quadrupel)
    dnl AC_MSG_ERROR(quadrupel headers not found.)
  else
    dnl For debugging
    AC_MSG_RESULT(...found in $QUADRUPEL_INCDIR)
  fi

  AC_MSG_CHECKING(for quadrupel libquadrupel.a)
  for i in $PHP_QUADRUPEL /usr/local /usr ; do
    if test -f $i/$PHP_LIBDIR/libquadrupel.a; then
      QUADRUPEL_LIBDIR=$i/$PHP_LIBDIR
      break
    fi
  done

  if test -z "$QUADRUPEL_LIBDIR"; then
    extra_sources="quadrupel/qp_movie.c quadrupel/qp_frame.c quadrupel/qp_util.c " 
    AC_MSG_RESULT(...Using bundled quadrupel)
  else
    dnl For debugging
    AC_MSG_RESULT(...found in $QUADRUPEL_LIBDIR)
  fi
fi


PHP_ARG_WITH(ffmpeg,for ffmpeg support, 
[  --with-ffmpeg[=DIR]       Include ffmpeg support (requires ffmpeg >= 0.49.0).])


dnl Determine path to ffmpeg libs
if test "$PHP_FFMPEG" != "no"; then

  AC_MSG_CHECKING(for ffmpeg headers)
  for i in $PHP_FFMPEG /usr/local /usr ; do
    if test -f $i/include/ffmpeg/avcodec.h; then
      FFMPEG_DIR=$i
      FFMPEG_INCDIR=$i/include/ffmpeg
      break
    elif test -f $i/include/avcodec.h; then
      FFMPEG_DIR=$i
      FFMPEG_INCDIR=$i/include
      break
    fi
  done

  if test -z "$FFMPEG_DIR"; then
    AC_MSG_ERROR(ffmpeg headers not found. Make sure you've built ffmpeg as shared libs using the --enable-shared option)
  else
    dnl For debugging
    AC_MSG_RESULT(...found in $FFMPEG_INCDIR)
  fi

  AC_MSG_CHECKING(for ffmpeg libavcodec.so)
  for i in $PHP_FFMPEG /usr/local /usr ; do
    if test -f $i/lib/libavcodec.so; then
      FFMPEG_LIBDIR=$i/lib
      break
    fi
  done

  if test -z "$FFMPEG_LIBDIR"; then
    AC_MSG_ERROR(ffmpeg share libraries not found. Make sure you've built ffmpeg as shared libs using the --enable-shared option)
  else
    dnl For debugging
    AC_MSG_RESULT(...found in $FFMPEG_LIBDIR)
  fi

  dnl FFMPEG_LIBDIR=$FFMPEG_DIR/lib

  CFLAGS="$CFLAGS -g -Wall -fno-strict-aliasing"

  PHP_SUBST(FFMPEG_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(avcodec, $FFMPEG_LIBDIR, FFMPEG_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(avformat, $FFMPEG_LIBDIR, FFMPEG_SHARED_LIBADD)
  PHP_ADD_INCLUDE($FFMPEG_INCDIR)

  dnl Use bundled quadrupel for now

  if test "$QUADRUPEL_LIBDIR"; then
    PHP_ADD_LIBRARY_WITH_PATH(quadrupel, $QUADRUPEL_LIBDIR, FFMPEG_SHARED_LIBADD)
  fi
  PHP_ADD_INCLUDE($QUADRUPEL_INCDIR)
fi

  PHP_NEW_EXTENSION(ffmpeg, ffmpeg-php.c ffmpeg_movie.c ffmpeg_frame.c ffmpeg_animated_gif.c $extra_sources, $ext_shared,, \\$(GDLIB_CFLAGS))
  AC_DEFINE(HAVE_FFMPEG_PHP,1,[ ])
    

dnl PHP_DEBUG_MACRO(test.dbg)
