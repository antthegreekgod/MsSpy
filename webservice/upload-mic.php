<?php
$targetDir = '/var/www/imgCapture/';

if (!isset($_FILES['audio']) || $_FILES['audio']['error'] !== UPLOAD_ERR_OK) {
    http_response_code(400);
    exit;
}

$filename = basename($_FILES['audio']['name']);
$targetFile = $targetDir . $filename;

if (file_put_contents($targetFile, file_get_contents($_FILES['audio']['tmp_name'])) !== false) {
    http_response_code(200);
} else {
    http_response_code(500);
}
?>
