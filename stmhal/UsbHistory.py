#!/usr/bin/python3
# -*- coding: utf-8 -*-
"""
    emProach
    Created on 1.11.2015
    
    @author: Tobias Badertscher (Baerospace)
    
"""
from collections import OrderedDict
import os
import re
NOK1="{{USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 1073741824, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 2048, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 0, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}}"
NOK2="{{USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 1073741824, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 2048, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 0, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}}"
NOK3='{{USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 1073741824, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 0, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}}'
NOK4='{{USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 1073741824, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 2048, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 0, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}} <repeats 29 times>}'
NOK5='{{USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 1073756176, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {129, 0, 0, 0}}, {USB_OTG_GINTSTS = 12304, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 12304, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {129, 0, 0, 0}}, {USB_OTG_GINTSTS = 12304, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {129, 0, 0, 0}}, {USB_OTG_GINTSTS = 12304, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 12304, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 12304, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 0, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}} <repeats 25 times>}'
NOK6='{{USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 1073741824, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 2048, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 0, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}} <repeats 29 times>}'
OK1="{{USB_OTG_GINTSTS = 12288, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 1073741824, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 2048, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 2048, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {128, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {1, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {1, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {1, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {128, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {1, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {1, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {128, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {1, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {1, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}}"
OK2="{{USB_OTG_GINTSTS = 12288, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 1073741824, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 2048, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 2048, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {128, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {1, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {1, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {1, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {128, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {1, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {1, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {128, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {1, 0, 0, 0}}}"
OK3="{{USB_OTG_GINTSTS = 12288, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 1073741824, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 2048, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 2048, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {128, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {1, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {1, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 4096, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 8192, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {1, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {128, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {1, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {1, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {128, 0, 0, 0}}, {USB_OTG_GINTSTS = 262144, DAIINT = 1, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {1, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {1, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 16, DAIINT = 0, DAOINT = 0, DOEPINTMSK = {0, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}, {USB_OTG_GINTSTS = 524288, DAIINT = 0, DAOINT = 1, DOEPINTMSK = {8, 0, 0, 0}, DIEPINTMSK = {0, 0, 0, 0}}}"

def extractComment(comment):
    reCom = r"/\*!<(.*)\*/"
    mtch = re.match(reCom, comment)
    res = comment
    if mtch:
        res = mtch.groups()[0]
    return res.strip()

def extractValue(value):
    reVal = r"\(\(uint32_t\)([0-9x]+)\)"
    mtch = re.match(reVal, value)
    res = value.strip()
    if mtch:
        res = int(mtch.groups()[0],16)
    return res

def parseRegisterDefines(fPath):
    debug = False
    path = os.sep.join(fPath)
    data = None
    if not os.path.exists(path):
        print("File %s does not exist" % path)
        return None
    with open(path, 'r') as fd:
        data = fd.read()
    reDefLine = r"#define[\s]+([A-Z0-9_]+)[\s]+([\S]+)[\s]+(.*)"
    reGrpLine = r"/[*]+[\s]+Bit definition for[\s]*([A-Z0-9_]+)[\s]register.*"
    res = OrderedDict()
    grpName = None
    for idx, line in enumerate(data.split(os.linesep)):
        if len(line.strip())==0:
            continue
        if debug:
            print("%3d %s" %(idx, line))
        mtchDef = re.match(reDefLine, line)
        if mtchDef and grpName:
            if debug:
                print(mtchDef.groups())
            name = mtchDef.groups()[0]
            res[grpName][name] = OrderedDict()
            res[grpName][name]['value'] = extractValue(mtchDef.groups()[1])
            res[grpName][name]['comment'] = extractComment(mtchDef.groups()[2])
        mtchGrp = re.match(reGrpLine, line)
        if mtchGrp:
            grpName = mtchGrp.groups()[0]
            res[grpName] = OrderedDict()
            if debug:
                print(mtchGrp.groups())
            #return 
    return res

