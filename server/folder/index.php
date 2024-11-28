<?php
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        // Get the content from the POST request
        $content = file_get_contents("/dev/stdin");
    
        // echo($content);

        // Specify the file where the content will be saved
        $file = "saved_content4.jpg";

        // Save the content to the file
        file_put_contents($file, $content);

        // Output a success message
        echo "Content saved to $file";
    } else {
        // Output an error message if the request method is not POST
        echo "Invalid request method";
    }
?>

