--TEST--
ffmpeg getPixelFormat test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test.avi');
printf("ffmpeg getPixelFormat(): %s\n", $mov->getPixelFormat());
?>
--EXPECTREGEX--
ffmpeg getPixelFormat(): yuv420p
