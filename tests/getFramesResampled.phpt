--TEST--
ffmpeg getFramesResampled test
--SKIPIF--
<?php 
extension_loaded('ffmpeg') or die("skip ffmpeg extension not loaded"); 
extension_loaded('gd') or die("skip gd extension not avaliable.");
function_exists("imagecreatetruecolor") or die("skip function imagecreatetruecolor unavailable");
?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/robot.avi');

$framecount = $mov->getFrameCount();
for($i = 1; $i <= $framecount; $i++) {
    $img = sprintf("%s/test-%04d.png", dirname(__FILE__), $i);
    $width = 40 + ($i % 2 ? $i+1 : $i);
    $height = 40 + ($i % 2 ? $i+1 : $i);
    $frame = $mov->getFrame($i);
    $frame->resize($width, $height); 
    $gd_image = $frame->toGDImage();
    imagepng($gd_image, $img);
    printf("ffmpeg getFramesResampled(%d): md5 = %s\n", 
            $i, md5(file_get_contents($img)));
    imagedestroy($gd_image);
    unlink($img);
}
?>
--EXPECT--
ffmpeg getFramesResampled(1): md5 = da815929cfd0580a1db04935bb908f85
ffmpeg getFramesResampled(2): md5 = da815929cfd0580a1db04935bb908f85
ffmpeg getFramesResampled(3): md5 = ac78b620648b015f559a54772cafb9a3
ffmpeg getFramesResampled(4): md5 = 3440b3809adc674de5090b23db0ff684
ffmpeg getFramesResampled(5): md5 = f94cfeab75fe15b99f6b5aed098e3002
ffmpeg getFramesResampled(6): md5 = f94cfeab75fe15b99f6b5aed098e3002
ffmpeg getFramesResampled(7): md5 = 221c5d7c3b7788cc76beba4e6546dd65
ffmpeg getFramesResampled(8): md5 = 221c5d7c3b7788cc76beba4e6546dd65
ffmpeg getFramesResampled(9): md5 = 813d4eef8e7f3cae5d93bcc64293269a
ffmpeg getFramesResampled(10): md5 = f8cb39a306633f39b90301b225f5a7cc
ffmpeg getFramesResampled(11): md5 = bc239c6ee6031545d077b50c04bdfcc9
ffmpeg getFramesResampled(12): md5 = bc239c6ee6031545d077b50c04bdfcc9
ffmpeg getFramesResampled(13): md5 = 4e4283a52a90db22f1eb157591c4aae2
ffmpeg getFramesResampled(14): md5 = 4e4283a52a90db22f1eb157591c4aae2
ffmpeg getFramesResampled(15): md5 = 020ba1a2b37acdaa9305a0a19e249452
ffmpeg getFramesResampled(16): md5 = ee0069c69f302eae8c805630975e9e9f
ffmpeg getFramesResampled(17): md5 = 0ca190b9113dba0331df25cf57094dd1
ffmpeg getFramesResampled(18): md5 = 0ca190b9113dba0331df25cf57094dd1
ffmpeg getFramesResampled(19): md5 = 73a04962503c522f19287558de8198d5
ffmpeg getFramesResampled(20): md5 = 73a04962503c522f19287558de8198d5
ffmpeg getFramesResampled(21): md5 = 8b6ae3905ca9aace6392a1260299b4af
ffmpeg getFramesResampled(22): md5 = f56dd3da1cabb347db03695d95b6738c
ffmpeg getFramesResampled(23): md5 = 2904e176014ba0df06ebe93abf130314
ffmpeg getFramesResampled(24): md5 = 2904e176014ba0df06ebe93abf130314
ffmpeg getFramesResampled(25): md5 = 8179dd2ff66c9fbd2dd3193757487d00
ffmpeg getFramesResampled(26): md5 = 8179dd2ff66c9fbd2dd3193757487d00
ffmpeg getFramesResampled(27): md5 = 651f6d073bbc40d5e274823c168532a1
ffmpeg getFramesResampled(28): md5 = 93b86aae8ee02ac68dfa7c7cc4dc680c
ffmpeg getFramesResampled(29): md5 = 71c4f4684f060c6c19ef3e23bf80c288
ffmpeg getFramesResampled(30): md5 = 71c4f4684f060c6c19ef3e23bf80c288
ffmpeg getFramesResampled(31): md5 = e25da35be99c1f17af4f2cb5b4f32780
ffmpeg getFramesResampled(32): md5 = e25da35be99c1f17af4f2cb5b4f32780
ffmpeg getFramesResampled(33): md5 = dd33ddc22d9ddf925df5e44c9176768f
ffmpeg getFramesResampled(34): md5 = b85f1d27947136b26fb4f18c20e1f7a2
ffmpeg getFramesResampled(35): md5 = 72d5bccee540b006848dd5e78967346c
ffmpeg getFramesResampled(36): md5 = 72d5bccee540b006848dd5e78967346c
ffmpeg getFramesResampled(37): md5 = a7eea806eb3e32921f34cd11cb5416e2
ffmpeg getFramesResampled(38): md5 = a7eea806eb3e32921f34cd11cb5416e2
ffmpeg getFramesResampled(39): md5 = b7ff2440f930d3c5186e1b6f15024cb7
ffmpeg getFramesResampled(40): md5 = 10357e6c84a8dc48091b02f2def180d1
ffmpeg getFramesResampled(41): md5 = 15785b55a136d59c4f829c45607083a8
ffmpeg getFramesResampled(42): md5 = 15785b55a136d59c4f829c45607083a8
ffmpeg getFramesResampled(43): md5 = 2c8a05d37c114848931304d94f97d9da
ffmpeg getFramesResampled(44): md5 = 2c8a05d37c114848931304d94f97d9da
ffmpeg getFramesResampled(45): md5 = bd5ef22f6302b423ee74fa040d910945
ffmpeg getFramesResampled(46): md5 = 81114e782de1b038d926e7ad56234d94
ffmpeg getFramesResampled(47): md5 = ea0f6d89e482a798419fc674cd1b5cf2
ffmpeg getFramesResampled(48): md5 = ea0f6d89e482a798419fc674cd1b5cf2
ffmpeg getFramesResampled(49): md5 = dfce3c62c55a39c4f81384766d50b669
ffmpeg getFramesResampled(50): md5 = dfce3c62c55a39c4f81384766d50b669
ffmpeg getFramesResampled(51): md5 = 29a7da8a0010ebbcc33a2f116a5e3d41
ffmpeg getFramesResampled(52): md5 = a6ebcca2401429ed152564b87e309ec6
ffmpeg getFramesResampled(53): md5 = bc6e4797e9be84d942eb50c4ffe25007
ffmpeg getFramesResampled(54): md5 = bc6e4797e9be84d942eb50c4ffe25007
ffmpeg getFramesResampled(55): md5 = 58ef58fe8bc0d8b10ee0e19987fc8616
ffmpeg getFramesResampled(56): md5 = 58ef58fe8bc0d8b10ee0e19987fc8616
ffmpeg getFramesResampled(57): md5 = 38d317caadbb92902eb61c895bd17579
ffmpeg getFramesResampled(58): md5 = 1807c3e01c8b32096c10a794ddbd660e
ffmpeg getFramesResampled(59): md5 = 1ab7fe88210055c31ec61d3f8c1937b7
ffmpeg getFramesResampled(60): md5 = 1ab7fe88210055c31ec61d3f8c1937b7
ffmpeg getFramesResampled(61): md5 = 621efbda5a6359f22d910dd91d6d0b33
ffmpeg getFramesResampled(62): md5 = 621efbda5a6359f22d910dd91d6d0b33
ffmpeg getFramesResampled(63): md5 = 81efefbb7c3d23114ae30f995098da6d
ffmpeg getFramesResampled(64): md5 = e47836750b4145fd334cb3f9140c7d52
ffmpeg getFramesResampled(65): md5 = 05a5ce792777a09fca7ce0406ea6f590
ffmpeg getFramesResampled(66): md5 = 05a5ce792777a09fca7ce0406ea6f590
ffmpeg getFramesResampled(67): md5 = af73589bf171a8eb02b2e25ae0eb5dde
ffmpeg getFramesResampled(68): md5 = af73589bf171a8eb02b2e25ae0eb5dde
ffmpeg getFramesResampled(69): md5 = 913a1a9f69937d4f34e6ddc13f93d7fc
ffmpeg getFramesResampled(70): md5 = 7bebf9b08778c9bb4a499e82e01ec9f6
ffmpeg getFramesResampled(71): md5 = ed7c97fb24a5fe2f5a17d1bd213e2134
ffmpeg getFramesResampled(72): md5 = ed7c97fb24a5fe2f5a17d1bd213e2134
ffmpeg getFramesResampled(73): md5 = 5eb44f2dc4c99657e056022df634291f
ffmpeg getFramesResampled(74): md5 = 5eb44f2dc4c99657e056022df634291f
ffmpeg getFramesResampled(75): md5 = 8ac851efc9cb7f79cba469072a4a9231
ffmpeg getFramesResampled(76): md5 = 46ef5e381da86e37d05f4a5e2e33a10d
ffmpeg getFramesResampled(77): md5 = 2731cb243e2943f0244501f0e4f7a5e9
ffmpeg getFramesResampled(78): md5 = 2731cb243e2943f0244501f0e4f7a5e9
ffmpeg getFramesResampled(79): md5 = fa163de24df2f28dea996e7e8bbabd1a
ffmpeg getFramesResampled(80): md5 = fa163de24df2f28dea996e7e8bbabd1a
ffmpeg getFramesResampled(81): md5 = 84217099891b1f84fd90a76b0c8935f8
ffmpeg getFramesResampled(82): md5 = d211cccd779305759005f5ee156a72ca
ffmpeg getFramesResampled(83): md5 = c15aeb80fec2b324882c95d9654f94b3
ffmpeg getFramesResampled(84): md5 = c15aeb80fec2b324882c95d9654f94b3
ffmpeg getFramesResampled(85): md5 = f36bac94f2dcbec0c8f31d4f4f14da92
ffmpeg getFramesResampled(86): md5 = f36bac94f2dcbec0c8f31d4f4f14da92
ffmpeg getFramesResampled(87): md5 = 282a4812447c9ba774cb4bb3d90938cd
ffmpeg getFramesResampled(88): md5 = 4ccd8ff047e2f5ae0fe27db291d2fe7c
ffmpeg getFramesResampled(89): md5 = f7e51583d06eb78e58c81ecf50220bb4
ffmpeg getFramesResampled(90): md5 = f7e51583d06eb78e58c81ecf50220bb4
ffmpeg getFramesResampled(91): md5 = 437cd6b0a67081da38647d1ef49abc98
ffmpeg getFramesResampled(92): md5 = 437cd6b0a67081da38647d1ef49abc98
ffmpeg getFramesResampled(93): md5 = da42b958139e67f0765a8943d6742bad
ffmpeg getFramesResampled(94): md5 = 82bea4636e1526985f7d33eaab3c7d10
ffmpeg getFramesResampled(95): md5 = 04443e252a36a596e53359a0f3ec0304
ffmpeg getFramesResampled(96): md5 = 04443e252a36a596e53359a0f3ec0304
ffmpeg getFramesResampled(97): md5 = 8d0978857139479280cfe37ec819cdd4
ffmpeg getFramesResampled(98): md5 = 8d0978857139479280cfe37ec819cdd4
ffmpeg getFramesResampled(99): md5 = 685e17e84e324a42e1b2481a149ac191
ffmpeg getFramesResampled(100): md5 = 760b47ea511c34492d5aef41b00aa03a
ffmpeg getFramesResampled(101): md5 = a2fcd83f4c2d37b1a705bc548b991c65
ffmpeg getFramesResampled(102): md5 = a2fcd83f4c2d37b1a705bc548b991c65
ffmpeg getFramesResampled(103): md5 = 87cc4f897524946db19a6cd285963f05
ffmpeg getFramesResampled(104): md5 = 87cc4f897524946db19a6cd285963f05
ffmpeg getFramesResampled(105): md5 = 72298a654a6f1055578f882ef29bc7ed
ffmpeg getFramesResampled(106): md5 = 3c65501b8d81529211fef0fab670d563
ffmpeg getFramesResampled(107): md5 = 8be967348923abe9b9f54fc29b09f611
ffmpeg getFramesResampled(108): md5 = 8be967348923abe9b9f54fc29b09f611
ffmpeg getFramesResampled(109): md5 = 5ada4563ca6c747504c9b81425798bd9
ffmpeg getFramesResampled(110): md5 = 5ada4563ca6c747504c9b81425798bd9
ffmpeg getFramesResampled(111): md5 = 0a82b597ccdec04b1c3148945754bd38
ffmpeg getFramesResampled(112): md5 = 8dd67ea383e9593d4f6b1329f26489ce
ffmpeg getFramesResampled(113): md5 = 6cee9769c76f1c1f3f724bc5e44e0433
ffmpeg getFramesResampled(114): md5 = 6cee9769c76f1c1f3f724bc5e44e0433
ffmpeg getFramesResampled(115): md5 = d084b0be2afe60d1b658d0ad7ace0400
ffmpeg getFramesResampled(116): md5 = d084b0be2afe60d1b658d0ad7ace0400
ffmpeg getFramesResampled(117): md5 = 940f3fb6de0312acf145b447f4894dec
ffmpeg getFramesResampled(118): md5 = 022184af4a0ff14588b71707e84d418c
ffmpeg getFramesResampled(119): md5 = 470e19278acfbfdb0e3e4f5ee6c0350b
ffmpeg getFramesResampled(120): md5 = 470e19278acfbfdb0e3e4f5ee6c0350b
