--TEST--
ffmpeg hasAudio test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded\n"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf("ffmpeg hasAudio(): %s\n", $mov->hasAudio() ? 'Yes' : 'No');
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/robot.avi');
printf("ffmpeg hasAudio(): %s\n", $mov->hasAudio() ? 'Yes' : 'No');
?>
--EXPECT--
ffmpeg hasAudio(): No
ffmpeg hasAudio(): Yes
