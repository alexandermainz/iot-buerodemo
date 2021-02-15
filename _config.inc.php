<?php
require("vendor/autoload.php");

// Datenbank einmalig initialieren
$db = new PDO("sqlite:/www/htdocs/w018f2b5/net-things.de/thingsdb.sqlite");
$db->exec('CREATE TABLE IF NOT EXISTS "status"	(
                        id		int PRIMARY KEY,
                        text	varchar(50),
                        timestamp	datetime
                        )' );
$db->exec('INSERT INTO "status" (id, "text") VALUES (1, \'Unterwegs\')');
$db->exec('INSERT INTO "status" (id, "text") VALUES (2, \'(keine Nachricht)\')');
$db = null;
?>