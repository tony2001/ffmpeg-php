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
$i = 0;

while (($frame = $mov->getFrame()) != false) {
    $i = $mov->getFrameNumber();
    $filename = $img . $i . '.png';
    $gd_image = $frame->toGDImage();
    imagepng($gd_image, $filename);
    printf("ffmpeg frame->isKeyFrame($i)= %d\n", $frame->isKeyFrame());
    imagedestroy($gd_image);
    unlink($filename);
}
?>
--EXPECT--
ffmpeg frame->isKeyFrame(1)= 1
ffmpeg frame->isKeyFrame(2)= 0
ffmpeg frame->isKeyFrame(3)= 0
ffmpeg frame->isKeyFrame(4)= 0
ffmpeg frame->isKeyFrame(5)= 0
ffmpeg frame->isKeyFrame(6)= 0
ffmpeg frame->isKeyFrame(7)= 0
ffmpeg frame->isKeyFrame(8)= 0
ffmpeg frame->isKeyFrame(9)= 0
ffmpeg frame->isKeyFrame(10)= 0
ffmpeg frame->isKeyFrame(11)= 0
ffmpeg frame->isKeyFrame(12)= 0
ffmpeg frame->isKeyFrame(13)= 1
ffmpeg frame->isKeyFrame(14)= 0
ffmpeg frame->isKeyFrame(15)= 0
ffmpeg frame->isKeyFrame(16)= 0
ffmpeg frame->isKeyFrame(17)= 0
ffmpeg frame->isKeyFrame(18)= 0
ffmpeg frame->isKeyFrame(19)= 0
ffmpeg frame->isKeyFrame(20)= 0
ffmpeg frame->isKeyFrame(21)= 0
ffmpeg frame->isKeyFrame(22)= 0
ffmpeg frame->isKeyFrame(23)= 0
ffmpeg frame->isKeyFrame(24)= 0
ffmpeg frame->isKeyFrame(25)= 1
ffmpeg frame->isKeyFrame(26)= 0
ffmpeg frame->isKeyFrame(27)= 0
ffmpeg frame->isKeyFrame(28)= 0
ffmpeg frame->isKeyFrame(29)= 0
ffmpeg frame->isKeyFrame(30)= 0
ffmpeg frame->isKeyFrame(31)= 0
ffmpeg frame->isKeyFrame(32)= 0
ffmpeg frame->isKeyFrame(33)= 0
ffmpeg frame->isKeyFrame(34)= 0
ffmpeg frame->isKeyFrame(35)= 0
ffmpeg frame->isKeyFrame(36)= 0
ffmpeg frame->isKeyFrame(37)= 1
ffmpeg frame->isKeyFrame(38)= 0
ffmpeg frame->isKeyFrame(39)= 0
ffmpeg frame->isKeyFrame(40)= 0
ffmpeg frame->isKeyFrame(41)= 0
ffmpeg frame->isKeyFrame(42)= 0
ffmpeg frame->isKeyFrame(43)= 0
ffmpeg frame->isKeyFrame(44)= 0
ffmpeg frame->isKeyFrame(45)= 0
ffmpeg frame->isKeyFrame(46)= 0
ffmpeg frame->isKeyFrame(47)= 0
ffmpeg frame->isKeyFrame(48)= 0
ffmpeg frame->isKeyFrame(49)= 1
ffmpeg frame->isKeyFrame(50)= 0
ffmpeg frame->isKeyFrame(51)= 0
ffmpeg frame->isKeyFrame(52)= 0
ffmpeg frame->isKeyFrame(53)= 0
ffmpeg frame->isKeyFrame(54)= 0
ffmpeg frame->isKeyFrame(55)= 0
ffmpeg frame->isKeyFrame(56)= 0
ffmpeg frame->isKeyFrame(57)= 0
ffmpeg frame->isKeyFrame(58)= 0
ffmpeg frame->isKeyFrame(59)= 0
ffmpeg frame->isKeyFrame(60)= 0
ffmpeg frame->isKeyFrame(61)= 1
ffmpeg frame->isKeyFrame(62)= 0
ffmpeg frame->isKeyFrame(63)= 0
ffmpeg frame->isKeyFrame(64)= 0
ffmpeg frame->isKeyFrame(65)= 0
ffmpeg frame->isKeyFrame(66)= 0
ffmpeg frame->isKeyFrame(67)= 0
ffmpeg frame->isKeyFrame(68)= 0
ffmpeg frame->isKeyFrame(69)= 0
ffmpeg frame->isKeyFrame(70)= 0
ffmpeg frame->isKeyFrame(71)= 0
ffmpeg frame->isKeyFrame(72)= 0
ffmpeg frame->isKeyFrame(73)= 1
ffmpeg frame->isKeyFrame(74)= 0
ffmpeg frame->isKeyFrame(75)= 0
ffmpeg frame->isKeyFrame(76)= 0
ffmpeg frame->isKeyFrame(77)= 0
ffmpeg frame->isKeyFrame(78)= 0
ffmpeg frame->isKeyFrame(79)= 0
ffmpeg frame->isKeyFrame(80)= 0
ffmpeg frame->isKeyFrame(81)= 0
ffmpeg frame->isKeyFrame(82)= 0
ffmpeg frame->isKeyFrame(83)= 0
ffmpeg frame->isKeyFrame(84)= 0
ffmpeg frame->isKeyFrame(85)= 1
ffmpeg frame->isKeyFrame(86)= 0
ffmpeg frame->isKeyFrame(87)= 0
ffmpeg frame->isKeyFrame(88)= 0
ffmpeg frame->isKeyFrame(89)= 0
ffmpeg frame->isKeyFrame(90)= 0
ffmpeg frame->isKeyFrame(91)= 0
ffmpeg frame->isKeyFrame(92)= 0
ffmpeg frame->isKeyFrame(93)= 0
ffmpeg frame->isKeyFrame(94)= 0
ffmpeg frame->isKeyFrame(95)= 0
ffmpeg frame->isKeyFrame(96)= 0
ffmpeg frame->isKeyFrame(97)= 1
ffmpeg frame->isKeyFrame(98)= 0
ffmpeg frame->isKeyFrame(99)= 0
ffmpeg frame->isKeyFrame(100)= 0
ffmpeg frame->isKeyFrame(101)= 0
ffmpeg frame->isKeyFrame(102)= 0
ffmpeg frame->isKeyFrame(103)= 0
ffmpeg frame->isKeyFrame(104)= 0
ffmpeg frame->isKeyFrame(105)= 0
ffmpeg frame->isKeyFrame(106)= 0
ffmpeg frame->isKeyFrame(107)= 0
ffmpeg frame->isKeyFrame(108)= 0
ffmpeg frame->isKeyFrame(109)= 1
ffmpeg frame->isKeyFrame(110)= 0
ffmpeg frame->isKeyFrame(111)= 0
ffmpeg frame->isKeyFrame(112)= 0
ffmpeg frame->isKeyFrame(113)= 0
ffmpeg frame->isKeyFrame(114)= 0
ffmpeg frame->isKeyFrame(115)= 0
ffmpeg frame->isKeyFrame(116)= 0
ffmpeg frame->isKeyFrame(117)= 0
ffmpeg frame->isKeyFrame(118)= 0
ffmpeg frame->isKeyFrame(119)= 0
ffmpeg frame->isKeyFrame(120)= 0
