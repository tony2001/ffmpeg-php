--TEST--
ffmpeg has_audio test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded\n"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf("ffmpeg has_audio(): %s\n", $mov->has_audio() ? 'Yes' : 'No');
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/robot.avi');
printf("ffmpeg has_audio(): %s\n", $mov->has_audio() ? 'Yes' : 'No');
?>
--EXPECT--
ffmpeg has_audio(): No
ffmpeg has_audio(): Yes
