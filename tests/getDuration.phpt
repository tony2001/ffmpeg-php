--TEST--
ffmpeg getDuration test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded\n"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf("ffmpeg getDuration(): %0.2f\n", $mov->getDuration());
?>
--EXPECT--
ffmpeg getDuration(): 5.00 
