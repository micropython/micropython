"""
ESP32 OTA Update HTTP Server

This module creates a simple HTTP server that accepts firmware uploads
for OTA updates. Useful for local development and testing.

Example usage:
    import ota_server
    import network

    # Connect to WiFi
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect('SSID', 'password')
    while not wlan.isconnected():
        pass

    # Start OTA server on port 8080
    ota_server.start(port=8080)

From your computer:
    # Upload firmware
    curl -X POST -F "firmware=@firmware.bin" http://ESP32_IP:8080/update

    # Or use web browser
    # Navigate to: http://ESP32_IP:8080
"""

import socket
import machine
import gc


HTML_UPLOAD_FORM = """<!DOCTYPE html>
<html>
<head>
    <title>ESP32 OTA Update</title>
    <style>
        body { font-family: Arial, sans-serif; max-width: 600px; margin: 50px auto; padding: 20px; }
        h1 { color: #333; }
        .upload-box { border: 2px dashed #ccc; padding: 40px; text-align: center; }
        input[type="file"] { margin: 20px 0; }
        button { background: #4CAF50; color: white; padding: 10px 20px; border: none; cursor: pointer; font-size: 16px; }
        button:hover { background: #45a049; }
        .info { background: #e7f3fe; padding: 15px; margin: 20px 0; border-left: 4px solid #2196F3; }
    </style>
</head>
<body>
    <h1>ESP32 OTA Firmware Update</h1>

    <div class="info">
        <strong>Current Version:</strong> {version}<br>
        <strong>Free Memory:</strong> {free_mem} bytes<br>
        <strong>IP Address:</strong> {ip_addr}
    </div>

    <div class="upload-box">
        <h2>Upload New Firmware</h2>
        <form method="POST" action="/update" enctype="multipart/form-data">
            <input type="file" name="firmware" accept=".bin" required>
            <br>
            <button type="submit">Upload and Update</button>
        </form>
    </div>

    <div class="info">
        <strong>Instructions:</strong>
        <ol>
            <li>Select firmware.bin file</li>
            <li>Click "Upload and Update"</li>
            <li>Wait for upload to complete</li>
            <li>ESP32 will reboot automatically</li>
        </ol>
    </div>

    <p><a href="/info">View Partition Info</a></p>
</body>
</html>
"""


def get_version():
    """Get current firmware version."""
    try:
        import sys
        return f"MicroPython {sys.version}"
    except:
        return "Unknown"


def get_ip():
    """Get current IP address."""
    try:
        import network
        wlan = network.WLAN(network.STA_IF)
        if wlan.isconnected():
            return wlan.ifconfig()[0]
        return "Not connected"
    except:
        return "Unknown"


def parse_multipart(data, boundary):
    """
    Parse multipart/form-data content.

    Returns:
        Dictionary with field names as keys and data as values
    """
    parts = data.split(b'--' + boundary)
    result = {}

    for part in parts:
        if b'Content-Disposition' in part:
            # Extract field name
            start = part.find(b'name="') + 6
            end = part.find(b'"', start)
            field_name = part[start:end].decode()

            # Extract data (after \r\n\r\n)
            data_start = part.find(b'\r\n\r\n') + 4
            data_end = part.rfind(b'\r\n')
            if data_start > 3 and data_end > data_start:
                field_data = part[data_start:data_end]
                result[field_name] = field_data

    return result


def handle_upload(client, data, boundary):
    """Handle firmware upload and install."""
    try:
        from esp32 import Partition
        import ota_update

        print("Parsing uploaded firmware...")
        fields = parse_multipart(data, boundary)

        if 'firmware' not in fields:
            return b"HTTP/1.1 400 Bad Request\r\n\r\nNo firmware file provided"

        firmware_data = fields['firmware']
        print(f"Received firmware: {len(firmware_data)} bytes")

        # Write to temporary file
        temp_file = '/tmp_ota_firmware.bin'
        with open(temp_file, 'wb') as f:
            f.write(firmware_data)

        print("Firmware saved, installing...")

        # Send response before rebooting
        response = b"""HTTP/1.1 200 OK\r
Content-Type: text/html\r\n\r\n
<!DOCTYPE html>
<html>
<head>
    <title>Update Success</title>
    <meta http-equiv="refresh" content="10">
</head>
<body>
    <h1>Firmware Update Successful!</h1>
    <p>ESP32 is rebooting...</p>
    <p>This page will refresh in 10 seconds.</p>
</body>
</html>"""

        client.send(response)
        client.close()

        # Install firmware
        ota_update.update_from_file(temp_file)

    except Exception as e:
        print(f"Upload error: {e}")
        error_html = f"""HTTP/1.1 500 Internal Server Error\r
Content-Type: text/html\r\n\r\n
<!DOCTYPE html>
<html>
<body>
    <h1>Upload Failed</h1>
    <p>Error: {e}</p>
    <p><a href="/">Back</a></p>
</body>
</html>"""
        return error_html.encode()


