--TEST--
ffmpeg get_pixelformat test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf("ffmpeg get_pixelformat(): %s\n", $mov->get_pixelformat());
?>
--EXPECT--
ffmpeg get_pixelformat(): yuv420p
