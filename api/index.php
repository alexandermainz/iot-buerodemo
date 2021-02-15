<?php
require_once "../_config.inc.php";

use Respect\Rest\Router;
$r3 = new Router;

$r3->get('/whereami', function() {
    header("Cache-Control:max-age=30");
    $db = new PDO("sqlite:/www/htdocs/w018f2b5/net-things.de/thingsdb.sqlite");
    $result = $db->query('SELECT "text" FROM "status" WHERE id=1');
    if ($result !== FALSE)
        return $result->fetchColumn();
    else
        return "";
})->accept(array('text/plain' => function($data) {
    header('Content-length: '.strlen($data));
    return $data;
}));

$r3->post('/whereami', function() {
    $val = substr(trim(stripslashes(file_get_contents('php://input'))), 0, 50);
    $db = new PDO("sqlite:/www/htdocs/w018f2b5/net-things.de/thingsdb.sqlite");
    $query = 'UPDATE "status" SET "text" = :val WHERE id=1';
    $stmt = $db->prepare($query);
    $stmt->bindValue(':val', $val);
    if ($stmt->execute())
        http_response_code(200);
    else 
        http_response_code(500);
});

$r3->get('/usermessage', function() {
    header("Cache-Control:max-age=30");
    $db = new PDO("sqlite:/www/htdocs/w018f2b5/net-things.de/thingsdb.sqlite");
    $result = $db->query('SELECT "text" FROM "status" WHERE id=2');
    if ($result !== FALSE)
        return $result->fetchColumn();
    else
        return "";
})->accept(array('text/plain' => function($data) {
    header('Content-length: '.strlen($data));
    return $data;
}));

$r3->post('/leavemessage', function() {
    header('Access-Control-Allow-Origin: *');
    $val = substr(trim(stripslashes(file_get_contents('php://input'))), 0, 42);
    $db = new PDO("sqlite:/www/htdocs/w018f2b5/net-things.de/thingsdb.sqlite");
    $query = 'UPDATE "status" SET "text" = :val WHERE id=2';
    $stmt = $db->prepare($query);
    $stmt->bindValue(':val', $val);
    if ($stmt->execute())
        http_response_code(200);
    else 
        http_response_code(500);
});
?>
