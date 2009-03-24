--TEST--
ffmpeg getFrameCount test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded\n"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/robot.avi');
printf("ffmpeg getFrameCount(): %d\n", $mov->getFrameCount());
?>
--EXPECT--
ffmpeg getFrameCount(): 240
