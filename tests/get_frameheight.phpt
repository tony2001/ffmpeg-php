--TEST--
ffmpeg get_frameheight test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded\n"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf("ffmpeg get_frameheight(): %d\n", $mov->get_frameheight());
?>
--EXPECT--
ffmpeg get_frameheight(): 240
