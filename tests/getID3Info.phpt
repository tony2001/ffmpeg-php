--TEST--
ffmpeg getFileName test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie('test_media/Ballad of the Sneak.mp3');
printf("ffmpeg getTitle(): %s\n", $mov->getTitle());
printf("ffmpeg getArtist(): %s\n", $mov->getArtist());
printf("ffmpeg getAlbum(): %s\n", $mov->getAlbum());
printf("ffmpeg getGenre(): %s\n", $mov->getGenre());
printf("ffmpeg getTrackNumber(): %s\n", $mov->getTrackNumber());
printf("ffmpeg getYear(): %s\n", $mov->getYear());

?>
--EXPECT--
ffmpeg getTitle(): Ballad of the Sneak
ffmpeg getArtist(): DaVinci's Notebook
ffmpeg getAlbum(): Strong Bad Sings
ffmpeg getGenre(): Ballad
ffmpeg getTrackNumber(): 1
ffmpeg getYear(): 2004

