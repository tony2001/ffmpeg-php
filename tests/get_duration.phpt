--TEST--
ffmpeg get_duration test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded\n"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
printf("ffmpeg get_duration(): %0.2f\n", $mov->get_duration());
?>
--EXPECT--
ffmpeg get_duration(): 5.00 
