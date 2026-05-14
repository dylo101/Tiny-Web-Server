# Tiny Web Server
To learn sockets + HTTP basics

# How it Works
- TCP socket → bind to :8080
- listen for incoming clients
- accept returns a client socket
- Send HTTP header (note the \r\n\r\n)
- Stream file bytes, close

# Instructions
- gcc -o server.c server
- ./server
- curl -i http://localhost:8080 (or go to http://localhost:8080 in browser)

# To-Do
- If file missing → send 404 Not Found
- Detect MIME by extension (.css, .js, .png)
- Add Content-Length, keep-alive, multiple files
- Basic concurrency (fork/threads)
- Live-reload via WebSocket for dev UXGithub - https://t.co/Whfr7AZHcc
