--TEST--
ffmpeg getBitrate test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf("ffmpeg getBitrate(): %s\n", $mov->getBitrate());
?>
--EXPECT--
ffmpeg getBitrate(): 306
