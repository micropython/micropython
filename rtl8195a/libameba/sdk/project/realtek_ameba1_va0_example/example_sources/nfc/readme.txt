Example Description

This example describes how to use nfc interface. 

Requirement Components:
1. nfc reader. 
	Ex. Smart phone which has NFC reader. In Android, you can use below app

	NFC Tag reader
	https://play.google.com/store/apps/details?id=com.nxp.taginfolite

	NFC Tag reader & writer
	https://play.google.com/store/apps/details?id=com.wakdev.wdnfc

	NFC tag writer
	https://play.google.com/store/apps/details?id=com.nxp.nfc.tagwriter

2. Connect NFC antenna.
	By default the NFC antenna is provided but not connected.
	You can choose your desired antenna and weld it on the board


Verification Steps:
(a) Open nfc reader app, Tap phone on NFC antenna, then the ndef message content is text "HELLO WORLD!"
(b) Open nfc writer app, write something to the tag. (Ex. text message "abcdefg")
	It'll also dump raw data on the log.
(c) Open nfc reader app, tap phone on NFC antenna, and check if the conten is exactly the same as previous move.

