--TEST--
ffmpeg getFileName test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test.avi');
printf("ffmpeg getFileName(): %s\n", $mov->getFileName());
?>
--EXPECTREGEX--
ffmpeg getFileName(): .*/test.avi
