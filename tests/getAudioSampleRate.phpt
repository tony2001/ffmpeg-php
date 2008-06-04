--TEST--
ffmpeg getAudioSampleRate test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/robot.avi');
printf("ffmpeg getAudioSampleRate(): %s\n", $mov->getAudioSampleRate());
?>
--EXPECT--
ffmpeg getAudioSampleRate(): 32000
