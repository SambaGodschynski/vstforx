<?php 
/**
* MantisBT Core API's
*/
	require_once( 'core.php' );

	require_once( 'current_user_api.php' );
	/*require_once( 'news_api.php' );
	require_once( 'date_api.php' );
	require_once( 'print_api.php' );
	require_once( 'rss_api.php' );
	require_once( 'database_api.php' );*/

	access_ensure_project_level( REPORTER );

$path = '../binary/';

$file = $_GET['file'];
if ( !isset($file) || !file_exists($path.$file) ) {
    echo $file. " not found!";
    die();
}

//$name = "dateiname.zip"; // Name, unter welchem die Datei an den Browser geschickt wird

header("Content-type: application/octet-stream");

// Anstatt "attachment" kann auch "inline" gewählt werden, damit der Browser

// die Datei normal darstellen kann und nicht immer der "Speichern unter"-Dialog erscheint

header("Content-disposition: inline; filename=".$file);

header("Content-Length: ".filesize( $path.$file ) );

readfile( $path.$file );

?>


