import term, system, consts
from collections import OrderedDict

class UartMenu():
	def __init__(self, gts = None, pm = None, safe = False, pol="Power off"):
		self.gts = gts
		self.menu = self.menu_main
		if (safe):
			self.menu = self.menu_safe
		self.buff = ""
		self.pm = pm
		self.power_off_label = pol
		self.menu_items = list()
		self.menu_fun = list()
		self.settings_items = list()
		self.settings_fun = list()
		self.register_main("Python shell", self.drop_to_shell)
		self.register_main("Apps", self.opt_launcher)
		self.register_main("Installer", self.opt_installer)
		self.register_main("Settings", self.menu_settings)
		self.register_main("About", self.opt_about)
		self.register_main("Check for updates", self.opt_ota_check)

		self.register_setting("Configure WiFi", self.opt_configure_wifi, 0)
		self.register_setting("< Return to main menu", self.menu_main, 1)

	
	def register_main(self, name, function, pos=100):
		self.menu_items.insert(pos, name)
		self.menu_fun.insert(pos, function)

	def register_setting(self, name, function, pos=-1):
		self.settings_items.insert(pos, name)
		self.settings_fun.insert(pos, function)

	def main(self):
		term.setPowerManagement(self.pm)
		while self.menu:
			self.menu()
		
	def drop_to_shell(self):
		self.menu = False
		term.clear()
		import shell
	
	def menu_main(self):
		message = "Welcome!\nYour badge is running firmware version "+str(consts.INFO_FIRMWARE_BUILD)+": "+consts.INFO_FIRMWARE_NAME+"\n"
		cb = term.menu("Main menu", self.menu_items, 0, message)
		self.menu = self.menu_fun[cb]
		return
		
	def opt_change_nickname(self):
		system.start("dashboard.terminal.nickname", True)
		
	def opt_installer(self):
		system.start("dashboard.terminal.installer", True)
	
	def opt_launcher(self):
		system.start("dashboard.terminal.launcher", True)
	
	def opt_configure_wifi(self):
		system.start("dashboard.terminal.wifi", True)
		
	def opt_ota(self):
		system.ota(True)
		
	def opt_ota_check(self):
		system.start("checkforupdates", True)
	
	def opt_about(self):
		system.start("about", True)
		
	def menu_settings(self):
		cb = term.menu("Settings",self.settings_items)
		self.menu = self.settings_fun[cb]
