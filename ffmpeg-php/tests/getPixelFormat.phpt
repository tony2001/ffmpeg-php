--TEST--
ffmpeg getPixelFormat test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/robot.avi');
printf("ffmpeg getPixelFormat(): %s\n", $mov->getPixelFormat());
?>
--EXPECT--
ffmpeg getPixelFormat(): yuv420p
