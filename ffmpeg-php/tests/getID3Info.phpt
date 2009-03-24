--TEST--
ffmpeg getID3Info() test
--SKIPIF--
<?php extension_loaded('ffmpeg') or die("ffmpeg extension not loaded"); ?>
--FILE--
<?php
$mov = new ffmpeg_movie(dirname(__FILE__) . '/test_media/16Hz-20kHz-Exp-1f-5sec.mp3');
printf("ffmpeg getTitle(): %s\n", $mov->getTitle());
printf("ffmpeg getArtist(): %s\n", $mov->getArtist());
printf("ffmpeg getAlbum(): %s\n", $mov->getAlbum());
printf("ffmpeg getGenre(): %s\n", $mov->getGenre());
printf("ffmpeg getTrackNumber(): %s\n", $mov->getTrackNumber());
printf("ffmpeg getYear(): %s\n", $mov->getYear());

?>
--EXPECT--
ffmpeg getTitle(): Test mp3
ffmpeg getArtist(): Alexander Thomas
ffmpeg getAlbum(): MP3 test files (http://www.dr-lex.be/software/testsounds.html#Tones)
ffmpeg getGenre(): (37)
ffmpeg getTrackNumber(): 3
ffmpeg getYear(): 2008
