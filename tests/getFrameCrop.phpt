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
        $img = sprintf("tmp.png", dirname(__FILE__));
        imagepng($gd_image, $img);
        // generate md5 of file
        printf("%s\n", md5(file_get_contents($img)));
        unlink($img);
    } else {
        printf("failed\n");
    }
}

$framenumber = 73;
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
$img = sprintf("%s/test-%04d.png", dirname(__FILE__), $framenumber);

/* cropping as part of resize */
$frame = $mov->getFrame($framenumber);
$frame->resize(190, 240, 10, 10, 10, 10);
$gd_image = $frame->toGDImage();
print('ffmpeg resize and crop: md5 = ');
print_image_md5($gd_image);
imagedestroy($gd_image);
$framenumber++;

/* cropping without resize */
$frame = $mov->getFrame($framenumber);
$frame->crop(10, 10, 10, 10);
$gd_image = $frame->toGDImage();
print('ffmpeg crop: md5 = ');
print_image_md5($gd_image);
imagedestroy($gd_image);
?>
--EXPECT--
ffmpeg resize and crop: md5 = 811bf64a01eb3f919f3e8cbb45e7f07a
ffmpeg crop: md5 = d3973f39a33e89e53b03d9707c392cb2
