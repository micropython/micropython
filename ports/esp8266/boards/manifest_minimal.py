freeze('$(PORT_DIR)/modules', (
    '_boot.py', 'flashbdev.py', 'inisetup.py', 'port_diag.py',
    'webrepl.py', 'webrepl_setup.py', 'websocket_helper.py'
))
freeze('$(MPY_DIR)/tools', ('upip.py', 'upip_utarfile.py'))
