--TEST--
ffmpeg getVideoBitRate test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/robot.avi');
printf("ffmpeg getVideoBitRate(): %s\n", $mov->getVideoBitRate());
?>
--EXPECT--
ffmpeg getVideoBitRate(): 104857200
