<?php
error_log("PHP: starting the script.\n", 3); // Write to stderr

// Read input from stdin
$content = file_get_contents('php://stdin');
if (!$content) {
    $content = "<EMPTY>";
}
error_log("PHP: stdin: " . $content . "\n", 3); // Write to stderr

// Send HTTP headers
header("Content-Type: text/html");

// Print HTML content
echo "<html>";
echo "<head>";
echo "<title>Test CGI</title>";
echo "</head>";
echo "<body>";

echo "<center><h1>Hello from PHP!</h1></center>";

echo "<h2>Env Vars:</h2>";
foreach ($_SERVER as $param => $value) {
    echo "$param: $value <br>";
}

echo "<h2>Body:</h2>";
echo "Content of stdin: <br>";
echo $content;

echo "<h2>Greetings:</h2>";

$method = $_SERVER['REQUEST_METHOD'];

if ($method != "GET") {
    echo "Only GET method allowed for this script! <br>";
} else {
    $qs = $_SERVER['QUERY_STRING'];
    if ($qs) {
        echo "Query string: $qs <br>";
        parse_str($qs, $params);
        $name = $params['name'];
        echo "<h1>Hello $name!!!!!!!!!!!!!!!!!!</h1>";
    }
}

echo "</body>";
echo "</html>";
?>
