import term, system, consts

class UartMenu():
	def __init__(self, gts, pm, safe = False, pol="Power off"):
		self.gts = gts
		self.menu = self.menu_main
		self.buff = ""
		self.pm = pm
		self.power_off_label = pol
	
	def main(self):
		if self.pm:
			term.setPowerManagement(self.pm)
		while self.menu:
			self.menu()
		
	def drop_to_shell(self):
		self.menu = False
		term.clear()
		import shell
	
	def menu_main(self):
		items = ["Python shell", "Apps", "Installer", "Settings", "Tools"]
		if self.gts:
			items.append(self.power_off_label)
		callbacks = [self.drop_to_shell, self.opt_launcher, self.opt_installer, self.menu_settings, self.menu_tools, self.go_to_sleep]
		message = ""
		cb = term.menu("Main menu", items, 0, message)
		self.menu = callbacks[cb]
		return
	
	def go_to_sleep(self):
		self.gts()
		
	def opt_installer(self):
		system.start("dashboard.terminal.installer", True)
	
	def opt_launcher(self):
		system.start("dashboard.terminal.launcher", True)
		
	def opt_configure_orientation(self):
		system.start("dashboard.terminal.orientation", True)

	def opt_downloader(self):
		system.start("dashboard.terminal.downloader", True)
		
	def menu_settings(self):
		items = ["Change display orientation", "< Return to main menu"]
		callbacks = [self.opt_configure_orientation, self.menu_main, self.menu_main]
		cb = term.menu("Settings", items)
		self.menu = callbacks[cb]
	
	def menu_tools(self):
		items = ["File downloader", "< Return to main menu"]
		callbacks = [self.opt_downloader, self.menu_main, self.menu_main]
		cb = term.menu("Tools", items)
		self.menu = callbacks[cb]

