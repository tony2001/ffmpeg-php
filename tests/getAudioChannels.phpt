--TEST--
ffmpeg getAudioChannels test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/robot.avi');
printf("ffmpeg getAudioChannels(): %s\n", $mov->getAudioChannels());
?>
--EXPECT--
ffmpeg getAudioChannels(): 1
