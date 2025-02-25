"""
Use this script to avoid attaching manually usb devices 
to the WSL Ubuntu instance.
1. Copy this script somewhere in the Windows filesystem

curl -s -L https://raw.githubusercontent.com/Infineon/micropython/ports-psoc6-main/tools/psoc6/wsl-usb.py > wsl-usb.py

2. Use the Power Shell with admin rights
3. Use -h, --help flags to find out the available commands and options.
"""
import argparse, subprocess

version = "1.0.0"


def get_kitprog_list():
    usbipd_list_cmd = ["usbipd.exe", "list"]

    try:
        usbipd_proc = subprocess.Popen(usbipd_list_cmd, stdout=subprocess.PIPE)
        output = usbipd_proc.communicate()[0]
    except:
        raise Exception("usbipd error")

    kitprog3_list = []
    kitprog3_descr = "KitProg3 CMSIS-DAP, KitProg3 bridge, KitProg3 USB-UART (C..."
    lines = output.decode("utf-8").split("\r\n")
    for l in lines:
        # Create (busid, vid:pid, device state) tuple from line
        split_line = l.split("  ")
        strip_line = [i for i in split_line if i != ""]

        # Entries matching KitProg3 probe description
        if len(strip_line) == 4:
            if strip_line[2] == kitprog3_descr:
                kitprog3_list.append((strip_line[0], strip_line[3]))

    return kitprog3_list


def get_ppp_adapter_ip():
    ipconfig_cmd = ["ipconfig.exe", "-all"]

    try:
        ipconfig_proc = subprocess.Popen(ipconfig_cmd, stdout=subprocess.PIPE)
        output = ipconfig_proc.communicate()[0]
    except:
        raise Exception("ipconfig error")

    lines = output.decode("utf-8").split("\r\n")
    strip_lines = [i for i in lines if i != ""]
    ppp_entry = strip_lines.index("PPP adapter iconnect.infineon.com - iconnect.infineon.com:")
    entry_from_ppp_adapter = strip_lines[ppp_entry:]
    ipv4 = next(ip_entry for ip_entry in entry_from_ppp_adapter if "IPv4 Address" in ip_entry)

    ipv4.strip()
    ipv4 = ipv4.strip("IPv4 Address. . . . . . . . . . . : ")
    ipv4 = ipv4.removesuffix("(Preferred)")

    return ipv4


def attach():
    kp3_list = get_kitprog_list()
    ppp_ip = get_ppp_adapter_ip()

    usbipd_cmd = ["usbipd.exe", "bind"]
    wsl_usbip_cmd = ["wsl", "sudo", "usbip", "attach"]

    for d in kp3_list:
        if d[1] != "Attached":
            usbipd_bind_cmd = usbipd_cmd.copy()
            usbipd_bind_cmd.append("--busid=" + str(d[0]))

            try:
                usbipd_proc = subprocess.Popen(usbipd_bind_cmd)
                usbipd_proc.wait()
            except:
                raise Exception("usbipd error")

            wsl_usbip_attach_cmd = wsl_usbip_cmd.copy()
            wsl_usbip_attach_cmd.append("--remote=" + str(ppp_ip))
            wsl_usbip_attach_cmd.append("--busid=" + str(d[0]))

            try:
                usbipd_proc = subprocess.Popen(wsl_usbip_attach_cmd)
                usbipd_proc.wait()
            except:
                raise Exception("wsl usbip error")


def detach():
    kp3_list = get_kitprog_list()

    usbipd_cmd = ["usbipd.exe", "unbind"]

    # Unbind in windows is sufficient
    for d in kp3_list:
        usbipd_unbind_cmd = usbipd_cmd.copy()
        usbipd_unbind_cmd.append("--busid=" + str(d[0]))

        try:
            usbipd_proc = subprocess.Popen(usbipd_unbind_cmd)
            usbipd_proc.wait()
        except:
            raise Exception("usbipd error")


def parser():
    def main_parser_func(args):
        parser.print_help()

    def parser_attach_func(args):
        attach()

    def parser_detach_func(args):
        detach()

    def parser_list_func(args):
        print(get_kitprog_list())

    class ver_action(argparse.Action):
        def __init__(self, option_strings, dest, **kwargs):
            return super().__init__(
                option_strings, dest, nargs=0, default=argparse.SUPPRESS, **kwargs
            )

        def __call__(self, parser, namespace, values, option_string, **kwargs):
            print("wsl-usb version: " + version)
            parser.exit()

    parser = argparse.ArgumentParser(description="wsl-usb")
    parser.add_argument("-v", "--version", action=ver_action, help="wsl-usb version")
    subparser = parser.add_subparsers()
    parser.set_defaults(func=main_parser_func)

    # Attach
    parser_attach = subparser.add_parser("attach", description="Attach kitprog3 devices")
    parser_attach.set_defaults(func=parser_attach_func)

    # detach
    parser_detach = subparser.add_parser("detach", description="Detach kitprog3 devices")
    parser_detach.set_defaults(func=parser_detach_func)

    # List
    parser_list = subparser.add_parser("list", description="List kitprog3 devices with status")
    parser_list.set_defaults(func=parser_list_func)

    # Parser call
    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    parser()
