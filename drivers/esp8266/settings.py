# Define UART connection to match your hardware setup
uart_port = 0
uart_baud_rate = 115200
uart_tx_pin = 0
uart_rx_pin = 1

# Define WiFi parameters, ssid and password
ssid = "your wifi ssid here"
password = "your wifi password here"

# Define get request parameters
gethost = "httpbin.org"
getpath = "/get"
getuseragent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.106 Safari/537.36"
getcontenttype = "text/plain"  # "application/json", "application/x-www-form-urlencoded", "text/plain",...
getpayload = ""
getport = 80

# Define put request parameters
puthost = "httpbin.org"
putpath = "/put"
putuseragent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.106 Safari/537.36"
putcontenttype = "text/plain"  # "application/json", "application/x-www-form-urlencoded", "text/plain",...
putpayload = ""
putport = 80

# Define post request parameters
posthost = "www.httpbin.org"
postpath = "/post"
postuseragent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.106 Safari/537.36"
postcontenttype = "application/json"  # "application/json", "application/x-www-form-urlencoded", "text/plain",...
postpayload = "{'key':'value'}"
postport = 80
