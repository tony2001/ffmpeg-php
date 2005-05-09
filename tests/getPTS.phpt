--TEST--
ffmpeg getFrames forward test
--SKIPIF--
<?php 
extension_loaded('ffmpeg') or die("skip ffmpeg extension not loaded"); 
extension_loaded('gd') or die("skip gd extension not avaliable.");
function_exists("imagecreatetruecolor") or die("skip function imagecreatetruecolor unavailable");
?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');

$framecount = $mov->getFrameCount();
for($i = 1; $i <= $framecount; $i++) {
    $frame = $mov->getFrame($i);
    printf("ffmpeg getPresentationTimestamp($i): = %s\n", $frame->getPTS());
}
?>
--EXPECT--
ffmpeg getPresentationTimestamp(1): = 0
ffmpeg getPresentationTimestamp(2): = 41667
ffmpeg getPresentationTimestamp(3): = 83333
ffmpeg getPresentationTimestamp(4): = 125000
ffmpeg getPresentationTimestamp(5): = 166667
ffmpeg getPresentationTimestamp(6): = 208333
ffmpeg getPresentationTimestamp(7): = 250000
ffmpeg getPresentationTimestamp(8): = 291667
ffmpeg getPresentationTimestamp(9): = 333333
ffmpeg getPresentationTimestamp(10): = 375000
ffmpeg getPresentationTimestamp(11): = 416667
ffmpeg getPresentationTimestamp(12): = 458333
ffmpeg getPresentationTimestamp(13): = 500000
ffmpeg getPresentationTimestamp(14): = 541667
ffmpeg getPresentationTimestamp(15): = 583333
ffmpeg getPresentationTimestamp(16): = 625000
ffmpeg getPresentationTimestamp(17): = 666667
ffmpeg getPresentationTimestamp(18): = 708333
ffmpeg getPresentationTimestamp(19): = 750000
ffmpeg getPresentationTimestamp(20): = 791667
ffmpeg getPresentationTimestamp(21): = 833333
ffmpeg getPresentationTimestamp(22): = 875000
ffmpeg getPresentationTimestamp(23): = 916667
ffmpeg getPresentationTimestamp(24): = 958333
ffmpeg getPresentationTimestamp(25): = 1000000
ffmpeg getPresentationTimestamp(26): = 1041667
ffmpeg getPresentationTimestamp(27): = 1083333
ffmpeg getPresentationTimestamp(28): = 1125000
ffmpeg getPresentationTimestamp(29): = 1166667
ffmpeg getPresentationTimestamp(30): = 1208333
ffmpeg getPresentationTimestamp(31): = 1250000
ffmpeg getPresentationTimestamp(32): = 1291667
ffmpeg getPresentationTimestamp(33): = 1333333
ffmpeg getPresentationTimestamp(34): = 1375000
ffmpeg getPresentationTimestamp(35): = 1416667
ffmpeg getPresentationTimestamp(36): = 1458333
ffmpeg getPresentationTimestamp(37): = 1500000
ffmpeg getPresentationTimestamp(38): = 1541667
ffmpeg getPresentationTimestamp(39): = 1583333
ffmpeg getPresentationTimestamp(40): = 1625000
ffmpeg getPresentationTimestamp(41): = 1666667
ffmpeg getPresentationTimestamp(42): = 1708333
ffmpeg getPresentationTimestamp(43): = 1750000
ffmpeg getPresentationTimestamp(44): = 1791667
ffmpeg getPresentationTimestamp(45): = 1833333
ffmpeg getPresentationTimestamp(46): = 1875000
ffmpeg getPresentationTimestamp(47): = 1916667
ffmpeg getPresentationTimestamp(48): = 1958333
ffmpeg getPresentationTimestamp(49): = 2000000
ffmpeg getPresentationTimestamp(50): = 2041667
ffmpeg getPresentationTimestamp(51): = 2083333
ffmpeg getPresentationTimestamp(52): = 2125000
ffmpeg getPresentationTimestamp(53): = 2166667
ffmpeg getPresentationTimestamp(54): = 2208333
ffmpeg getPresentationTimestamp(55): = 2250000
ffmpeg getPresentationTimestamp(56): = 2291667
ffmpeg getPresentationTimestamp(57): = 2333333
ffmpeg getPresentationTimestamp(58): = 2375000
ffmpeg getPresentationTimestamp(59): = 2416667
ffmpeg getPresentationTimestamp(60): = 2458333
ffmpeg getPresentationTimestamp(61): = 2500000
ffmpeg getPresentationTimestamp(62): = 2541667
ffmpeg getPresentationTimestamp(63): = 2583333
ffmpeg getPresentationTimestamp(64): = 2625000
ffmpeg getPresentationTimestamp(65): = 2666667
ffmpeg getPresentationTimestamp(66): = 2708333
ffmpeg getPresentationTimestamp(67): = 2750000
ffmpeg getPresentationTimestamp(68): = 2791667
ffmpeg getPresentationTimestamp(69): = 2833333
ffmpeg getPresentationTimestamp(70): = 2875000
ffmpeg getPresentationTimestamp(71): = 2916667
ffmpeg getPresentationTimestamp(72): = 2958333
ffmpeg getPresentationTimestamp(73): = 3000000
ffmpeg getPresentationTimestamp(74): = 3041667
ffmpeg getPresentationTimestamp(75): = 3083333
ffmpeg getPresentationTimestamp(76): = 3125000
ffmpeg getPresentationTimestamp(77): = 3166667
ffmpeg getPresentationTimestamp(78): = 3208333
ffmpeg getPresentationTimestamp(79): = 3250000
ffmpeg getPresentationTimestamp(80): = 3291667
ffmpeg getPresentationTimestamp(81): = 3333333
ffmpeg getPresentationTimestamp(82): = 3375000
ffmpeg getPresentationTimestamp(83): = 3416667
ffmpeg getPresentationTimestamp(84): = 3458333
ffmpeg getPresentationTimestamp(85): = 3500000
ffmpeg getPresentationTimestamp(86): = 3541667
ffmpeg getPresentationTimestamp(87): = 3583333
ffmpeg getPresentationTimestamp(88): = 3625000
ffmpeg getPresentationTimestamp(89): = 3666667
ffmpeg getPresentationTimestamp(90): = 3708333
ffmpeg getPresentationTimestamp(91): = 3750000
ffmpeg getPresentationTimestamp(92): = 3791667
ffmpeg getPresentationTimestamp(93): = 3833333
ffmpeg getPresentationTimestamp(94): = 3875000
ffmpeg getPresentationTimestamp(95): = 3916667
ffmpeg getPresentationTimestamp(96): = 3958333
ffmpeg getPresentationTimestamp(97): = 4000000
ffmpeg getPresentationTimestamp(98): = 4041667
ffmpeg getPresentationTimestamp(99): = 4083333
ffmpeg getPresentationTimestamp(100): = 4125000
ffmpeg getPresentationTimestamp(101): = 4166667
ffmpeg getPresentationTimestamp(102): = 4208333
ffmpeg getPresentationTimestamp(103): = 4250000
ffmpeg getPresentationTimestamp(104): = 4291667
ffmpeg getPresentationTimestamp(105): = 4333333
ffmpeg getPresentationTimestamp(106): = 4375000
ffmpeg getPresentationTimestamp(107): = 4416667
ffmpeg getPresentationTimestamp(108): = 4458333
ffmpeg getPresentationTimestamp(109): = 4500000
ffmpeg getPresentationTimestamp(110): = 4541667
ffmpeg getPresentationTimestamp(111): = 4583333
ffmpeg getPresentationTimestamp(112): = 4625000
ffmpeg getPresentationTimestamp(113): = 4666667
ffmpeg getPresentationTimestamp(114): = 4708333
ffmpeg getPresentationTimestamp(115): = 4750000
ffmpeg getPresentationTimestamp(116): = 4791667
ffmpeg getPresentationTimestamp(117): = 4833333
ffmpeg getPresentationTimestamp(118): = 4875000
ffmpeg getPresentationTimestamp(119): = 4916667
ffmpeg getPresentationTimestamp(120): = 4958333
