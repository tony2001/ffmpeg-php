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


function getDirFiles($dirPath)
{
    if ($handle = opendir($dirPath))
    {
        while (false !== ($file = readdir($handle))) {
            $fullpath = $dirPath . '/' . $file;
            if (!is_dir($fullpath) && $file != "CVS" && $file != "." && $file != "..")
                $filesArr[] = trim($fullpath);
        }
        closedir($handle);
    } 

    return $filesArr;   
}


if (php_sapi_name() != 'cgi') {
    echo '<pre>';
}

// load extension
if(!extension_loaded($extension)) {
    dl($extension_soname) or die("Can't load extension $extension_fullname\n");
}

// print available functions and aliases
printf("libavcodec version number: %d\n", LIBAVCODEC_VERSION_NUMBER);
printf("libavcodec build number: %d\n", LIBAVCODEC_BUILD_NUMBER);

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
$movies = getDirFiles(dirname(__FILE__) . '/test_media');

echo "--------------------\n\n";
foreach($movies as $movie) {        
    $mov = new ffmpeg_movie($movie);
    printf("file name = %s\n", $mov->get_filename());
    printf("duration = %s seconds\n", $mov->get_duration());
    printf("frame count = %s\n", $mov->get_framecount());
    printf("frame rate = %0.3f fps\n", $mov->get_framerate());
    printf("comment = %s\n", $mov->get_comment());
    printf("title = %s\n", $mov->get_title());
    printf("author = %s\n", $mov->get_author());
    printf("copyright = %s\n", $mov->get_copyright());
    printf("frame height = %d pixels\n", $mov->get_frameheight());
    printf("frame width = %d pixels\n", $mov->get_framewidth());
    printf("has audio = %s\n", $mov->has_audio() == 0 ? 'No' : 'Yes');
    printf("get pixel format = %s\n", $mov->get_pixelformat());
    printf("get video bit rate = %d kb/s\n", $mov->get_video_bitrate());
/*    
    while (1) {
        $frame = $mov->getFrame();
        if (!is_resource($frame)) {
            break;
        }
        echo "get frame() $frame" . "\n";
    }
 */    
    printf("get frame = %s\n", $mov->get_frame(10));
    printf("get frame number = %d\n", $mov->get_framenumber());
    echo "\n--------------------\n\n";
}

if (php_sapi_name() != 'cgi') {
    echo '</pre>';
}

?>
