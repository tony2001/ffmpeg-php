--TEST--
ffmpeg get_framenumber test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded\n"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');

/* move frame point to frame 5 */
$gd_image = $mov->getFrame(5);
imagedestroy($gd_image);

printf("ffmpeg get_framenumber(): %d\n", $mov->get_framenumber());
?>
--EXPECT--
ffmpeg get_framenumber(): 5