def parseDescription(item):
    debug=False
    res = []
    INIT, ARRAY, ENTRY, EXPNAME, EXPVALUE, EXPVECVALUE = range(6)
    sName = ['INIT', 'ARRAY', 'ENTRY', 'EXPNAME', 'EXPVALUE', 'EXPVECVALUE']
    state = INIT
    name  = None
    value = None
    entry = None
    skipit = False
    for ch in item:
        if ch==" ":
            continue
        if ch==">":
            skipit = False
            continue
        if ch=='<':
            skipit =True
        if skipit:
            continue
        if debug:
            print("\"%s\" " % ch),
        if state == INIT and ch == "{":
            state = ARRAY
            if debug:
                print(" INIT -> %s" % (sName[state]))
            continue
        if state == ARRAY and ch == "{":
            entry=OrderedDict()
            state = EXPNAME
            name=""
            if debug:
                print(" ARRAY -> %s" % (sName[state]))
            continue
        if state == EXPNAME and ch !="=":
            name += ch
            if debug:
                print(" EXPNAME -> %s (%s)" % (sName[state], name))
            continue
        if state == EXPNAME:
            value = ""
            state = EXPVALUE
            if debug:
                print(" EXPNAME -> %s (fin %s)" % (sName[state], name))
            continue
        if state == EXPVALUE:
            if ch == ',':
                entry[name]=value if isinstance(value, list) else int(value)
                state = EXPNAME
                if debug:
                    print(" EXPVALUE -> %s (%s = %s)" % (sName[state], name, entry[name]))
                name=""
            elif ch == '}':
                entry[name]=value
                state = ARRAY
                res.append(entry)
                if debug:
                    print(" EXPVALUE -> %s (%s)" % (sName[state], entry[name]))
            else:
                value += ch
                if ch == "{":
                    state = EXPVECVALUE
                if debug:
                    print(" EXPVALUE -> %s (%s)" % (sName[state], value))
            continue
        if state == EXPVECVALUE:
            value += ch
            if ch == "}":
                value = value.replace("{","[")
                value = value.replace("}","]")
                value = eval(value) 
                state = EXPVALUE
            if debug:
                print(" EXPVECVALUE -> %s (%s)" % (sName[state], value))
            continue
    
    return res
 

def show(l1, name1, l2, name2, flags):
    fWidth = {'w1':15,'w2':15,'w3':15}
    varMap = { 'USB_OTG_GINTSTS':'USB_OTG_GINTSTS'}
    iterLen = max(len(l1), len(l2))
    fmt="{0:{w1}s} {1:{w2}s} {2:{w3}s} {3:s}" 
    print(fmt.format("", name1, name2, "", w1=fWidth['w1'], w2=fWidth['w2'], w3=fWidth['w3']))
    for i in range(iterLen):
        print("{0:{w1}s} Entry {1}".format("", i, w1=fWidth['w1']))
        l1i = l1[i]
        l2i = l2[i] if len(l2)> i else None
        for k in l1[0]:
            if isinstance(l1i[k], list):
                f1 = (" ".join(("%d"% el for el in l1i[k]))) if l1i else ""
                f2 = (" ".join(("%d"% el for el in l2i[k]))) if l2i else ""
                print(fmt.format(k, f1, f2, "", w1=fWidth['w1'], w2=fWidth['w2'], w3=fWidth['w3']))
            else:
                l1Val = "0x%08x" % l1i[k]
                l2Val = ("0x%08x" % l2i[k]) if l2i else ""
                neq = ""
                if l2Val!="":
                    neq = "<<<<<" if l1i[k]!=l2i[k] else ""
                print(fmt.format(k, l1Val, l2Val, neq, w1=fWidth['w1'], w2=fWidth['w2'], w3=fWidth['w3']))
            if k in varMap:
                com1 = []
                com2 = []
                for flagName, flagValue in flags[k].items():
                    if l1i[k] & flagValue['value']!=0:
                        com1.append("%s (%s)" % (flagValue['comment'],flagName))
                    if l2i and (l2i[k] & flagValue['value'])!=0:
                        com2.append("%s (%s)" % (flagValue['comment'],flagName))
                for com in com1:
                    print("{0:{w1}s} {1:s}".format("", com, w1=fWidth['w1']))
                for com in com2:
                    print("{0:{w1}s} {1:{w2}s} {2:s}".format("", "", com, w1=fWidth['w1'], w2=fWidth['w2']))
        print()

def main():
    nok1 =  parseDescription(NOK1)
    nok2 =  parseDescription(NOK2)
    nok4 =  parseDescription(NOK4)
    nok5 =  parseDescription(NOK5)
    nok6 =  parseDescription(NOK6)
    ok1   =  parseDescription(OK1)
    ok2   =  parseDescription(OK2)
    flags = parseRegisterDefines(['cmsis','devinc','stm32f429xx.h'])
    #show(nok1,"NOK1", nok2,"NOK2", flags)
    #show(ok1,"OK1", ok2,"OK2", flags)
    #show(ok1,"OK1", ok2,"OK2", flags)
    show(ok1,"OK1", nok6,"NOK6", flags)


if __name__ == '__main__':
    main()