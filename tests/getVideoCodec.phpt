--TEST--
ffmpeg getVideoCodec test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf("ffmpeg getVideoCodec(): %s\n", $mov->getVideoCodec());
?>
--EXPECTREGEX--
ffmpeg getVideoCodec\(\): mpeg4 
