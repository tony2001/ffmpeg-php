--TEST--
ffmpeg getFrameHeight test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded\n"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf("ffmpeg getFrameHeight(): %d\n", $mov->getFrameHeight());
?>
--EXPECT--
ffmpeg getFrameHeight(): 240
