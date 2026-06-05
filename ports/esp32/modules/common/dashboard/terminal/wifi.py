import network, term, sys, system, nvs

system.serialWarning()

def main():
	term.empty_lines()
	items = ["Scan for networks", "Manual SSID entry", "Return to main menu"]
	fun = [scan, manual, home]
	fun[term.menu("WiFi setup", items)]()

def home():
	system.home(True)

def scan():
	term.header(True, "WiFi setup")
	print("Scanning...")
	sta_if = network.WLAN(network.STA_IF)
	sta_if.active(True)
	ssid_result = sta_if.scan()
	ssid_list = []
	for ssid in ssid_result:
		ssid_list.append(ssid[0].decode('utf-8', 'ignore'))
	ssid_list.append("< Back")
	option = term.menu("WiFi setup - Select a network", ssid_list)
	if option != "< Back":
		password(ssid_result[option][0].decode('utf-8', 'ignore'), ssid_result[option][4])

def manual():
	term.header(True, "WiFi setup - Enter SSID")
	print("Enter the SSID for the network you want to connect to, ")
	print("or leave blank to go back. Press RETURN to continue.")
	ssid = term.prompt("SSID", 1,5)
	if (len(ssid) >= 1):
		items = ["WEP / WPA / WPA2", "Open network", "WPA / WPA2 Enterprise", "Return to main menu"]
		option = term.menu("WiFi setup - Authentication method", items)
		if option == 0:
			password(ssid, 1)
		elif option == 1:
			password(ssid, 0)
		elif option == 2:
			password(ssid, 5)
		
def password(ssidName, ssidType):
	if ssidType == 0:
		confirm(ssidName, '')
	elif ssidType == 5:
		term.header(True, "WiFi setup")
		print("WPA / WPA2 Enterprise is currently not supported!")
		print("")
		print("Press any key to return to the main menu")
		sys.stdin.read(1)
	else:
		term.header(True, "WiFi setup - Enter PASSWORD")
		print("Enter the PASSWORD for '"+ssidName+"', ")
		print("or leave blank to go back. Press RETURN to continue.")
		password = term.prompt("PASSWORD", 1,5)
		if (len(password) >= 1):
			confirm(ssidName, password)

def confirm(ssid, password):
	term.header(True, "WiFi setup")
	nvs.nvs_setstr("system", "wifi.ssid", ssid)
	nvs.nvs_setstr("system", "wifi.password", password)
	print("New configuration has been saved.")
	print("")
	print("SSID:\t\t"+ssid)
	if len(password)<1:
		print("No password")
	else:
		print("PASSWORD:\t"+password)
	print("")
	print("Press any key to return to the homescreen")
	sys.stdin.read(1)
	system.home(True)

while True:
	main()
