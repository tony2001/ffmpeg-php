--TEST--
ffmpeg getFrameNumber test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded\n"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');

/* move frame point to frame 6 */
$gd_image = $mov->getFrame(5);
imagedestroy($gd_image);

printf("ffmpeg getFrameNumber(): %d\n", $mov->getFrameNumber());
?>
--EXPECT--
ffmpeg getFrameNumber(): 6
