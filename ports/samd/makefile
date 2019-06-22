#BOARD = TRINKET
UF2DEV = /dev/sdb
#UF2CONV = /home/damien/others/uf2/utils/uf2conv.py

include Makefile

deploy: $(BUILD)/firmware.uf2
	$(ECHO) "Copying $< to the board"
	mount $(UF2DEV)
	cat /mnt/INFO_UF2.TXT > /dev/null
	cp $< /mnt
	umount /mnt
