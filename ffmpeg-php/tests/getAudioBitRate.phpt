--TEST--
ffmpeg getAudioBitRate test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/robot.avi');
printf("ffmpeg getAudioBitRate(): %s\n", $mov->getAudioBitRate());
?>
--EXPECT--
ffmpeg getAudioBitRate(): 32000
