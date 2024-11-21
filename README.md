# Webserver - 1337 Project

## Overview
The **Webserver** project at 1337 involves creating a simple HTTP web server from scratch. The goal is to develop a functional server that can handle HTTP requests, manage connections, serve static files, and respond with the appropriate HTTP responses. This project will teach essential concepts in networking, multi-threading, and HTTP, giving you a deeper understanding of how web servers work behind the scenes.

## Objective

The goal of this project is to implement a simple web server capable of handling HTTP requests, routing to the appropriate static resources, and responding with HTTP status codes. The server should be able to:

- Accept incoming HTTP requests.
- Parse HTTP requests and serve the appropriate files.
- Handle errors, such as missing files or unsupported HTTP methods.
- Support basic multi-threading to manage multiple client connections concurrently.
- Understand HTTP methods like GET, POST, and handle basic headers.
- Serve static content like HTML, CSS, JavaScript, and images.

This web server is a foundation for learning how web servers function and the HTTP protocol works. It is not intended to be production-ready but rather a learning tool.

---

## Features

- **HTTP Request Parsing**: The server parses incoming HTTP requests to extract headers and request types (GET, POST).
- **File Handling**: The server can read and serve static files from the file system, such as HTML, CSS, JavaScript, and images.
- **HTTP Responses**: Returns appropriate HTTP status codes and response headers based on the request (e.g., 200 OK, 404 Not Found).
- **Multi-threading**: Supports multiple concurrent client connections using threads to handle each request in isolation.
- **Error Handling**: Handles errors such as 404 (file not found) or 405 (method not allowed).
- **cgi**: create mini cgi to serve dynamique content.

---

## Technologies Used

- **C Programming**: The project is implemented in C, focusing on system-level programming and efficient use of memory and resources.
- **Sockets**: The server uses TCP sockets for communication with clients, allowing the server to accept connections from clients on a specified port.
- **HTTP Protocol**: Basic understanding and implementation of HTTP methods (GET, POST), status codes, headers, and responses.
- **File I/O**: The server interacts with the file system to serve static files to clients.

## How to Run

### 1. Clone the Repository

```bash
git clone https://github.com/haghouli/webserver.git
cd webserver
make

