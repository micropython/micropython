from machine import SDCard
import uos

sd = SDCard()
uos.mount(sd, "/sd")
