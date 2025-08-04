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
            'ATTRS{interface}=="KitProg\d.*"',
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
                sn = attr.group()[len("ATTRS{serial}==") :]
                sn = sn.strip('"')
                return sn

        return ""

    kitprog_devs = []
    dev_list = get_ttyACM_dev_list()

    if dev_list != []:
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


def get_devices(search_param, board=None, devs_yml=None, hw_ext=None):
    dev_list = []
    port_sn_map = udevadm_get_kitprog3_attached_devs()

    if board is not None and devs_yml is not None:
        board_sn_map_list = get_devs_from_yml(devs_yml)

        for board_sn_map_item in board_sn_map_list:
            if board == board_sn_map_item["board_type"]:
                for dev in port_sn_map:
                    for mapped_board_item in board_sn_map_item["board_list"]:
                        if dev["sn"] == mapped_board_item["sn"]:
                            if hw_ext is None:
                                dev_list.append(dev[search_param])
                                break
                            if hw_ext is not None:
                                if "hw_ext" in mapped_board_item.keys():
                                    if hw_ext == mapped_board_item["hw_ext"]:
                                        dev_list.append(dev[search_param])
                                        break
    else:
        for dev in port_sn_map:
            dev_list.append(dev[search_param])

    return dev_list


def get_devices_serial_num(board=None, devs_yml=None, hw_ext=None):
    return get_devices("sn", board, devs_yml, hw_ext)


def get_devices_port(board=None, devs_yml=None, hw_ext=None):
    return get_devices("port", board, devs_yml, hw_ext)


def parser():
    def main_parser_func(args):
        parser.print_help()

    def parse_validate_opt_arg_mutual_required(args):
        if args.devs_yml and args.board is None:
            parser.error("--devs-yml requires --board.")
        if args.board and args.devs_yml is None:
            parser.error("--board requires --dev-yml.")
        if args.hw_ext is not None and (args.board is None or args.devs_yml is None):
            parser.error("--hw_ext requires --board and --dev-yml.")

    def parser_get_devices_serial_num(args):
        parse_validate_opt_arg_mutual_required(args)
        devs_serial = get_devices_serial_num(args.board, args.devs_yml, args.hw_ext)
        print(*devs_serial)

    def parser_get_devices_port(args):
        parse_validate_opt_arg_mutual_required(args)
        devs_port = get_devices_port(args.board, args.devs_yml, args.hw_ext)
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
    parser_sn.add_argument(
        "--hw-ext", type=str, default=None, help="Required external hardware configuration"
    )
    parser_sn.set_defaults(func=parser_get_devices_serial_num)

    # Get devices port
    parser_port = subparser.add_parser("port", description="Get kitprog3 devices port list")
    parser_port.add_argument("-b", "--board", type=str, help="Board name")
    parser_port.add_argument(
        "-y", "--devs-yml", type=str, help="Device list yml with board - serial number map"
    )
    parser_port.add_argument(
        "--hw-ext", type=str, default=None, help="Required external hardware configuration"
    )
    parser_port.set_defaults(func=parser_get_devices_port)

    # Parser call
    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    parser()