def handle_info():
    """Handle partition info request."""
    try:
        import ota_update
        info = ota_update.get_partition_info()

        html = f"""HTTP/1.1 200 OK\r
Content-Type: text/html\r\n\r\n
<!DOCTYPE html>
<html>
<head>
    <title>Partition Info</title>
    <style>
        body {{ font-family: Arial, sans-serif; max-width: 600px; margin: 50px auto; padding: 20px; }}
        table {{ width: 100%; border-collapse: collapse; }}
        th, td {{ padding: 10px; text-align: left; border-bottom: 1px solid #ddd; }}
        th {{ background: #4CAF50; color: white; }}
    </style>
</head>
<body>
    <h1>Partition Information</h1>

    <h2>Current Partition</h2>
    <table>
        <tr><th>Property</th><th>Value</th></tr>
        <tr><td>Label</td><td>{info['current']['label']}</td></tr>
        <tr><td>Size</td><td>{info['current']['size']} bytes</td></tr>
        <tr><td>Address</td><td>0x{info['current']['address']:X}</td></tr>
    </table>

    <h2>Next OTA Partition</h2>
    <table>
        <tr><th>Property</th><th>Value</th></tr>
        <tr><td>Label</td><td>{info['next']['label']}</td></tr>
        <tr><td>Size</td><td>{info['next']['size']} bytes</td></tr>
        <tr><td>Address</td><td>0x{info['next']['address']:X}</td></tr>
    </table>

    <p><a href="/">Back to Upload</a></p>
</body>
</html>"""
        return html.encode()

    except Exception as e:
        return f"HTTP/1.1 500 Error\r\n\r\nError: {e}".encode()


def start(port=8080, timeout=300):
    """
    Start OTA update HTTP server.

    Args:
        port: TCP port to listen on (default 8080)
        timeout: Socket timeout in seconds (default 300)
    """
    print(f"\n{'='*50}")
    print(f"ESP32 OTA Update Server")
    print(f"{'='*50}")
    print(f"Listening on: http://{get_ip()}:{port}")
    print(f"Version: {get_version()}")
    print(f"Free memory: {gc.mem_free()} bytes")
    print(f"{'='*50}\n")

    # Create server socket
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(('0.0.0.0', port))
    server.listen(1)
    server.settimeout(timeout)

    print("Server ready! Waiting for connections...")
    print("Press Ctrl+C to stop\n")

    try:
        while True:
            try:
                client, addr = server.accept()
                print(f"\nConnection from: {addr}")

                # Receive request
                request = client.recv(4096)

                # Parse request line
                request_line = request.split(b'\r\n')[0]
                print(f"Request: {request_line.decode()}")

                # Route requests
                if b'GET / ' in request_line:
                    # Serve upload form
                    html = HTML_UPLOAD_FORM.format(
                        version=get_version(),
                        free_mem=gc.mem_free(),
                        ip_addr=get_ip()
                    )
                    response = f"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n{html}"
                    client.send(response.encode())

                elif b'GET /info' in request_line:
                    # Serve partition info
                    response = handle_info()
                    client.send(response)

                elif b'POST /update' in request_line:
                    # Handle firmware upload
                    # Extract boundary from Content-Type header
                    content_type_line = [line for line in request.split(b'\r\n') if b'Content-Type' in line and b'boundary' in line]
                    if content_type_line:
                        boundary = content_type_line[0].split(b'boundary=')[1]

                        # Receive full request body
                        content_length = int([line.split(b': ')[1] for line in request.split(b'\r\n') if b'Content-Length' in line][0])

                        # Receive in chunks
                        body = request.split(b'\r\n\r\n', 1)[1]
                        while len(body) < content_length:
                            body += client.recv(4096)

                        handle_upload(client, body, boundary)
                    else:
                        client.send(b"HTTP/1.1 400 Bad Request\r\n\r\nMissing boundary")

                else:
                    # 404 Not Found
                    client.send(b"HTTP/1.1 404 Not Found\r\n\r\n404 - Page Not Found")

                client.close()
                gc.collect()

            except OSError as e:
                if e.args[0] == 11:  # EAGAIN - timeout, continue
                    continue
                else:
                    print(f"Socket error: {e}")
                    break

    except KeyboardInterrupt:
        print("\n\nServer stopped by user")
    finally:
        server.close()
        print("Server shut down")


# Start server if run as main
if __name__ == '__main__':
    start()
