--TEST--
ffmpeg getFrame cropping test
--SKIPIF--
<?php 
extension_loaded('ffmpeg') or die("skip ffmpeg extension not loaded"); 
extension_loaded('gd') or die("skip gd extension not avaliable.");
function_exists("imagecreatetruecolor") or die("skip function imagecreatetruecolor unavailable");
?>
--FILE--
<?php

function print_image_md5($gd_image) {
    if ($gd_image) {
        $img = sprintf("%s/tmp.png", dirname(__FILE__), $frame);
        imagepng($gd_image, $img);
        // generate md5 of file
        printf("%s\n", md5(file_get_contents($img)));
//        unlink($img);
    } else {
        printf("failed\n");
    }
}

$frame = 73;
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
$img = sprintf("%s/test-%04d.png", dirname(__FILE__), $frame);

$gd_image = $mov->getFrameResampled(190, 240, $frame, 10, 10, 10, 10);
print('ffmpeg getFrame(): md5 = ');
print_image_md5($gd_image);
imagedestroy($gd_image);
$frame++;
$gd_image = $mov->getFrame($frame, 10, 10, 10, 10);
print('ffmpeg getFrame(): md5 = ');
print_image_md5($gd_image);
imagedestroy($gd_image);
?>
--EXPECT--
ffmpeg getFrame(): md5 = 811bf64a01eb3f919f3e8cbb45e7f07a
ffmpeg getFrame(): md5 = d3973f39a33e89e53b03d9707c392cb2
