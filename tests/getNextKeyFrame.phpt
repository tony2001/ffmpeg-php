--TEST--
ffmpeg getFrame without an argument test
--SKIPIF--
<?php 
extension_loaded('ffmpeg') or die("skip ffmpeg extension not loaded"); 
extension_loaded('gd') or die("skip gd extension not avaliable.");
function_exists("imagecreatetruecolor") or die("skip function imagecreatetruecolor unavailable");
?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');
$img = dirname(__FILE__) . '/test-';

while (($frame = $mov->getNextKeyFrame()) != false) {
    $i = $mov->getFrameNumber();
    $filename = $img . $i . '.png';
    $gd_image = $frame->toGDImage();
    imagepng($gd_image, $filename);
    printf("ffmpeg getFrame($i): md5 = %s\n", md5(file_get_contents($filename)));
    imagedestroy($gd_image);
    unlink($filename);
}
?>
--EXPECT--
ffmpeg getFrame(1): md5 = bc5ed540e864bd5352cf526726663844
ffmpeg getFrame(13): md5 = 8527383d8556260d581e7ccd09a478a7
ffmpeg getFrame(25): md5 = 1ac1036ac0bc153f9cd1955a5b7a733a
ffmpeg getFrame(37): md5 = 893d3c696720b727445723f706337b53
ffmpeg getFrame(49): md5 = 987c6513ae5ec608be5d335841bd1c66
ffmpeg getFrame(61): md5 = 291a876d42773db5ad25bbd0aba0fb95
ffmpeg getFrame(73): md5 = cc36d30fb5b269146802e740ce4ab239
ffmpeg getFrame(85): md5 = af1e8feb72654a9ea62909ef9232f1df
ffmpeg getFrame(97): md5 = cf89e3c19741ac437d6c4875a623223d
ffmpeg getFrame(109): md5 = 64f998f1d29748443869ba1991642ca6
