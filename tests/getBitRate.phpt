--TEST--
ffmpeg getBitRate test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf("ffmpeg getBitRate(): %s\n", $mov->getBitRate());
?>
--EXPECT--
ffmpeg getBitRate(): 306
