<?
/*
 * This test script is not part of the automatic regression tests. It serves
 * as a simple manual test script and an example of the syntax for calling
 * the ffmpeg-php functions
 * 
 * To run it from the command line type 'php -q ffmpeg_test.php'or from a 
 * browser * copy this file into your web root and point your browser at it.
 */

$extension = 'ffmpeg';
$extension_soname = $extension . '.' . PHP_SHLIB_SUFFIX;
$extension_fullname = PHP_EXTENSION_DIR . '.' . $extension_soname;

if (php_sapi_name() != 'cgi') {
    echo '<pre>';
}

// load extension
if(!extension_loaded($extension)) {
    dl($extension_soname) or die("Can't load extension $extension_fullname");
}

// print available functions and aliases
echo "\nFunctions available in $extension_fullname extension:\n";
foreach(get_extension_funcs($extension) as $func) {
    echo $func."\n";
}

$class = "ffmpeg_movie";
echo "\nMethods available in class $class:\n";
foreach(get_class_methods($class) as $method) {
    echo $method."\n";
}

// put some movie files into this array to test the ffmpeg functions
$movies[] = "./test.avi"; // movie with sound

echo "--------------------\n\n";
foreach($movies as $movie) {        
    $mov = new ffmpeg_movie($movie);
    printf("file name = %s\n", $mov->getFileName());
    printf("duration = %s\n", $mov->getDuration());
    printf("frame count = %s\n", $mov->getFrameCount());
    printf("frame rate = %s\n", $mov->getFrameRate());
    printf("comment = %s\n", $mov->getComment());
    printf("title = %s\n", $mov->getTitle());
    printf("author = %s\n", $mov->getAuthor());
    printf("copyright = %s\n", $mov->getCopyright());
//    printf("has video = %s\n", $mov->hasVideo());
//    printf("has audio = %s\n", $mov->hasAudio());
//    printf("get frame = %d\n", $mov->getFrame(1));
    echo "\n--------------------\n\n";
}

if (php_sapi_name() != 'cgi') {
    echo '</pre>';
}

?>
