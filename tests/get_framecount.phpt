--TEST--
ffmpeg get_framecount test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded\n"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf("ffmpeg get_framecount(): %0.2f\n", $mov->get_framecount());
?>
--EXPECT--
ffmpeg get_framecount(): 120.00
