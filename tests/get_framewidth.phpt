--TEST--
ffmpeg get_framewidth test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded\n"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf("ffmpeg get_framewidth(): %d\n", $mov->get_framewidth());
?>
--EXPECT--
ffmpeg get_framewidth(): 190
