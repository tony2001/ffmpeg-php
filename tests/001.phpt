--TEST--
ffmpeg test
--SKIPIF--
<?php 

$extension = 'ffmpeg';
$extension_soname = $extension . '.' . PHP_SHLIB_SUFFIX;
$extension_fullname = PHP_EXTENSION_DIR . '.' . $extension_soname;

// load extension
if(!extension_loaded($extension)) {
    dl($extension_soname) or die("Extension $extension is not compiled into PHP\n");
}
?>
--FILE--
<?php

echo "ffmpeg test: ";
$class = "ffmpeg_movie";
$methods = get_class_methods($class) or die("Can't get functions");
echo is_array($methods) ? 'ok' : 'failed';
echo "\n";

?>
--EXPECT--
ffmpeg test: ok
