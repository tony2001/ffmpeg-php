--TEST--
ffmpeg get_framerate test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded\n"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf("ffmpeg get_framerate(): %0.2f\n", $mov->get_framerate());
?>
--EXPECT--
ffmpeg get_framerate(): 24.00
