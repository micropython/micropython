# This script support the discovery of attached KitProg3 devices

import argparse, glob, os, re, subprocess, yaml


def get_devs_from_yml(dev_yml):
    if not os.path.exists(dev_yml):
        raise Exception("devices .yml file does not exit")

    board_sn_map = []
    with open(dev_yml, "r") as devs_f:
        board_sn_map = yaml.safe_load(devs_f)

    return board_sn_map


def udevadm_get_kitprog3_attached_devs():
    def get_ttyACM_dev_list():
        return glob.glob(os.path.join("/dev", "ttyACM*"), recursive=False)

    def get_udevadm_port_attrs_output(dev):
        dev_param = "--name=" + str(dev)
        cmd_line_args = ["udevadm", "info", dev_param, "--attribute-walk"]
        udevadm_output_lines = []
        try:
            udevadm_proc = subprocess.Popen(cmd_line_args, stdout=subprocess.PIPE)
            while True:
                line = udevadm_proc.stdout.readline()
                udevadm_output_lines.append(line)
                if not line:
                    break
        except:
            raise Exception("udevadm error")

        return udevadm_output_lines

    def is_kitprog_device(udevadm_output_lines):
        def is_device_attr_found(line, pattern):
            attr = re.search(pattern, str(line))
            if attr is not None:
                return True

            return False

        # It is a kitprog probe is these matches
        # are found in the device attributes
        required_attr_match = [
            'ATTRS{interface}=="KitProg\d USBUART"',
            'ATTRS{product}==".*KitProg\d.*"',
        ]

        attr_found_count = 0

        for line in udevadm_output_lines:
            for req_match in required_attr_match:
                if is_device_attr_found(line, req_match):
                    attr_found_count += 1

        if attr_found_count == len(required_attr_match):
            return True

        return False

    def get_kitprog_serial_number(udevadm_output_lines):
        for line in udevadm_output_lines:
            attr = re.search('ATTRS{serial}=="[a-zA-Z0-9]*"', str(line))
            if attr is not None:
                sn = attr.group().removeprefix("ATTRS{serial}==")
                sn = sn.strip('"')
                return sn

        return ""

    kitprog_devs = []
    dev_list = get_ttyACM_dev_list()

    if dev_list is not []:
        for dev in dev_list:
            udevadm_output_lines = get_udevadm_port_attrs_output(dev)
            if is_kitprog_device(udevadm_output_lines):
                sn = get_kitprog_serial_number(udevadm_output_lines)
                # new kp device
                kp_dev = {}
                kp_dev["port"] = dev
                kp_dev["sn"] = sn

                kitprog_devs.append(kp_dev)

    return kitprog_devs


def get_devices_serial_num(board=None, devs_yml=None):
    sn_list = []
    port_sn_map = udevadm_get_kitprog3_attached_devs()

    if board is not None and devs_yml is not None:
        board_sn_map = get_devs_from_yml(devs_yml)

        for dev in port_sn_map:
            for listed_board in board_sn_map:
                if board == listed_board["board"]:
                    if dev["sn"] in listed_board["sn"]:
                        sn_list.append(dev["sn"])
                        break
    else:
        for dev in port_sn_map:
            sn_list.append(dev["sn"])

    return sn_list


def get_devices_port(board=None, devs_yml=None):
    port_list = []
    port_sn_map = udevadm_get_kitprog3_attached_devs()

    if board is not None and devs_yml is not None:
        board_sn_map = get_devs_from_yml(devs_yml)

        for dev in port_sn_map:
            for listed_board in board_sn_map:
                if board == listed_board["board"]:
                    if dev["sn"] in listed_board["sn"]:
                        port_list.append(dev["port"])
                        break
    else:
        for dev in port_sn_map:
            port_list.append(dev["port"])

    return port_list


if __name__ == "__main__":
    pass


def parser():
    def main_parser_func(args):
        parser.print_help()

    def parse_validate_opt_arg_mutual_required(args):
        if args.devs_yml and args.board is None:
            parser.error("--devs-yml requires --board.")
        if args.board and args.devs_yml is None:
            parser.error("--board requires --dev-yml.")

    def parser_get_devices_serial_num(args):
        parse_validate_opt_arg_mutual_required(args)
        devs_serial = get_devices_serial_num(args.board, args.devs_yml)
        print(*devs_serial)

    def parser_get_devices_port(args):
        parse_validate_opt_arg_mutual_required(args)
        devs_port = get_devices_port(args.board, args.devs_yml)
        print(*devs_port)

    parser = argparse.ArgumentParser(description="Get kitprog3 device utility")

    subparser = parser.add_subparsers()
    parser.set_defaults(func=main_parser_func)

    # Get devices serial numbers
    parser_sn = subparser.add_parser(
        "serial-number", description="Get kitprog3 devices serial number list"
    )
    parser_sn.add_argument("-b", "--board", type=str, help="Board name")
    parser_sn.add_argument(
        "-y", "--devs-yml", type=str, help="Device list yml with board - serial number map"
    )
    parser_sn.set_defaults(func=parser_get_devices_serial_num)

    # Get devices port
    parser_port = subparser.add_parser("port", description="Get kitprog3 devices port list")
    parser_port.add_argument("-b", "--board", type=str, help="Board name")
    parser_port.add_argument(
        "-y", "--devs-yml", type=str, help="Device list yml with board - serial number map"
    )
    parser_port.set_defaults(func=parser_get_devices_port)

    # Parser call
    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    parser()
