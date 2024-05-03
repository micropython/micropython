# photo.py

__version__ = "1.0.3"  # Major.Minor.Patch

# Ways to run this program:-

# 1. With ampy
#    ampy --port $PORT run bin/photo.py
# example output:-
#    photo fn=out.jpg size=22((default)) quality=10
#    Length of buf: 23579

# 2. From REPL shell
#     >>> ARGV=["pic.jpg","5","10"];exec(open("bin/photo.py").read())
# example output:-
#    photo fn=pic.jpg size=5(FRAME_QVGA) quality=10
#    Length of buf: 9495

# 3. using mipyshell
# To run this program with arguments, install https://github.com/vsolina/mipyshell
# and save this file as bin/photo.py - then (for size 5 and quality 10):-
#     photo outfile.jpg 5 10


import camera


def __main__(args):
    capture(args[2:])  # mipyshell first 2 arguments are "python" and "photo.py"


def capture(args):
    fn = "out.jpg"
    quality = 10
    size = 22

    camera_frames = {
        0: {"name": "FRAME_96X96", "value": camera.FRAME_96X96},
        1: {"name": "FRAME_QQVGA", "value": camera.FRAME_QQVGA},
        2: {"name": "FRAME_QCIF", "value": camera.FRAME_QCIF},
        3: {"name": "FRAME_HQVGA", "value": camera.FRAME_HQVGA},
        4: {"name": "FRAME_240X240", "value": camera.FRAME_240X240},
        5: {"name": "FRAME_QVGA", "value": camera.FRAME_QVGA},
        6: {"name": "FRAME_CIF", "value": camera.FRAME_CIF},
        7: {"name": "FRAME_HVGA", "value": camera.FRAME_HVGA},
        8: {"name": "FRAME_VGA", "value": camera.FRAME_VGA},
        9: {"name": "FRAME_SVGA", "value": camera.FRAME_SVGA},
        10: {"name": "FRAME_XGA", "value": camera.FRAME_XGA},
        11: {"name": "FRAME_HD", "value": camera.FRAME_HD},
        12: {"name": "FRAME_SXGA", "value": camera.FRAME_SXGA},
        13: {"name": "FRAME_UXGA", "value": camera.FRAME_UXGA},
        14: {"name": "FRAME_FHD", "value": camera.FRAME_FHD},
        15: {"name": "FRAME_P_HD", "value": camera.FRAME_P_HD},
        16: {"name": "FRAME_P_3MP", "value": camera.FRAME_P_3MP},
        17: {"name": "FRAME_QXGA", "value": camera.FRAME_QXGA},
        18: {"name": "FRAME_QHD", "value": camera.FRAME_QHD},
        19: {"name": "FRAME_WQXGA", "value": camera.FRAME_WQXGA},
        20: {"name": "FRAME_P_FHD", "value": camera.FRAME_P_FHD},
        21: {"name": "FRAME_QSXGA", "value": camera.FRAME_QSXGA},
        22: {"name": "(default)", "value": None},
    }

    if len(args) > 0:
        fn = args[0]

    ## ESP32-CAM (default configuration) - https://bit.ly/2Ndn8tN
    camera.init(0, format=camera.JPEG, fb_location=camera.PSRAM)

    if len(args) > 1:
        size = int(args[1])
        camera.framesize(camera_frames[size]["value"])

    if len(args) > 2:
        quality = int(args[2])
        camera.quality(quality)

    print(
        "photo fn={} size={}({}) quality={}".format(fn, size, camera_frames[size]["name"], quality)
    )

    # AI-Thinker esp32-cam board
    # ai_thinker = {PIN_PWDN:32, PIN_RESET:-1, PIN_XCLK:0, PIN_SIOD:26, PIN_SIOC:27, PIN_D7:35, PIN_D6:34, PIN_D5:39, PIN_D4:36, PIN_D3:21, PIN_D2:19, PIN_D1:18, PIN_D0:5, PIN_VSYNC:25, PIN_HREF:23, PIN_PCLK:22, XCLK_MHZ:16, PIXFORMAT:5, FRAMESIZE:10, JPEG_QUALITY:10, FB_COUNT:1, }

    ## M5Camera (Version B) - https://bit.ly/317Xb74
    # camera.init(0, d0=32, d1=35, d2=34, d3=5, d4=39, d5=18, d6=36, d7=19, format=camera.JPEG, framesize=camera.FRAME_VGA, xclk_freq=camera.XCLK_10MHz, href=26, vsync=25, reset=15, sioc=23, siod=22, xclk=27, pclk=21, fb_location=camera.PSRAM)   #M5CAMERA

    ## T-Camera Mini (green PCB) - https://bit.ly/31H1aaF
    # import axp202 # source https://github.com/lewisxhe/AXP202_PythonLibrary
    # USB current limit must be disabled (otherwise init fails)
    # axp=axp202.PMU( scl=22, sda=21, address=axp202.AXP192_SLAVE_ADDRESS  )
    # limiting=axp.read_byte( axp202.AXP202_IPS_SET )
    # limiting &= 0xfc
    # axp.write_byte( axp202.AXP202_IPS_SET, limiting )

    # camera.init(0, d0=5, d1=14, d2=4, d3=15, d4=18, d5=23, d6=36, d7=39, format=camera.JPEG, framesize=camera.FRAME_VGA, xclk_freq=camera.XCLK_20MHz, href=25, vsync=27, reset=-1, pwdn=-1, sioc=12, siod=13, xclk=32, pclk=19)

    # The parameters: format=camera.JPEG, xclk_freq=camera.XCLK_10MHz are standard for all cameras.
    # You can try using a faster xclk (20MHz), this also worked with the esp32-cam and m5camera
    # but the image was pixelated and somehow green.

    #  ## Other settings:
    #  # flip up side down
    #  camera.flip(1)
    #  # left / right
    #  camera.mirror(1)

    #  # framesize
    #  camera.framesize(camera.FRAME_240x240)
    #  # The options are the following:
    #  # FRAME_96X96 FRAME_QQVGA FRAME_QCIF FRAME_HQVGA FRAME_240X240
    #  # FRAME_QVGA FRAME_CIF FRAME_HVGA FRAME_VGA FRAME_SVGA
    #  # FRAME_XGA FRAME_HD FRAME_SXGA FRAME_UXGA FRAME_FHD
    #  # FRAME_P_HD FRAME_P_3MP FRAME_QXGA FRAME_QHD FRAME_WQXGA
    #  # FRAME_P_FHD FRAME_QSXGA
    #  # Check this link for more information: https://bit.ly/2YOzizz
    #
    #  # special effects
    #  camera.speffect(camera.EFFECT_NONE)
    #  # The options are the following:
    #  # EFFECT_NONE (default) EFFECT_NEG EFFECT_BW EFFECT_RED EFFECT_GREEN EFFECT_BLUE EFFECT_RETRO
    #
    #  # white balance
    #  camera.whitebalance(camera.WB_NONE)
    #  # The options are the following:
    #  # WB_NONE (default) WB_SUNNY WB_CLOUDY WB_OFFICE WB_HOME
    #
    #  # saturation
    #  camera.saturation(0)
    #  # -2,2 (default 0). -2 grayscale
    #
    #  # brightness
    #  camera.brightness(0)
    #  # -2,2 (default 0). 2 brightness
    #
    #  # contrast
    #  camera.contrast(0)
    #  #-2,2 (default 0). 2 highcontrast
    #
    #  # quality
    #  camera.quality(10)
    #  # 10-63 lower number means higher quality
    #

    buf = camera.capture()

    if buf:
        print("Length of buf:", len(buf))

        if fn:
            with open(fn, "wb") as f:
                f.write(buf)
        else:
            print("not written - no filename given")
            # print("Contents of buf in hex:", buf.hex())

    else:
        print("Capture failed (too big for PSRAM?")

    # print("open http://esp32-cam-05.local/foo.jpg")

    camera.deinit()


try:
    # if 'ARGV' in locals():
    eval(
        "capture(ARGV)"
    )  # ARGV is supplied by caller thusly: ARGV=["pic.jpg","5","10"];exec(open("bin/photo.py").read())
except:  # Exception as e:
    # print(e) # name 'ARGV' isn't defined
    capture([])
