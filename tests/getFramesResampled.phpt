--TEST--
ffmpeg getFrameResampled test
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
    $img = sprintf("%s/test-%04d.png", dirname(__FILE__), $i);
    $width = 40 + ($i % 2 ? $i+1 : $i);
    $height = 40 + ($i % 2 ? $i+1 : $i);
    $frame = $mov->getFrame($i);
    $frame->resize($width, $height); 
    $gd_image = $frame->toGDImage();
    imagepng($gd_image, $img);
    printf("ffmpeg getFrameResampled(%d): md5 = %s\n", 
            $i, md5(file_get_contents($img)));
    imagedestroy($gd_image);
    unlink($img);
}
?>
--EXPECT--
ffmpeg getFrameResampled(1): md5 = 569f2a072d2b320217b06ee7af55e2b1
ffmpeg getFrameResampled(2): md5 = f84313afb867709a5edb5accc5fc6aaa
ffmpeg getFrameResampled(3): md5 = da425ba861d0e397e0c9d22342dba7e9
ffmpeg getFrameResampled(4): md5 = da425ba861d0e397e0c9d22342dba7e9
ffmpeg getFrameResampled(5): md5 = 61e50f436a445b5a66d93e358a7b85f1
ffmpeg getFrameResampled(6): md5 = 61ceba54e069628d484831f4f3b3037c
ffmpeg getFrameResampled(7): md5 = 9ed8c15c97d4236de6538742fb9843e2
ffmpeg getFrameResampled(8): md5 = dd2d1bc83012b240cb481bb27b9efa32
ffmpeg getFrameResampled(9): md5 = 2d53b3e7922f992d0aed361366a3fb5a
ffmpeg getFrameResampled(10): md5 = fa31c8cad04535345068d6de73aff1c3
ffmpeg getFrameResampled(11): md5 = 4faef76903dfb895c4351576ff514029
ffmpeg getFrameResampled(12): md5 = 2906e159e56567a38cd6267bbc75f8db
ffmpeg getFrameResampled(13): md5 = 9d40faad2ca2f724a9a6c57ed0de3c29
ffmpeg getFrameResampled(14): md5 = 9d40faad2ca2f724a9a6c57ed0de3c29
ffmpeg getFrameResampled(15): md5 = cf61bffb55e52b89978a01d189a1eeac
ffmpeg getFrameResampled(16): md5 = 281e3b994b1cf1c6ccac81951589376d
ffmpeg getFrameResampled(17): md5 = 029ff93b5da9c79c923b5cc3b4bfeb52
ffmpeg getFrameResampled(18): md5 = de21f22d19f84d7a521047adf6cd1fab
ffmpeg getFrameResampled(19): md5 = 6515086444b5cd00738f3dfbdb502c85
ffmpeg getFrameResampled(20): md5 = 6515086444b5cd00738f3dfbdb502c85
ffmpeg getFrameResampled(21): md5 = 823d9d4e1390f1c096c50c83838345b0
ffmpeg getFrameResampled(22): md5 = 372d7d4f1eb8717126c944dd7bde93b4
ffmpeg getFrameResampled(23): md5 = 6f942605916110d551bc173ac8f1cd45
ffmpeg getFrameResampled(24): md5 = e8ecd05939936bbf7f1966cfb62b849e
ffmpeg getFrameResampled(25): md5 = 743c00c43347447d06158cc68710c77b
ffmpeg getFrameResampled(26): md5 = 743c00c43347447d06158cc68710c77b
ffmpeg getFrameResampled(27): md5 = 030fb57a235cb743449ebcda9dcd0a6e
ffmpeg getFrameResampled(28): md5 = 835d11370bf0255aaa191e495ba2aed1
ffmpeg getFrameResampled(29): md5 = f72592e4c48289a012126d9bb69457fc
ffmpeg getFrameResampled(30): md5 = 6742312289119a2ce89a493b78f6b104
ffmpeg getFrameResampled(31): md5 = 48409b4de1dd34d879acff200ecd14c6
ffmpeg getFrameResampled(32): md5 = b55d04db8e32c9c43e8913bbc90ddc7c
ffmpeg getFrameResampled(33): md5 = 224cb0cefa03c88b4695faa4a99adc6a
ffmpeg getFrameResampled(34): md5 = 81ba32944bb621e03e57f83e7ee9e710
ffmpeg getFrameResampled(35): md5 = ba5507a293db2f1267640a45c1b2cd9f
ffmpeg getFrameResampled(36): md5 = e137782a4002482ed17f04404c7c7ce0
ffmpeg getFrameResampled(37): md5 = 244a14d872a16a1db710e374a3b61598
ffmpeg getFrameResampled(38): md5 = 244a14d872a16a1db710e374a3b61598
ffmpeg getFrameResampled(39): md5 = 576dc7c1d342229c94c56f7f7fe31db2
ffmpeg getFrameResampled(40): md5 = c0e08493b51eae82c2904c5ef0181646
ffmpeg getFrameResampled(41): md5 = 9f629459b722e37819b80244f3538112
ffmpeg getFrameResampled(42): md5 = b500155fd117e82b29907dd10166c029
ffmpeg getFrameResampled(43): md5 = 9541042630545e10afab253a651ca817
ffmpeg getFrameResampled(44): md5 = 9ac7890c1c287c4ba747aefe60eab530
ffmpeg getFrameResampled(45): md5 = 8febca074c7be9bb72ea76b0a3df1545
ffmpeg getFrameResampled(46): md5 = 8f12734bdeb2b0ff14a133d589df4f4c
ffmpeg getFrameResampled(47): md5 = e16876a2d1ea9aa1132bd48164ff6490
ffmpeg getFrameResampled(48): md5 = 14a191e7025c261c3b4ba3085293c361
ffmpeg getFrameResampled(49): md5 = 5afa88aec27671740edf8f34fbc66152
ffmpeg getFrameResampled(50): md5 = 5afa88aec27671740edf8f34fbc66152
ffmpeg getFrameResampled(51): md5 = eeff8ec94b7d8376ab41d89708c0debf
ffmpeg getFrameResampled(52): md5 = 7eeb4c27089b1dffc93eabb9cf740bcb
ffmpeg getFrameResampled(53): md5 = cf8e53d3d955aa1a847b15a9aadf07c8
ffmpeg getFrameResampled(54): md5 = 65bc217b0db5e3590e9dabf238e0026a
ffmpeg getFrameResampled(55): md5 = 83ae1cf74e7663f90f2e02cd4f009f13
ffmpeg getFrameResampled(56): md5 = 86c7247873f3156cc418b4dc306f7a1d
ffmpeg getFrameResampled(57): md5 = bb87414b519d1b536599628d500fafe0
ffmpeg getFrameResampled(58): md5 = 2c4fc8cff7462c0b682e85199d1c5191
ffmpeg getFrameResampled(59): md5 = e62f78af526e5a4cd37564474e836271
ffmpeg getFrameResampled(60): md5 = 5214f018866796621d89e57636de967c
ffmpeg getFrameResampled(61): md5 = 0fda1dea7c9c76aa38b2665fe7592320
ffmpeg getFrameResampled(62): md5 = 0fda1dea7c9c76aa38b2665fe7592320
ffmpeg getFrameResampled(63): md5 = 2e164ea5c9ffc8c1fbd1e3208161f857
ffmpeg getFrameResampled(64): md5 = 870acf52691579ace0197b1107dd2b37
ffmpeg getFrameResampled(65): md5 = 107f0b8efd6ce664b679bd4ab41c48ae
ffmpeg getFrameResampled(66): md5 = 51faa1b2c6bc69207a91bbd036f5b552
ffmpeg getFrameResampled(67): md5 = 16280ad39b443cc2769fa6a727e376ef
ffmpeg getFrameResampled(68): md5 = 68a3b23a447f1f8ffd76bc8d208648ed
ffmpeg getFrameResampled(69): md5 = 37740ab917063319fff96c737a1210d0
ffmpeg getFrameResampled(70): md5 = 57dccc0bdedcc37bb5dc1d643c97ca92
ffmpeg getFrameResampled(71): md5 = 5ec34825e3467af0013be2dae1b1b8fa
ffmpeg getFrameResampled(72): md5 = d67968efbf46cd75d56a54ac37211749
ffmpeg getFrameResampled(73): md5 = fb1fe680644ee85ff29a8d41ae49cf27
ffmpeg getFrameResampled(74): md5 = fb1fe680644ee85ff29a8d41ae49cf27
ffmpeg getFrameResampled(75): md5 = 0dcda6a31349a69b34237f25944e5bc5
ffmpeg getFrameResampled(76): md5 = 1e98fe87ba69e68bc23368b551871339
ffmpeg getFrameResampled(77): md5 = 72f016a0e7d84d39aeaf72a0171367ce
ffmpeg getFrameResampled(78): md5 = 3b47d91a70fd247031ecb50bf11b5256
ffmpeg getFrameResampled(79): md5 = 487c1d81598bb994196f1ba837fcf081
ffmpeg getFrameResampled(80): md5 = be81f2efa099a0d01cc4f5deb44cc9b1
ffmpeg getFrameResampled(81): md5 = 4cfacbfae92a153b422d0a8603e915a2
ffmpeg getFrameResampled(82): md5 = 1647798fc53e1ae55eadd482e2aa2702
ffmpeg getFrameResampled(83): md5 = ebfeffa00a3dff17f88aa816815d435f
ffmpeg getFrameResampled(84): md5 = 61424e7b4dab94b688574ba675a1f117
ffmpeg getFrameResampled(85): md5 = 335859f7206a787de1ced98747c8ad99
ffmpeg getFrameResampled(86): md5 = 335859f7206a787de1ced98747c8ad99
ffmpeg getFrameResampled(87): md5 = 8bf15a86b43b503041ebf744977764cd
ffmpeg getFrameResampled(88): md5 = bf8e0c8df444182ffdf12ec271b1b6f6
ffmpeg getFrameResampled(89): md5 = 343a0165ae640ad89be39f0700dc5f89
ffmpeg getFrameResampled(90): md5 = 2bdc8418f0ba355a4a00360d1dde82b9
ffmpeg getFrameResampled(91): md5 = e09ee21a4900480f70b0663b543298df
ffmpeg getFrameResampled(92): md5 = f328089db10ad7c5139c6bb2f86168f9
ffmpeg getFrameResampled(93): md5 = 69ed431fb7435b82a6595c839f64f8fc
ffmpeg getFrameResampled(94): md5 = ee565de18213c39b2a13ca21223ecc1c
ffmpeg getFrameResampled(95): md5 = e20779a66c9fedc3f5bc0cac6a6248cf
ffmpeg getFrameResampled(96): md5 = e9abb484089968c9bc18c3dd0497f778
ffmpeg getFrameResampled(97): md5 = 37de2de209c131589a49d5adcd7645b5
ffmpeg getFrameResampled(98): md5 = 37de2de209c131589a49d5adcd7645b5
ffmpeg getFrameResampled(99): md5 = e83d106555121501b3bd0cb2e680b962
ffmpeg getFrameResampled(100): md5 = e91d9b2033459954ee266e68ecc81bcd
ffmpeg getFrameResampled(101): md5 = ba75420ed88e9d4aa370a3c495991bc1
ffmpeg getFrameResampled(102): md5 = 1885d60aca181408830b3da702d4eb55
ffmpeg getFrameResampled(103): md5 = c409034d0990e4427020479509031f42
ffmpeg getFrameResampled(104): md5 = c23068d0a372e63e39fe78de01861f3f
ffmpeg getFrameResampled(105): md5 = eef51e66696146f0126937008b814e42
ffmpeg getFrameResampled(106): md5 = 0092e2a48e7d0496e0829830d3db6789
ffmpeg getFrameResampled(107): md5 = fbcca1ca4c4952c213562feed8232d66
ffmpeg getFrameResampled(108): md5 = e75af8837e8aae55d916423f3d33e5ac
ffmpeg getFrameResampled(109): md5 = 404f4f4a03eebd26dce5ad8350260895
ffmpeg getFrameResampled(110): md5 = 404f4f4a03eebd26dce5ad8350260895
ffmpeg getFrameResampled(111): md5 = eec0f521cb4b34c8c33278c0ceb26477
ffmpeg getFrameResampled(112): md5 = c1249862f33657dfab281e5d572e423c
ffmpeg getFrameResampled(113): md5 = f7ed97aebcbca3e149a0462162bf298c
ffmpeg getFrameResampled(114): md5 = c7d13e6e31aa268a6b2c840fc3ffdbde
ffmpeg getFrameResampled(115): md5 = 13600dd1965de3a9c7a96503c0178aeb
ffmpeg getFrameResampled(116): md5 = cf87adfc60252aad2278140755f01139
ffmpeg getFrameResampled(117): md5 = 243505567c12944f5f054bacde0510ba
ffmpeg getFrameResampled(118): md5 = c4358fb603ebdfc3725ce42e3275e3dd
ffmpeg getFrameResampled(119): md5 = b1620af757c0c43c499065aaaf7b6ff1
ffmpeg getFrameResampled(120): md5 = ba8c84f881fbd36b01bff158b4cc7fa6
