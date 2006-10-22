--TEST--
ffmpeg persistent movie test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi', 1);
$mov->getFrame(10);
printf("ffmpeg getFrameNumber(): %d\n", $mov->getFrameNumber());
$mov2 = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi', 1);
printf("ffmpeg getFrameNumber(): %d\n", $mov2->getFrameNumber());
?>
--EXPECTREGEX--
ffmpeg getFrameNumber(): 10
ffmpeg getFrameNumber(): 10
