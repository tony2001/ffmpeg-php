--TEST--
ffmpeg getFrameNumber test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded\n"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/robot.avi');

/* move frame point to frame 5 */
$mov->getFrame(5);
printf("ffmpeg getFrameNumber(): %d\n", $mov->getFrameNumber());
?>
--EXPECT--
ffmpeg getFrameNumber(): 5
