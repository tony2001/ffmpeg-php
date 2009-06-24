<?
/*
 * This test script is not part of the automatic regression tests. It serves
 * as a simple manual test script and an example of the syntax for calling
 * the ffmpeg-php functions
 * 
 * To run it from the command line type 'php -q ffmpeg_test.php 'or from a 
 * browser * copy this file into your web root and point your browser at it.
 */

ob_start();

echo '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">';
echo '<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">';
echo '<head>';
echo '<title>Ffmpeg-php Extension Test Page</title>';
echo '<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />';
echo '<style type="text/css">';
echo '

.hor-minimalist-a
{
	font-family: "Lucida Sans Unicode", "Lucida Grande", Sans-Serif;
	font-size: 12px;
	background: #fff;
	margin: 45px;
	width: 480px;
	border-collapse: collapse;
	text-align: left;
}


.hor-minimalist-a caption { font-size: 16px; font-weight: bold; color: #039; padding: 10px 8px; }
.hor-minimalist-a th { font-size: 14px; font-weight: normal; color: #039; padding: 10px 8px;
	border-bottom: 2px solid #6678b1;
}

.hor-minimalist-a td { color: #669; padding: 9px 8px 0px 8px; }
.hor-minimalist-a tbody tr:hover td { color: #009; }';

echo '</style>';
echo '</head>';
echo '<body><div align="center">';
echo '<table width="340" class="hor-minimalist-a">';
echo "<tr><th>Ffmpeg-php Extension Version Info<br/></th></tr>";
echo "<tr><td>";
printf("Ffmpeg-php version string: %s<br/>", FFMPEG_PHP_VERSION_STRING);
printf("libavcodec build number: %d<br/>", LIBAVCODEC_BUILD_NUMBER);
printf("libavcodec version number: %d<br/>", LIBAVCODEC_VERSION_NUMBER);
printf("libavcodec build number: %d<br/>", LIBAVCODEC_BUILD_NUMBER);
echo "</td></tr></table>";

echo "<p/>";
print_class_methods("ffmpeg_movie");
echo "<p/>";
print_class_methods("ffmpeg_frame");

// get an array for movies from the test media directory 
$movies = getDirFiles(dirname(__FILE__) . '/tests/test_media');

$i = 1;
foreach($movies as $movie) {        
    $mov = new ffmpeg_movie($movie);

    echo '<table width="90%" class="hor-minimalist-a">';
    printf('<caption>Processing Test File: %s...</caption>', basename($mov->getFilename()));
    printf('<thead><tr><th width="150">Method</th><th>Result<br/></th></tr></thead>', $mov->getFileName());
    printf("<tr><td>file name:</td><td>%s<br/></td></tr>", $mov->getFileName());
    printf("<tr><td>duration</td><td>%s seconds<br/></td></tr>", $mov->getDuration());
    printf("<tr><td>frame count</td><td>%s<br/></td></tr>", $mov->getFrameCount());
    printf("<tr><td>frame rate</td><td>%0.3f fps<br/></td></tr>", $mov->getFrameRate());
    printf("<tr><td>comment</td><td>%s<br/></td></tr>", $mov->getComment());
    printf("<tr><td>title</td><td>%s<br/></td></tr>", $mov->getTitle());
    printf("<tr><td>author</td><td>%s<br/></td></tr>", $mov->getAuthor());
    printf("<tr><td>copyright</td><td>%s<br/></td></tr>", $mov->getCopyright());
    printf("<tr><td>get bit rate</td><td>%d<br/></td></tr>", $mov->getBitRate());
    printf("<tr><td>has audio</td><td>%s<br/></td></tr>", $mov->hasAudio() == 0 ? 'No' : 'Yes');
    if ($mov->hasAudio()) {
        printf("<tr><td>get audio stream id</td><td>%s<br/></td></tr>", $mov->getAudioStreamId());
        printf("<tr><td>get audio codec</td><td>%s<br/></td></tr>", $mov->getAudioCodec());
        printf("<tr><td>get audio bit rate</td><td>%d<br/></td></tr>", $mov->getAudioBitRate());
        printf("<tr><td>get audio sample rate</td><td>%d<br/></td></tr>", $mov->getAudioSampleRate());
        printf("<tr><td>get audio channels</td><td>%s<br/></td></tr>", $mov->getAudioChannels());
    }
    printf("<tr><td>has video</td><td>%s<br/></td></tr>", $mov->hasVideo() == 0 ? 'No' : 'Yes');
    if ($mov->hasVideo()) {
        printf("<tr><td>frame height</td><td>%d pixels<br/></td></tr>", $mov->getFrameHeight());
        printf("<tr><td>frame width</td><td>%d pixels<br/></td></tr>", $mov->getFrameWidth());
        printf("<tr><td>get video stream id</td><td>%s<br/></td></tr>", $mov->getVideoStreamId());
        printf("<tr><td>get video codec</td><td>%s<br/></td></tr>", $mov->getVideoCodec());
        printf("<tr><td>get video bit rate</td><td>%d<br/></td></tr>", $mov->getVideoBitRate());
        printf("<tr><td>get pixel format</td><td>%s<br/></td></tr>", $mov->getPixelFormat());
        printf("<tr><td>get pixel aspect ratio</td><td>%s<br/></td></tr>", $mov->getPixelAspectRatio());
        printf("<tr><td>get frame</td><td>%s<br/></td></tr>", is_object($mov->getFrame(10)) ? 'true' : 'false');
        printf("<tr><td>get frame number</td><td>%d<br/></td></tr>", $mov->getFrameNumber());
	$thumbpath = "$i.png";
	if (make_test_thumbnail(rand(1,100), $mov->getFilename(), $thumbpath)) {
		printf('<tr><td>Random Thumbnail</td><td><img alt="Test Image" src="%s"/></td></tr>', $thumbpath);
	}
    }
    
    echo "</table>";
    echo "<p/><p/>";
    $i++;
}

echo '</div></body></html>';

if (php_sapi_name() == 'cli') {
	$content = ob_get_clean();
	$content = br2nl($content);
	$content = strip_tags($content);
	echo $content;
} else {
	ob_end_flush();
}

function make_test_thumbnail($frame, $input, $outpath) {

	$mov = new ffmpeg_movie($input);
	$ff_frame = $mov->getFrame($frame);
	if ($ff_frame) {
		$gd_image = $ff_frame->toGDImage();
		if ($gd_image) {
			imagepng($gd_image, $outpath);
			imagedestroy($gd_image);
		} else {
			return false;
		}
	} else {
		return false;
	}
	return true;
}


function print_class_methods($class) {
	$methods = get_class_methods($class);
	echo '<table width="340" class="hor-minimalist-a">';
	echo "<thead><tr><th>Methods available in class '$class'</th></tr></thead>";
	if (is_array($methods)) {
		foreach($methods as $method) {
			echo "<tr><td>$method<br/></td></tr>";
		}
	} else {
		echo "<tr><td>No Methods Defined<br/></td></tr>";
	}
	echo "</table>";
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

function br2nl($string) {
    return preg_replace('/\<br(\s*)?\/?\>/i', "\n", $string);
}

?>
