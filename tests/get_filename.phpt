--TEST--
ffmpeg get_filename test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf("ffmpeg get_filename(): %s\n", $mov->get_filename());
?>
--EXPECTREGEX--
ffmpeg get_filename(): .*/test.avi
