import wifi, term, system, urequests

system.serialWarning()

while True:
	wifi.connect()
	term.header(True, "Connecting to WiFi...")
	if not wifi.wait():
		system.home()

	print("Enter a URL to download from or nothing to cancel.")
	url = term.prompt("URL to download from", 1, 3, "")
	print("\n\n\n")
	print("Enter a path to save to or nothing to cancel.")
	path = term.prompt("Path to save to     ", 1, 6, "/")

	if len(url) < 1 or len(path) < 1:
		system.home()

	term.header(True, "Downloading...")

	request = urequests.get(url)

	term.header(True, "Saving...")

	f = open(path, 'wb')
	f.write(request.content)
	f.close()

	term.header(True, "Done")
