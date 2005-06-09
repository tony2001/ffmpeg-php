--TEST--
ffmpeg getAudioCodec test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/robot.avi');
printf("ffmpeg getAudioCodec(): %s\n", $mov->getAudioCodec());
?>
--EXPECT--
ffmpeg getAudioCodec(): mp2
