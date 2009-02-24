<?
/*
 * This test script is not part of the automatic regression tests. It serves
 * as a simple manual test script and an example of the syntax for calling
 * the ffmpeg-php functions
 * 
 * To run it from the command line type 'php -q ffmpeg_test.php ' or from a 
 * browser copy this file into your web root and point your browser at it.
 */

$extension = "ffmpeg";
$extension_soname = $extension . "." . PHP_SHLIB_SUFFIX;
$extension_fullname = PHP_EXTENSION_DIR . "/" . $extension_soname;

// load extension
if (!extension_loaded($extension)) {
    dl($extension_soname) or die("Can't load extension $extension_fullname\n");
}

if (php_sapi_name() != 'cli') {
    echo '<pre>';
}

printf("ffmpeg-php version string: %s\n", FFMPEG_PHP_VERSION_STRING);
printf("ffmpeg-php build date string: %s\n", FFMPEG_PHP_BUILD_DATE_STRING);
printf("libavcodec build number: %d\n", LIBAVCODEC_BUILD_NUMBER);
printf("libavcodec version number: %d\n", LIBAVCODEC_VERSION_NUMBER);

print_class_methods("ffmpeg_movie");
print_class_methods("ffmpeg_frame");

// get an array for movies from the test media directory 
$movies = getDirFiles(dirname(__FILE__) . '/tests/test_media');

echo "--------------------\n\n";
foreach($movies as $movie) {        
    $mov = new ffmpeg_movie($movie);
    printf("file name = %s\n", $mov->getFileName());
    printf("duration = %s seconds\n", $mov->getDuration());
    printf("frame count = %s\n", $mov->getFrameCount());
    printf("frame rate = %0.3f fps\n", $mov->getFrameRate());
    printf("comment = %s\n", $mov->getComment());
    printf("title = %s\n", $mov->getTitle());
    printf("author = %s\n", $mov->getAuthor());
    printf("copyright = %s\n", $mov->getCopyright());
    printf("get bit rate = %d\n", $mov->getBitRate());
    printf("has audio = %s\n", $mov->hasAudio() == 0 ? 'No' : 'Yes');
    if ($mov->hasAudio()) {
        printf("get audio stream id= %s\n", $mov->getAudioStreamId());
        printf("get audio codec = %s\n", $mov->getAudioCodec());
        printf("get audio bit rate = %d\n", $mov->getAudioBitRate());
        printf("get audio sample rate = %d \n", $mov->getAudioSampleRate());
        printf("get audio channels = %s\n", $mov->getAudioChannels());
    }
    printf("has video = %s\n", $mov->hasVideo() == 0 ? 'No' : 'Yes');
    if ($mov->hasVideo()) {
        printf("frame height = %d pixels\n", $mov->getFrameHeight());
        printf("frame width = %d pixels\n", $mov->getFrameWidth());
        printf("get video stream id= %s\n", $mov->getVideoStreamId());
        printf("get video codec = %s\n", $mov->getVideoCodec());
        printf("get video bit rate = %d\n", $mov->getVideoBitRate());
        printf("get pixel format = %s\n", $mov->getPixelFormat());
        printf("get pixel aspect ratio = %s\n", $mov->getPixelAspectRatio());
        $frame = $mov->getFrame(10);
        printf("get frame = %s\n", is_object($frame) ? 'true' : 'false');
        printf("  get frame number = %d\n", $mov->getFrameNumber());
        printf("  get frame width = %d\n", $frame->getWidth());
        printf("  get frame height = %d\n", $frame->getHeight());
    }
    echo "\n--------------------\n\n";
}

if (php_sapi_name() != 'cli') {
    echo '</pre>';
}

/* FUNCTIONS */
function print_class_methods($class) {
    echo "\nMethods available in class '$class':\n";
    $methods = get_class_methods($class);
    if (is_array($methods)) {
        foreach($methods as $method) {
            echo $method . "\n";
        }
    } else {
        echo "No Methods Defined\n";
    }
}

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


?>
