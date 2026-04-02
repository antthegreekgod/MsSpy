<?php
$data = json_decode(file_get_contents("php://input"), true);

// Extract img b64 data and filename
$imageBase64 = $data['image'];
$filename = $data['filename'];

// Remove "data:image/png;base64,"
$base64String = preg_replace('#^data:image/\w+;base64,#i', '', $imageBase64);
$imageData = base64_decode($base64String);

$uploadDir = '/var/www/screenshots'; // Change this to where ever you want to save the screenshot
$filePath = $uploadDir . '/' . $filename;

if (file_put_contents($filePath, $imageData)) {
    echo json_encode(['success' => true]);
} else {
    http_response_code(500);
    echo json_encode(['success' => false]);
}
?>
