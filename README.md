# Tiny Web Server
To learn sockets + HTTP basics

# How it Works
- TCP socket → bind to :8080
- listen for incoming clients
- accept returns a client socket
- Send HTTP header (note the \r\n\r\n)
- Stream file bytes, close

# Added Features
- 404 file not found
- MIME detection by file extension

# Instructions
- gcc -o server.c server
- ./server
- curl -i http://localhost:8080 (or go to http://localhost:8080 in browser)

# To-Do
- Add Content-Length, keep-alive, multiple files
- Basic concurrency (fork/threads)
- Live-reload via WebSocket for dev UXGithub - https://t.co/Whfr7AZHcc
