--TEST--
ffmpeg animatedGif
--SKIPIF--
<?php 
extension_loaded('ffmpeg') or die("skip ffmpeg extension not loaded"); 
extension_loaded('gd') or die("skip gd extension not avaliable.");
function_exists("imagecreatetruecolor") or die("skip function imagecreatetruecolor unavailable");
?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
$agif = sprintf("%s/test.gif", dirname(__FILE__));

$ff_gif = new ffmpeg_animated_gif($agif, 120, 96, 5, 5);

for ($i = 1; $i < 70; $i+=10) {
    $ff_frame = $mov->getFrame($i);
    $ff_gif->addFrame($ff_frame);
}

$img = imagecreatetruecolor(100,100);
$red = imagecolorallocate($img, 255, 0, 0);
$white = imagecolorallocate($img, 255, 255, 255);

imagerectangle($img, 5, 5, 90, 90, $white );

$ff_frame_img = new ffmpeg_frame($img);

$ff_gif->addFrame($ff_frame_img);

printf("ffmpeg animated gif: md5 = %s\n", md5(file_get_contents($agif)));
unlink($agif);

?>
--EXPECT--
ffmpeg animated gif: md5 = 3fecf3e06bf63e21b1704554da450c93
