<?php
$data = json_decode(file_get_contents("php://input"), true);

if (!$data || !isset($data['image']) || !isset($data['filename'])) {
    http_response_code(400);
    exit;
}

// Extract img b64 data and filename
$imageBase64 = $data['image'];
$filename = $data['filename'];

// Remove "data:image/png;base64," from the beginning
$base64String = preg_replace('#^data:image/\w+;base64,#i', '', $imageBase64);
$imageData = base64_decode($base64String);

$uploadDir = '/var/www/imgCapture';
$filePath = $uploadDir . '/' . $filename;

if (file_put_contents($filePath, $imageData)) {
    echo json_encode(['success' => true]);
} else {
    http_response_code(500);
    echo json_encode(['success' => false]);
}
?>
