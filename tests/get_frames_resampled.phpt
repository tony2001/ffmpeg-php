--TEST--
ffmpeg get_frame forwards test
--SKIPIF--
<?php 
extension_loaded('ffmpeg') or die("skip ffmpeg extension not loaded"); 
extension_loaded('gd') or die("skip gd extension not avaliable.");
function_exists("imagecreatetruecolor") or die("skip function imagecreatetruecolor unavailable");
?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/test.avi');

$framecount = $mov->get_framecount();
for($i = 1; $i <= $framecount; $i++) {
    $img = sprintf("%s/test-%04d.png", dirname(__FILE__), $i);
    $width = 40 + ($i % 2 ? $i+1 : $i);
    $height = 40 + ($i % 2 ? $i+1 : $i);
    $gd_image = $mov->get_frame_resampled($width, $height, $i);
    imagepng($gd_image, $img);
    printf("ffmpeg get_frame_resampled(%d, %d, %d): md5 = %s\n", 
            $i, $width, $height, md5(file_get_contents($img)));
    imagedestroy($gd_image);
    unlink($img);
}
?>
--EXPECT--
ffmpeg get_frame_resampled(1): md5 = 0f2f212f71055aaee46d9ee2844ad61d
ffmpeg get_frame_resampled(2): md5 = f9b4ee61db67b390abfb13be9cb0bca2
ffmpeg get_frame_resampled(3): md5 = 0a4b6035fe0cdc6bbed4ba2ff6c62654
ffmpeg get_frame_resampled(4): md5 = 0a4b6035fe0cdc6bbed4ba2ff6c62654
ffmpeg get_frame_resampled(5): md5 = 66da3bdd0c834fbcbf273aefb0a21ab2
ffmpeg get_frame_resampled(6): md5 = b423c05641db61f4cec097e52f53512f
ffmpeg get_frame_resampled(7): md5 = ca1a6a0fae99e9b9b6924dd1ce095429
ffmpeg get_frame_resampled(8): md5 = c6ea8c4795d55acf3be661c6199d238a
ffmpeg get_frame_resampled(9): md5 = e3cbdfdfd7e14180917147d7e24c93ba
ffmpeg get_frame_resampled(10): md5 = 13fe21907735ec8fd730dd67c81fd577
ffmpeg get_frame_resampled(11): md5 = 9dc7835e96119947bec2e669a18dd76d
ffmpeg get_frame_resampled(12): md5 = 136a991f33998d889b75b6ffc0be4f84
ffmpeg get_frame_resampled(13): md5 = 0fa1138c986f59059ca3767cb1ba395d
ffmpeg get_frame_resampled(14): md5 = 0fa1138c986f59059ca3767cb1ba395d
ffmpeg get_frame_resampled(15): md5 = 8e42b896278dff4fe57922fffc59bfb9
ffmpeg get_frame_resampled(16): md5 = ad6eb002866e6a260702b876571a7cd1
ffmpeg get_frame_resampled(17): md5 = 53ee28e1fe471f1f2414f49845a9cc95
ffmpeg get_frame_resampled(18): md5 = bb588b57973f877f906059635f2f2402
ffmpeg get_frame_resampled(19): md5 = 58edc0174cb99a075211dd9e098bcf7e
ffmpeg get_frame_resampled(20): md5 = 58edc0174cb99a075211dd9e098bcf7e
ffmpeg get_frame_resampled(21): md5 = 71a7580cba07535653d5b9bfb66ec40c
ffmpeg get_frame_resampled(22): md5 = 2956de46a962f22fd17919b335d3a2e0
ffmpeg get_frame_resampled(23): md5 = 96d1cac3d75c319bede571160162dbcd
ffmpeg get_frame_resampled(24): md5 = a246d429f940c257095ee5da28806698
ffmpeg get_frame_resampled(25): md5 = b4a86b729684a1be134e1b0b7882a4c0
ffmpeg get_frame_resampled(26): md5 = b4a86b729684a1be134e1b0b7882a4c0
ffmpeg get_frame_resampled(27): md5 = 6b12865525d8a808b485f40d65205f5b
ffmpeg get_frame_resampled(28): md5 = a9a336f190aa56216a9d2d6ff1cdd418
ffmpeg get_frame_resampled(29): md5 = a08242649ac9897dcbd2656c976c7993
ffmpeg get_frame_resampled(30): md5 = 7c723a91339dc4921cc3db71072af99e
ffmpeg get_frame_resampled(31): md5 = 1ccc12c19a5e05bb3740a26957cbc449
ffmpeg get_frame_resampled(32): md5 = 6c9e7bcd433bd3f551bc857cf87f4f39
ffmpeg get_frame_resampled(33): md5 = d1f67d68bf634422fb124958304af28d
ffmpeg get_frame_resampled(34): md5 = 77c711115903d3893f191986b6ff73aa
ffmpeg get_frame_resampled(35): md5 = 9d9c66732d4f4a51c4b1db1fe7663e40
ffmpeg get_frame_resampled(36): md5 = d9f0601abde11236439e2c9fd5487d8f
ffmpeg get_frame_resampled(37): md5 = 47f519ef7044e9c6b475a58dbaa5be1a
ffmpeg get_frame_resampled(38): md5 = 47f519ef7044e9c6b475a58dbaa5be1a
ffmpeg get_frame_resampled(39): md5 = 8140d0e0c62ffc7d5e624e18ca36f80c
ffmpeg get_frame_resampled(40): md5 = 201178f37e01319f82a8dfa3b70f1c0f
ffmpeg get_frame_resampled(41): md5 = 48825e8bd760bbb23e4be1421f508ae9
ffmpeg get_frame_resampled(42): md5 = 8a562f9618597841bec57bb865131e42
ffmpeg get_frame_resampled(43): md5 = 06bdc35138074c82e8d5ebb5d86fcb8d
ffmpeg get_frame_resampled(44): md5 = 4b3541706801e9919ba2bdcb6e3b834f
ffmpeg get_frame_resampled(45): md5 = 23ffb75a31a6fe2464109955b3cb1c0c
ffmpeg get_frame_resampled(46): md5 = a20c05068ec7d42a505742c6c7b065ec
ffmpeg get_frame_resampled(47): md5 = 917cd193c3b182fab24e97af707e8272
ffmpeg get_frame_resampled(48): md5 = 23bfa9cef931dbff21ee93bdc377ce55
ffmpeg get_frame_resampled(49): md5 = e34280b2daab80ead3b344fe7e86ba1d
ffmpeg get_frame_resampled(50): md5 = e34280b2daab80ead3b344fe7e86ba1d
ffmpeg get_frame_resampled(51): md5 = 7b95aa4c4033804b7777b6d6e2e5e4fa
ffmpeg get_frame_resampled(52): md5 = 0c153c79505e085dc14865f0fe192707
ffmpeg get_frame_resampled(53): md5 = 2c9d9565d19c7a60a0e86268a660dcbe
ffmpeg get_frame_resampled(54): md5 = 0b61f34e421b3b170a87a3aed35c877c
ffmpeg get_frame_resampled(55): md5 = 4c9c958e852759fd3ff72c1eb0f1b934
ffmpeg get_frame_resampled(56): md5 = 4f2f128e35ae619aac58a2b029eb7a3c
ffmpeg get_frame_resampled(57): md5 = 4a8f72c24225e4869dc2926ff554c707
ffmpeg get_frame_resampled(58): md5 = b818561d6224c2dfde78356fbdce1790
ffmpeg get_frame_resampled(59): md5 = 624deaedac5fad6c7200c9e0db276ad9
ffmpeg get_frame_resampled(60): md5 = 3df4613dc724467bf0605dbd10dcbf03
ffmpeg get_frame_resampled(61): md5 = b4b4ace134a16c09cd193466911c3a95
ffmpeg get_frame_resampled(62): md5 = b4b4ace134a16c09cd193466911c3a95
ffmpeg get_frame_resampled(63): md5 = 34995f1845fb75da4f45ef4a1dcf4b75
ffmpeg get_frame_resampled(64): md5 = bd8da53d8f2bcc4c71f836e8ea5d01ba
ffmpeg get_frame_resampled(65): md5 = 41e1364860eab845a12f0d0c5283f625
ffmpeg get_frame_resampled(66): md5 = 26f66d3fbc459522724bb6df9ef0049a
ffmpeg get_frame_resampled(67): md5 = 62df6550239f4c91cf32bd021f4bb34f
ffmpeg get_frame_resampled(68): md5 = 055ce87dd46c064d3d9f76382a35b2a2
ffmpeg get_frame_resampled(69): md5 = d4651e5ec16a485e29aee9d5634fad89
ffmpeg get_frame_resampled(70): md5 = 135b7ff2a0e5d15dc9c111c94e74c9a4
ffmpeg get_frame_resampled(71): md5 = d11a811e9abae2997f940cb47b607a83
ffmpeg get_frame_resampled(72): md5 = 47e8f5d8b40ae503703bd02b7af47ea8
ffmpeg get_frame_resampled(73): md5 = 5ac497e1a483220654136103ad7a4688
ffmpeg get_frame_resampled(74): md5 = 5ac497e1a483220654136103ad7a4688
ffmpeg get_frame_resampled(75): md5 = ff0aafb95e3683f76df833ead793d020
ffmpeg get_frame_resampled(76): md5 = 5547efeadde31af5db151ee6098e0219
ffmpeg get_frame_resampled(77): md5 = 0e9d59b94e47f11fdd3d4a5057bef608
ffmpeg get_frame_resampled(78): md5 = 0bdae9169a4d7a1c8157f383050a778b
ffmpeg get_frame_resampled(79): md5 = 9ba03035a35391cbefc01f5c3e1ba769
ffmpeg get_frame_resampled(80): md5 = 7f677039b5de04fb486761c44ebe231c
ffmpeg get_frame_resampled(81): md5 = 4760b23869fc977ad233df2ad0d3d9d0
ffmpeg get_frame_resampled(82): md5 = a23e25bb27b945d96b6ea20e507ca3ff
ffmpeg get_frame_resampled(83): md5 = 9471db7368b0ffac233a4c6046768f8f
ffmpeg get_frame_resampled(84): md5 = 11448d609620711e46e7f59baf319c15
ffmpeg get_frame_resampled(85): md5 = 904282625a7ea7f6f82873ace111f747
ffmpeg get_frame_resampled(86): md5 = 904282625a7ea7f6f82873ace111f747
ffmpeg get_frame_resampled(87): md5 = d1a29617a0dfa689eb1e431155050361
ffmpeg get_frame_resampled(88): md5 = ccf7243cbecee26d3f1bb09a3ee81e1e
ffmpeg get_frame_resampled(89): md5 = 20ff3080c71125e73f0cd76f8869500a
ffmpeg get_frame_resampled(90): md5 = 17ba3e87210cefc3168d5124d4c25e46
ffmpeg get_frame_resampled(91): md5 = ab9bc4f407503b156e7398f38206682f
ffmpeg get_frame_resampled(92): md5 = ae22f96a11683860dd24a93513433bdf
ffmpeg get_frame_resampled(93): md5 = 384d1d631e67f152b6c6c4a4e19e510c
ffmpeg get_frame_resampled(94): md5 = 958d292c71238056bbea12212bf7356c
ffmpeg get_frame_resampled(95): md5 = c5dcc7fe1d363ca137e0af1357616f71
ffmpeg get_frame_resampled(96): md5 = 02c149810a77124c9e0d6cbffcaaeb74
ffmpeg get_frame_resampled(97): md5 = af871cfb2fa678c5b06a8db5b4731b78
ffmpeg get_frame_resampled(98): md5 = af871cfb2fa678c5b06a8db5b4731b78
ffmpeg get_frame_resampled(99): md5 = dbdc123980c3ed2724024bd03dfa42aa
ffmpeg get_frame_resampled(100): md5 = f8b92811563e9bebf8434a6ce872b35f
ffmpeg get_frame_resampled(101): md5 = f48fa48bc48917fe33136c3481beaeef
ffmpeg get_frame_resampled(102): md5 = 7db41c82cc44930811c548711eafaa02
ffmpeg get_frame_resampled(103): md5 = c01d03b98ee630469f8a1ad593f548e1
ffmpeg get_frame_resampled(104): md5 = 8b8d86dbae4fa6c56dce1fc433b260a8
ffmpeg get_frame_resampled(105): md5 = 0b88a21bbc9d34e47e763c65abe474bc
ffmpeg get_frame_resampled(106): md5 = 41ab5667947f90fe84d8ca62296515e9
ffmpeg get_frame_resampled(107): md5 = 79fed9344749a1f38c724a03ebad9dad
ffmpeg get_frame_resampled(108): md5 = 4ec17cce511d501998c2827dc4c3862d
ffmpeg get_frame_resampled(109): md5 = 73ac0b62f1328263bbed69d62cc049e0
ffmpeg get_frame_resampled(110): md5 = 73ac0b62f1328263bbed69d62cc049e0
ffmpeg get_frame_resampled(111): md5 = cdfa25e4c515cdb4c6c839dc21ded16d
ffmpeg get_frame_resampled(112): md5 = dce9e022cea063a8acf234ca7f174a47
ffmpeg get_frame_resampled(113): md5 = 0b1ec2f239e45da64f8c78e46184c427
ffmpeg get_frame_resampled(114): md5 = f8abd204b2431fe5d11aedb08fcad7b3
ffmpeg get_frame_resampled(115): md5 = 55abc0dd9b469ecc58769af1227019ae
ffmpeg get_frame_resampled(116): md5 = 546bcf6948386244621ac843709fcbab
ffmpeg get_frame_resampled(117): md5 = d1439b4b00a353350cdf59c83c9b33a5
ffmpeg get_frame_resampled(118): md5 = 4c6479a21aed33e9c12a7e77d8a01862
ffmpeg get_frame_resampled(119): md5 = cfa84df7cae53ccb6585c569796df35d
ffmpeg get_frame_resampled(120): md5 = ea95a90c253fdd9ddfe4b3db6261a0ab
