--TEST--
ffmpeg getFrame test
--SKIPIF--
<?php 
extension_loaded('ffmpeg') or die("skip ffmpeg extension not loaded"); 
extension_loaded('gd') or die("skip gd extension not avaliable.");
function_exists("imagecreatetruecolor") or die("skip function imagecreatetruecolor unavailable");
?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test.avi');
$img = dirname(__FILE__) . '/test.png';

$gd_image = $mov->getFrame(73);

imagepng($gd_image, $img);
// generate md5 of file
printf("ffmpeg getFrame(): md5 = %s\n", md5(file_get_contents($img)));
unlink($img);
?>
--EXPECT--
ffmpeg getFrame(): md5 = 2b04a8f57be7654f8acfbbedcce9f8bd
