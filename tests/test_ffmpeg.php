<?
$module = 'ffmpeg';

// load extension
if(!extension_loaded($module))
    dl($module . "." . PHP_SHLIB_SUFFIX);

if (!extension_loaded($module))
    die("Module $module is not compiled into PHP");

// print available functions
$functions = get_extension_funcs($module);
echo "Functions available in the $module extension:<br>\n";

foreach($functions as $func)
    echo $func."<br>\n";

// put some movie files into this array to test the ffmpeg functions
$movies = array( /*"/home/tkirby/movies/cowbell.wmv", */);

echo "--------------------\n\n";
foreach($movies as $movie) {        
    $mov = ffmpeg_movie($movie);
    printf("file name = %s\n", $mov->getFileName());
    printf("duration = %s\n", $mov->getDuration());
    printf("frame count = %s\n", $mov->getFrameCount());
    printf("frame rate = %s\n", $mov->getFrameRate());
    printf("comment = %s\n", $mov->getComment());
    printf("title = %s\n", $mov->getTitle());
    printf("author = %s\n", $mov->getAuthor());
    printf("copyright = %s\n", $mov->getCopyright());
// BROKEN    $mov->getFrame(1);
    echo "\n--------------------\n\n";
}
?>
