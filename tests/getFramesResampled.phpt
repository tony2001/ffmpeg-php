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
ffmpeg getFramesResampled(1): md5 = d8c6aaf804d22927aa9f81753d69458e
ffmpeg getFramesResampled(2): md5 = 63da9334223d32812ec8b3a3832d44f7
ffmpeg getFramesResampled(3): md5 = cdc826a22c163bfff4c04b2f0eaef769
ffmpeg getFramesResampled(4): md5 = cdc826a22c163bfff4c04b2f0eaef769
ffmpeg getFramesResampled(5): md5 = 56785d82fdfffc0d79a40b5f7c485427
ffmpeg getFramesResampled(6): md5 = 152ec8d11b306b4a199b8d70b7fed800
ffmpeg getFramesResampled(7): md5 = 83259d8e45aee1431ea73d153d0629e1
ffmpeg getFramesResampled(8): md5 = 078307f5265a3f21be58b8df1a3f78bd
ffmpeg getFramesResampled(9): md5 = 97fd9e6dfbdbadd82d7f84bea316d8ba
ffmpeg getFramesResampled(10): md5 = 1243df0dd76635fca119724c7a42d5ba
ffmpeg getFramesResampled(11): md5 = fc3e15db4fb283acb8b2cf00ee36caa3
ffmpeg getFramesResampled(12): md5 = b48a4388cfb45f07c0425f16c074fa71
ffmpeg getFramesResampled(13): md5 = f21eff364b20deb29ccaaf5d9e767431
ffmpeg getFramesResampled(14): md5 = f21eff364b20deb29ccaaf5d9e767431
ffmpeg getFramesResampled(15): md5 = 9d1fe6c95cf9b0bb026ab63e3e2700f9
ffmpeg getFramesResampled(16): md5 = 338a09373b80ea5a93ee42ac7c5f46cc
ffmpeg getFramesResampled(17): md5 = 707937e2eae3d04371fd8b0e7a0d9f47
ffmpeg getFramesResampled(18): md5 = 80b36ce6074a51ab82911ff5a4218b0e
ffmpeg getFramesResampled(19): md5 = 42929a153e0b982ffea55530160e7ad2
ffmpeg getFramesResampled(20): md5 = 42929a153e0b982ffea55530160e7ad2
ffmpeg getFramesResampled(21): md5 = bd7a9a923cf70ad4df4620b22ea2acee
ffmpeg getFramesResampled(22): md5 = 3a06111a4ae357768f76159ff241b176
ffmpeg getFramesResampled(23): md5 = 271e1683f15e5bac325506e9d57664fc
ffmpeg getFramesResampled(24): md5 = 4df9c058b922d4e09ee3a4c014efcc0c
ffmpeg getFramesResampled(25): md5 = e3d6b89acc79498525704162dc1781ba
ffmpeg getFramesResampled(26): md5 = e3d6b89acc79498525704162dc1781ba
ffmpeg getFramesResampled(27): md5 = 27455ab6aa9588503f1ac7b706812ba5
ffmpeg getFramesResampled(28): md5 = f06b286635acbbddfc1cfb886a4d898a
ffmpeg getFramesResampled(29): md5 = beba5cbbf803c5a2f99ee3dc24255a07
ffmpeg getFramesResampled(30): md5 = 525e979884f5b2af559efd34d833c0e5
ffmpeg getFramesResampled(31): md5 = f269c109507e80bb7480008fe469cfe4
ffmpeg getFramesResampled(32): md5 = ca729708d39cbbc03142083bd726ece3
ffmpeg getFramesResampled(33): md5 = cd6e5310f0cb6a8aadb099a651d14623
ffmpeg getFramesResampled(34): md5 = 182f919607cf1156626b348f7ecbc7e8
ffmpeg getFramesResampled(35): md5 = 6ef33f48705cc7031f7473e425e743d3
ffmpeg getFramesResampled(36): md5 = 9c04e0655ca5e4df27cbf1364c6deca6
ffmpeg getFramesResampled(37): md5 = 39f9062306d5b71732b7ba7b50836aaa
ffmpeg getFramesResampled(38): md5 = 39f9062306d5b71732b7ba7b50836aaa
ffmpeg getFramesResampled(39): md5 = eb3781d0e7dde371ac017835cd8374c3
ffmpeg getFramesResampled(40): md5 = 3abe4c6ca07fde4ab7a0e62b23662bb8
ffmpeg getFramesResampled(41): md5 = 4a832d3ad5a652bc5ad057683b89cf8e
ffmpeg getFramesResampled(42): md5 = 5d33fc4081b559e808ea648a4ec4b674
ffmpeg getFramesResampled(43): md5 = f56802b1ba257440a9e1c6f7c1513bf3
ffmpeg getFramesResampled(44): md5 = 2442e130eb284561f92662d9d64c63c3
ffmpeg getFramesResampled(45): md5 = 16e4f2cf543092e8307777aa1eda6089
ffmpeg getFramesResampled(46): md5 = d4192c71b9ffd3c0db43b2f17df47a90
ffmpeg getFramesResampled(47): md5 = 2738726354f93b27fa16166a487bae03
ffmpeg getFramesResampled(48): md5 = c1da78131b277b63409106e1b9cb87cd
ffmpeg getFramesResampled(49): md5 = f3efdaf1987d6bc1809bf0a18433131d
ffmpeg getFramesResampled(50): md5 = f3efdaf1987d6bc1809bf0a18433131d
ffmpeg getFramesResampled(51): md5 = 090a547436618fda2724d03dc6185fb8
ffmpeg getFramesResampled(52): md5 = d188f4ee69c77cb3f10e5397e296667d
ffmpeg getFramesResampled(53): md5 = 193c5c9552af06f25f979fb6e01524a8
ffmpeg getFramesResampled(54): md5 = e8d643f80f0fb261b9950f58e6cd4bd6
ffmpeg getFramesResampled(55): md5 = d27ed76a8fc3c11d1e72872a0404bc39
ffmpeg getFramesResampled(56): md5 = 5f948db7dc841e417a088ed9cc848481
ffmpeg getFramesResampled(57): md5 = 56d607f785be7d1b65076c1fa73aeef2
ffmpeg getFramesResampled(58): md5 = cf706f568c4eadc865b53ff64a86eae2
ffmpeg getFramesResampled(59): md5 = cf734c982aa6852718fbded35e705c1f
ffmpeg getFramesResampled(60): md5 = 5617b3b48e11c08b00419b7711b20a31
ffmpeg getFramesResampled(61): md5 = 0a2c09b42a667a0424b302c5b515f7bd
ffmpeg getFramesResampled(62): md5 = 0a2c09b42a667a0424b302c5b515f7bd
ffmpeg getFramesResampled(63): md5 = 9119f89adb46b0a5b7ee31d2d3aa582e
ffmpeg getFramesResampled(64): md5 = 1f28e252ac84a1f32955473160c833bc
ffmpeg getFramesResampled(65): md5 = 303b2499baee02f414510dd4e3038d57
ffmpeg getFramesResampled(66): md5 = aae2d7025b7b36961ac22676dd3a2c36
ffmpeg getFramesResampled(67): md5 = d0aeceaf94e899023402c5d4c2b4a0e2
ffmpeg getFramesResampled(68): md5 = 979c682f74f3965fe2d5dedd1b1a72db
ffmpeg getFramesResampled(69): md5 = b802a3922f625c30ac9ffefc349d76b7
ffmpeg getFramesResampled(70): md5 = 5a8f410ea6835dc72528a3a231ab24d0
ffmpeg getFramesResampled(71): md5 = df81dd0b5d27906041822816155423a5
ffmpeg getFramesResampled(72): md5 = 23166891bc1d1a1d626f435bb278f51a
ffmpeg getFramesResampled(73): md5 = 63a8b3eb3cb8c1dcb26c2de620c26c52
ffmpeg getFramesResampled(74): md5 = 63a8b3eb3cb8c1dcb26c2de620c26c52
ffmpeg getFramesResampled(75): md5 = f92b0e31a8be01f28fd182a328a16ad8
ffmpeg getFramesResampled(76): md5 = 16bc8e953fb9435245a4716905aeb56c
ffmpeg getFramesResampled(77): md5 = 9a63a5d326075521e127506701eae246
ffmpeg getFramesResampled(78): md5 = 637c11c25feb94b3a156018b0ea31498
ffmpeg getFramesResampled(79): md5 = 5cc679bae6a16508206835a8577fc7e1
ffmpeg getFramesResampled(80): md5 = 85b36968fbed29c6050bf96e64df3c6a
ffmpeg getFramesResampled(81): md5 = d8317019ec47cd8cafbd9442274d3d9d
ffmpeg getFramesResampled(82): md5 = 4ba7441ff1f7758403f068e0f4227c7a
ffmpeg getFramesResampled(83): md5 = 4689de6c8e15e57663ec79ec599c37af
ffmpeg getFramesResampled(84): md5 = 7405717d9f116f4397391145366924df
ffmpeg getFramesResampled(85): md5 = 95f0a5eea884df4aac5c806c0f6f8142
ffmpeg getFramesResampled(86): md5 = 95f0a5eea884df4aac5c806c0f6f8142
ffmpeg getFramesResampled(87): md5 = 726616e1a4e65f9b025e35f311da984d
ffmpeg getFramesResampled(88): md5 = 5a9381fc32047d104b7b075d966c0609
ffmpeg getFramesResampled(89): md5 = 1a049b614ed19f0cbeeb39488c45080a
ffmpeg getFramesResampled(90): md5 = c8c4da36c6436590a3924e531584d76e
ffmpeg getFramesResampled(91): md5 = e4c0e2aa4229bc5a96f4e27f31cb972f
ffmpeg getFramesResampled(92): md5 = f0df630c0833b00e5c5005414f5890bf
ffmpeg getFramesResampled(93): md5 = 45b6936dde5694a628b4c1fd74edc9cf
ffmpeg getFramesResampled(94): md5 = f2ecbb741f48268a295d0e853a366eda
ffmpeg getFramesResampled(95): md5 = e4bcb9cb252c9791eabb75463dac868c
ffmpeg getFramesResampled(96): md5 = 4ba2b8d407a891b0d0d8c94cb46199da
ffmpeg getFramesResampled(97): md5 = 06b9509b8874c7df152e8d195be16bb3
ffmpeg getFramesResampled(98): md5 = 06b9509b8874c7df152e8d195be16bb3
ffmpeg getFramesResampled(99): md5 = 058e2a41cdebdbf63eb2a4a6414eb611
ffmpeg getFramesResampled(100): md5 = 117a319007d9794bdf9c52c5687ef37f
ffmpeg getFramesResampled(101): md5 = 98f16d7f9f033a671ec3fd8a7ddac323
ffmpeg getFramesResampled(102): md5 = 0888ed81b2dc00100e53d6cbd44d6023
ffmpeg getFramesResampled(103): md5 = 646310c7993e482dc2ccf580f7088e48
ffmpeg getFramesResampled(104): md5 = e5056128cdc2616cf2a1bd2941342bc6
ffmpeg getFramesResampled(105): md5 = c2f2e5d92937d8582d582c12f487fdf0
ffmpeg getFramesResampled(106): md5 = c04d092a8125f743bc851f11bd4ef4ff
ffmpeg getFramesResampled(107): md5 = 161018c2fd58036c24ed5727336a218b
ffmpeg getFramesResampled(108): md5 = 85ea0b26c82bc43c8198ef2e9630fec2
ffmpeg getFramesResampled(109): md5 = d7b10cb87f5b0de9a3c1a37fbded8d5c
ffmpeg getFramesResampled(110): md5 = d7b10cb87f5b0de9a3c1a37fbded8d5c
ffmpeg getFramesResampled(111): md5 = f1dae98b372b48bdba0e62e12549c254
ffmpeg getFramesResampled(112): md5 = e4a2f561862478aff7a002c21b44ee0f
ffmpeg getFramesResampled(113): md5 = 57c6358e6fb226e433ced7b56c502a2a
ffmpeg getFramesResampled(114): md5 = 8c489aceba6ca7f18fc555cb6b3ecc42
ffmpeg getFramesResampled(115): md5 = 4f6858d076e0a9bf8413a72bfef5ef57
ffmpeg getFramesResampled(116): md5 = 93c5df2223289acd9abac00368975fb2
ffmpeg getFramesResampled(117): md5 = 667083b7ae373253557fd0d218897ffe
ffmpeg getFramesResampled(118): md5 = 543b5f150e52bbde42f808547c0f663b
ffmpeg getFramesResampled(119): md5 = 8741d4703cefe17bdb783d972c4750e5
ffmpeg getFramesResampled(120): md5 = 81bf67da18e9356bb19f57d717f83114
