--TEST--
ffmpeg get_video_bitrate test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf("ffmpeg get_video_bitrate(): %s\n", $mov->get_video_bitrate());
?>
--EXPECT--
ffmpeg get_video_bitrate(): 306
