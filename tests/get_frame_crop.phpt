--TEST--
ffmpeg get_frame test
--SKIPIF--
<?php 
extension_loaded('ffmpeg') or die("skip ffmpeg extension not loaded"); 
extension_loaded('gd') or die("skip gd extension not avaliable.");
function_exists("imagecreatetruecolor") or die("skip function imagecreatetruecolor unavailable");
?>
--FILE--
<?php
$frame = 73; 
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
$img = sprintf("%s/test-%04d.png", dirname(__FILE__), $frame);

$gd_image = $mov->get_frame_resampled(190, 200, $frame, 40, 0, 0, 0);
if ($gd_image) {
    imagepng($gd_image, $img);
    imagedestroy($gd_image);
    // generate md5 of file
    printf("ffmpeg get_frame(): md5 = %s\n", md5(file_get_contents($img)));
//    unlink($img);
} else {
    printf("failed\n");
}
?>
--EXPECT--
ffmpeg get_frame(): md5 = cc36d30fb5b269146802e740ce4ab239
