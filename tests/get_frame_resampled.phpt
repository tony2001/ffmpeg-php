--TEST--
ffmpeg get_frame_resampled test
--SKIPIF--
<?php 
extension_loaded('ffmpeg') or die("skip ffmpeg extension not loaded"); 
extension_loaded('gd') or die("skip gd extension not avaliable.");
function_exists("imagecreatetruecolor") or die("skip function imagecreatetruecolor unavailable");
?>
--FILE--
<?php
$frame = 70;
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
$img = sprintf("%s/test-%04d.png", dirname(__FILE__), $frame);

$gd_image = $mov->get_frame_resampled(96, 120, $frame);
if ($gd_image) {
    imagepng($gd_image, $img);
    imagedestroy($gd_image);
    // generate md5 of file (NOTE: different versions of ffmpeg may produce different
    // md5 hashes since resampling has been changed slightly due to a fix. Need to
    // use EXPECTREX to test for both md5 possibilities.
    printf("ffmpeg get_frame_resampled(): md5 = %s\n", md5(file_get_contents($img)));
    unlink($img);
}
?>
--EXPECT--
ffmpeg get_frame_resampled(): md5 = 135b7ff2a0e5d15dc9c111c94e74c9a4 
