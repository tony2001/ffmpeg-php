--TEST--
ffmpeg getAudioStreamId test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/robot.avi');
printf("ffmpeg getAudioStreamId(): %s\n", $mov->getAudioStreamId());
?>
--EXPECT--
ffmpeg getAudioStreamId(): 0
