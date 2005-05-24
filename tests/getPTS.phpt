--TEST--
ffmpeg getPTS test
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
ffmpeg getPresentationTimestamp(2): = 0.041667
ffmpeg getPresentationTimestamp(3): = 0.083333
ffmpeg getPresentationTimestamp(4): = 0.125
ffmpeg getPresentationTimestamp(5): = 0.166667
ffmpeg getPresentationTimestamp(6): = 0.208333
ffmpeg getPresentationTimestamp(7): = 0.25
ffmpeg getPresentationTimestamp(8): = 0.291667
ffmpeg getPresentationTimestamp(9): = 0.333333
ffmpeg getPresentationTimestamp(10): = 0.375
ffmpeg getPresentationTimestamp(11): = 0.416667
ffmpeg getPresentationTimestamp(12): = 0.458333
ffmpeg getPresentationTimestamp(13): = 0.5
ffmpeg getPresentationTimestamp(14): = 0.541667
ffmpeg getPresentationTimestamp(15): = 0.583333
ffmpeg getPresentationTimestamp(16): = 0.625
ffmpeg getPresentationTimestamp(17): = 0.666667
ffmpeg getPresentationTimestamp(18): = 0.708333
ffmpeg getPresentationTimestamp(19): = 0.75
ffmpeg getPresentationTimestamp(20): = 0.791667
ffmpeg getPresentationTimestamp(21): = 0.833333
ffmpeg getPresentationTimestamp(22): = 0.875
ffmpeg getPresentationTimestamp(23): = 0.916667
ffmpeg getPresentationTimestamp(24): = 0.958333
ffmpeg getPresentationTimestamp(25): = 1
ffmpeg getPresentationTimestamp(26): = 1.041667
ffmpeg getPresentationTimestamp(27): = 1.083333
ffmpeg getPresentationTimestamp(28): = 1.125
ffmpeg getPresentationTimestamp(29): = 1.166667
ffmpeg getPresentationTimestamp(30): = 1.208333
ffmpeg getPresentationTimestamp(31): = 1.25
ffmpeg getPresentationTimestamp(32): = 1.291667
ffmpeg getPresentationTimestamp(33): = 1.333333
ffmpeg getPresentationTimestamp(34): = 1.375
ffmpeg getPresentationTimestamp(35): = 1.416667
ffmpeg getPresentationTimestamp(36): = 1.458333
ffmpeg getPresentationTimestamp(37): = 1.5
ffmpeg getPresentationTimestamp(38): = 1.541667
ffmpeg getPresentationTimestamp(39): = 1.583333
ffmpeg getPresentationTimestamp(40): = 1.625
ffmpeg getPresentationTimestamp(41): = 1.666667
ffmpeg getPresentationTimestamp(42): = 1.708333
ffmpeg getPresentationTimestamp(43): = 1.75
ffmpeg getPresentationTimestamp(44): = 1.791667
ffmpeg getPresentationTimestamp(45): = 1.833333
ffmpeg getPresentationTimestamp(46): = 1.875
ffmpeg getPresentationTimestamp(47): = 1.916667
ffmpeg getPresentationTimestamp(48): = 1.958333
ffmpeg getPresentationTimestamp(49): = 2
ffmpeg getPresentationTimestamp(50): = 2.041667
ffmpeg getPresentationTimestamp(51): = 2.083333
ffmpeg getPresentationTimestamp(52): = 2.125
ffmpeg getPresentationTimestamp(53): = 2.166667
ffmpeg getPresentationTimestamp(54): = 2.208333
ffmpeg getPresentationTimestamp(55): = 2.25
ffmpeg getPresentationTimestamp(56): = 2.291667
ffmpeg getPresentationTimestamp(57): = 2.333333
ffmpeg getPresentationTimestamp(58): = 2.375
ffmpeg getPresentationTimestamp(59): = 2.416667
ffmpeg getPresentationTimestamp(60): = 2.458333
ffmpeg getPresentationTimestamp(61): = 2.5
ffmpeg getPresentationTimestamp(62): = 2.541667
ffmpeg getPresentationTimestamp(63): = 2.583333
ffmpeg getPresentationTimestamp(64): = 2.625
ffmpeg getPresentationTimestamp(65): = 2.666667
ffmpeg getPresentationTimestamp(66): = 2.708333
ffmpeg getPresentationTimestamp(67): = 2.75
ffmpeg getPresentationTimestamp(68): = 2.791667
ffmpeg getPresentationTimestamp(69): = 2.833333
ffmpeg getPresentationTimestamp(70): = 2.875
ffmpeg getPresentationTimestamp(71): = 2.916667
ffmpeg getPresentationTimestamp(72): = 2.958333
ffmpeg getPresentationTimestamp(73): = 3
ffmpeg getPresentationTimestamp(74): = 3.041667
ffmpeg getPresentationTimestamp(75): = 3.083333
ffmpeg getPresentationTimestamp(76): = 3.125
ffmpeg getPresentationTimestamp(77): = 3.166667
ffmpeg getPresentationTimestamp(78): = 3.208333
ffmpeg getPresentationTimestamp(79): = 3.25
ffmpeg getPresentationTimestamp(80): = 3.291667
ffmpeg getPresentationTimestamp(81): = 3.333333
ffmpeg getPresentationTimestamp(82): = 3.375
ffmpeg getPresentationTimestamp(83): = 3.416667
ffmpeg getPresentationTimestamp(84): = 3.458333
ffmpeg getPresentationTimestamp(85): = 3.5
ffmpeg getPresentationTimestamp(86): = 3.541667
ffmpeg getPresentationTimestamp(87): = 3.583333
ffmpeg getPresentationTimestamp(88): = 3.625
ffmpeg getPresentationTimestamp(89): = 3.666667
ffmpeg getPresentationTimestamp(90): = 3.708333
ffmpeg getPresentationTimestamp(91): = 3.75
ffmpeg getPresentationTimestamp(92): = 3.791667
ffmpeg getPresentationTimestamp(93): = 3.833333
ffmpeg getPresentationTimestamp(94): = 3.875
ffmpeg getPresentationTimestamp(95): = 3.916667
ffmpeg getPresentationTimestamp(96): = 3.958333
ffmpeg getPresentationTimestamp(97): = 4
ffmpeg getPresentationTimestamp(98): = 4.041667
ffmpeg getPresentationTimestamp(99): = 4.083333
ffmpeg getPresentationTimestamp(100): = 4.125
ffmpeg getPresentationTimestamp(101): = 4.166667
ffmpeg getPresentationTimestamp(102): = 4.208333
ffmpeg getPresentationTimestamp(103): = 4.25
ffmpeg getPresentationTimestamp(104): = 4.291667
ffmpeg getPresentationTimestamp(105): = 4.333333
ffmpeg getPresentationTimestamp(106): = 4.375
ffmpeg getPresentationTimestamp(107): = 4.416667
ffmpeg getPresentationTimestamp(108): = 4.458333
ffmpeg getPresentationTimestamp(109): = 4.5
ffmpeg getPresentationTimestamp(110): = 4.541667
ffmpeg getPresentationTimestamp(111): = 4.583333
ffmpeg getPresentationTimestamp(112): = 4.625
ffmpeg getPresentationTimestamp(113): = 4.666667
ffmpeg getPresentationTimestamp(114): = 4.708333
ffmpeg getPresentationTimestamp(115): = 4.75
ffmpeg getPresentationTimestamp(116): = 4.791667
ffmpeg getPresentationTimestamp(117): = 4.833333
ffmpeg getPresentationTimestamp(118): = 4.875
ffmpeg getPresentationTimestamp(119): = 4.916667
ffmpeg getPresentationTimestamp(120): = 4.958333
