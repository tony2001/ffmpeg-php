--TEST--
ffmpeg persistent movie test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi', 1);
$mov2 = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi', 1);
printf("ffmpeg getDuration(): %0.2f\n", $mov->getDuration());
printf("ffmpeg getDuration(): %0.2f\n", $mov2->getDuration());
?>
--EXPECTREGEX--
ffmpeg getDuration(): 5.00
ffmpeg getDuration(): 5.00
