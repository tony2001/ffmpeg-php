--TEST--
ffmpeg getPixelAspectRatio test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf('frame number = ' . $mov->getFrameNumber() . "\n");
printf("ffmpeg getPixelAspectRatio(): %s\n", $mov->getPixelAspectRatio());
printf( 'frame number = ' . $mov->getFrameNumber() . "\n");

?>
--EXPECT--
frame number = 1
ffmpeg getPixelAspectRatio(): 1
frame number = 1
