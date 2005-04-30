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
    $i++;
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
ffmpeg getFrame(2): md5 = 5efa07538642208bd6df33a512a0e9f7
ffmpeg getFrame(3): md5 = 2c1dfba56179a9d2cec28ef0679f1563
ffmpeg getFrame(4): md5 = 2c1dfba56179a9d2cec28ef0679f1563
ffmpeg getFrame(5): md5 = 69cbad22cbf2b18beea30246adf0693e
ffmpeg getFrame(6): md5 = 4508be090c7349c59857f9a2cf029695
ffmpeg getFrame(7): md5 = 9a96dd6da18a8f3100d618f80e1b3186
ffmpeg getFrame(8): md5 = f240d484dc39a760bc8e0e6c38376c1f
ffmpeg getFrame(9): md5 = 91aa0e12cef1b562c500e5ad7b6ec639
ffmpeg getFrame(10): md5 = 38e441e98499a78995dbe58abfa76c15
ffmpeg getFrame(11): md5 = 33057011e3a1c70f9638d0e4170ec87f
ffmpeg getFrame(12): md5 = 815e75b73cf40fd6a428e2f9fd6c1aaa
ffmpeg getFrame(13): md5 = 8527383d8556260d581e7ccd09a478a7
ffmpeg getFrame(14): md5 = 8527383d8556260d581e7ccd09a478a7
ffmpeg getFrame(15): md5 = ccdf9253b96cb1a7baef835d3aed3f8b
ffmpeg getFrame(16): md5 = dbbd6a7ff35f518c69c4cd2855e15e21
ffmpeg getFrame(17): md5 = 364590e3862133488bd1334405cd5571
ffmpeg getFrame(18): md5 = d4ead99a338ea7898194714af9b8d245
ffmpeg getFrame(19): md5 = 8dfb30242dcee833980d3668232c3d3f
ffmpeg getFrame(20): md5 = 8dfb30242dcee833980d3668232c3d3f
ffmpeg getFrame(21): md5 = eff7e8b45406c78f1ff99eb87da6cbbe
ffmpeg getFrame(22): md5 = 89d380825713d1c4f2de5d19b671d359
ffmpeg getFrame(23): md5 = fb76abac8e2a48dfd2a27ba25b8b5a44
ffmpeg getFrame(24): md5 = 8a15626ca764dbb9aa01836596ddec25
ffmpeg getFrame(25): md5 = 1ac1036ac0bc153f9cd1955a5b7a733a
ffmpeg getFrame(26): md5 = 1ac1036ac0bc153f9cd1955a5b7a733a
ffmpeg getFrame(27): md5 = cff95ac7136e05d7c010ce7d5de5745e
ffmpeg getFrame(28): md5 = 8a3f1c5711db5ee9c1953c291d70ed9b
ffmpeg getFrame(29): md5 = ebd55e4bc9e851c9273166291e15c4e6
ffmpeg getFrame(30): md5 = cd84c8e1b99d9d7b3e6b68351e4baf65
ffmpeg getFrame(31): md5 = 972f5a3b8e81d9403caecc18ff03d2dc
ffmpeg getFrame(32): md5 = 0d2d11ed87daa1115040ed8bd7f9cccd
ffmpeg getFrame(33): md5 = dc02c2c37dec799950456630e7f9161f
ffmpeg getFrame(34): md5 = 53d925ebba138afa40d11f360dbdb563
ffmpeg getFrame(35): md5 = e4b5ad0b29f87370a458c559eed23503
ffmpeg getFrame(36): md5 = 68d6f04550b1951bb6906896736e3a97
ffmpeg getFrame(37): md5 = 893d3c696720b727445723f706337b53
ffmpeg getFrame(38): md5 = 893d3c696720b727445723f706337b53
ffmpeg getFrame(39): md5 = 10108cbfe702198e8ef8b1a3d3243976
ffmpeg getFrame(40): md5 = 44918b85d8619f3b3e7c8a7620a66875
ffmpeg getFrame(41): md5 = 5f26f0ed33259e3fdd8c5397a73ed504
ffmpeg getFrame(42): md5 = 53a7c6823faf0a20f8995109b76506f2
ffmpeg getFrame(43): md5 = cfb2640982bf6047f2e3a1754340816c
ffmpeg getFrame(44): md5 = feb31b7658c80aa4995127d734340f09
ffmpeg getFrame(45): md5 = 07ac7bfe4d5427ba2d609e529d3c447e
ffmpeg getFrame(46): md5 = fbe25c62ffe368a892a0c89bd6c91080
ffmpeg getFrame(47): md5 = 264bc002eabfce162661ab7c1ff2a26d
ffmpeg getFrame(48): md5 = 48ea88c344d2267c6576fc60b0591726
ffmpeg getFrame(49): md5 = 987c6513ae5ec608be5d335841bd1c66
ffmpeg getFrame(50): md5 = 987c6513ae5ec608be5d335841bd1c66
ffmpeg getFrame(51): md5 = 4e65b1889bfa03cc96aa5741db43f335
ffmpeg getFrame(52): md5 = b3b9664da81c13f4242babccf6be907c
ffmpeg getFrame(53): md5 = 18c9cb92e2064c0ad6b1ec657c236ec3
ffmpeg getFrame(54): md5 = 1a94f6d7645560b4f649596c911f4682
ffmpeg getFrame(55): md5 = 0ec4c8393dedacb0f9583c67b988d62b
ffmpeg getFrame(56): md5 = f8ed6df0da46286b00b9d46cc30f04c1
ffmpeg getFrame(57): md5 = 1f306b368dc3ffa4991e665f39138615
ffmpeg getFrame(58): md5 = 16d473aa1bc37274974abfa858ba6d08
ffmpeg getFrame(59): md5 = d98118837bcd3c1fbd137be58d187d6e
ffmpeg getFrame(60): md5 = 3e5fb3f0cfe8c3ba793fb9419843473c
ffmpeg getFrame(61): md5 = f0285b52f4057cba94e34001fd84aa4c
ffmpeg getFrame(62): md5 = f0285b52f4057cba94e34001fd84aa4c
ffmpeg getFrame(63): md5 = 5c0233f0bbcb9342d75dc61c43826696
ffmpeg getFrame(64): md5 = 6df63a2facd2778b1939fd89927e19f4
ffmpeg getFrame(65): md5 = f6c0f7df37077826950b9294edb043bc
ffmpeg getFrame(66): md5 = 86b4e606dd912516111b75aa959e338f
ffmpeg getFrame(67): md5 = 34154f7445a23eb579744f19858cfc2b
ffmpeg getFrame(68): md5 = 02b2917df92cd5d3cbf3f740f941d337
ffmpeg getFrame(69): md5 = 896533b553478786fee791f111c6396b
ffmpeg getFrame(70): md5 = ea3ea32dc7e59db73b51ac5208bf0cf4
ffmpeg getFrame(71): md5 = 5b88d65752e9f89c2c303f8c8a1a9ae4
ffmpeg getFrame(72): md5 = 4b1164133c7e1c97280fa043524c5ccd
ffmpeg getFrame(73): md5 = cc36d30fb5b269146802e740ce4ab239
ffmpeg getFrame(74): md5 = cc36d30fb5b269146802e740ce4ab239
ffmpeg getFrame(75): md5 = 34d4b79a38cf56fa8cc599533aaae346
ffmpeg getFrame(76): md5 = ce439fdc43c8a818188dc57d08395119
ffmpeg getFrame(77): md5 = 2da9f41572b319f51e64888cf4ce8dd4
ffmpeg getFrame(78): md5 = c3ba41e9be47fa1b3b784c34d3daa0e7
ffmpeg getFrame(79): md5 = a2402e9ed98169bcec4da4ac0e5ce718
ffmpeg getFrame(80): md5 = bb49cbb007581d628dd81fb152b1b9c5
ffmpeg getFrame(81): md5 = 83fcb36e27706034a9bdb942673d6d65
ffmpeg getFrame(82): md5 = 512faeb9492900a6ec8fe7df61966867
ffmpeg getFrame(83): md5 = cf05a3eb6fb0546d421af2498b2bad14
ffmpeg getFrame(84): md5 = 6c95336879fe36999d15ad54698be8db
ffmpeg getFrame(85): md5 = 264250f22a7a4f374653576f56d32a51
ffmpeg getFrame(86): md5 = 264250f22a7a4f374653576f56d32a51
ffmpeg getFrame(87): md5 = a337f04cdcfd3d3eccb0eeb9baaf9098
ffmpeg getFrame(88): md5 = e773283788c9146adf2243e49c4097c2
ffmpeg getFrame(89): md5 = 8ffac9c0a475cbc5cbb9c7bdc47847ef
ffmpeg getFrame(90): md5 = 06113996a89ffa3602d5dcc8b7d9fc42
ffmpeg getFrame(91): md5 = 740652b50fc8c95a8dba2b56ca434a5a
ffmpeg getFrame(92): md5 = ca7036c174e03f2842126c6b9e57b109
ffmpeg getFrame(93): md5 = 06444c47932cdcdd0771a0a0bd291e4f
ffmpeg getFrame(94): md5 = fb50ae5f62fb4fd2a6cefc555198c41c
ffmpeg getFrame(95): md5 = 991f9f53db08c1c04a6d3cecd93e16dc
ffmpeg getFrame(96): md5 = 727c895de8483f15182cee76ca4d3e0d
ffmpeg getFrame(97): md5 = ccae0d5e5a99a455276abc314bbad69e
ffmpeg getFrame(98): md5 = ccae0d5e5a99a455276abc314bbad69e
ffmpeg getFrame(99): md5 = 92e90a2d700fbf2d0aeeff5da513c217
ffmpeg getFrame(100): md5 = fd994cf3d579024b9a45de7e81c4e464
ffmpeg getFrame(101): md5 = ffab66359626076d43f2d12361d3f119
ffmpeg getFrame(102): md5 = e58219529e296cbddd272531ba21c647
ffmpeg getFrame(103): md5 = 1a81dddcc28bd1b1024f156db85a4e07
ffmpeg getFrame(104): md5 = 238889da46c637b7941a8f07b8095006
ffmpeg getFrame(105): md5 = 760f6e4bf6b511737f52ce405a2700ce
ffmpeg getFrame(106): md5 = 9c51d6b1936d18a29166766f3496451f
ffmpeg getFrame(107): md5 = 7f9c88905d5cca85b295a4a6cf0f02c8
ffmpeg getFrame(108): md5 = 17e54042ea339bcc5d72c3d98203e061
ffmpeg getFrame(109): md5 = c4a8f5f3839380d6f159f2ecb15d6059
ffmpeg getFrame(110): md5 = c4a8f5f3839380d6f159f2ecb15d6059
ffmpeg getFrame(111): md5 = d88620f6f3842e9d77923a616c167edf
ffmpeg getFrame(112): md5 = d3ff2bb7210d2997b3792f4c80d8792b
ffmpeg getFrame(113): md5 = efee392dbfbec70b69d33abf782d02d9
ffmpeg getFrame(114): md5 = 97b779a61be1beffa1f182c3f86461d6
ffmpeg getFrame(115): md5 = c0b2e4aa091641ca2e92a5657516dee0
ffmpeg getFrame(116): md5 = 4b52fed557f598057c46e2d58648ebd3
ffmpeg getFrame(117): md5 = 092e487a7849b2ecf6e25eca534b028d
ffmpeg getFrame(118): md5 = 02042be68e3ff390283b2f962e8e36f6
ffmpeg getFrame(119): md5 = c2b25f297d23671bf65b4eba268b202b
ffmpeg getFrame(120): md5 = fa20e8347182e121b60f6e8b8c30561a
