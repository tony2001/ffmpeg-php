--TEST--
ffmpeg getFrameIntoImage test
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

$gd_img = imagecreatetruecolor(100, 100);
$mov->getFrameIntoImage($gd_img, $frame);

if ($gd_img) {
    imagepng($gd_img, $img);
    imagedestroy($gd_img);
    // generate md5 of file (NOTE: different versions of ffmpeg may produce different
    // md5 hashes since resampling has been changed slightly due to a fix. Need to
    // use EXPECTREX to test for both md5 possibilities.
    printf("ffmpeg getFrameIntoImage(): md5 = %s\n", md5(file_get_contents($img)));
//    unlink($img);
}
?>
--EXPECT--
ffmpeg getFrameIntoImage(): md5 = e78489040201affdf0b93def9188e6a5 
