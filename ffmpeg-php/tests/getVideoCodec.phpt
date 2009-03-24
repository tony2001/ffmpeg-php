--TEST--
ffmpeg getVideoCodec test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/robot.avi');
printf("ffmpeg getVideoCodec(): %s\n", $mov->getVideoCodec());
?>
--EXPECT--
ffmpeg getVideoCodec(): mpeg1video
