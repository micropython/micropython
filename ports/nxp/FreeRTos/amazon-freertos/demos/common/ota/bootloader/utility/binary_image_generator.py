import os
import argparse


def parseParams():
    parser = argparse.ArgumentParser()

    parser.add_argument('-d', '--dir', required=True, help="Directory containing objcopy tool.")
    parser.add_argument('-b', '--bin', required=True, help="Name of objcopy tool.")
    parser.add_argument('-p', '--params', required=True, help="parameters to be passed to objcopy tool.")

    args = vars(parser.parse_args())
    return args


def main():
    args = parseParams()

    objcopy_dir = args['dir']
    objcopy_bin = args['bin']
    objcopy_full_path = os.path.join(objcopy_dir, objcopy_bin)

    bin_converter_command = '"' + objcopy_full_path + '"' + ' ' + args['params']
    print('Executing.. {}'.format(bin_converter_command))
    os.system(bin_converter_command)


if __name__ == "__main__":
    main()
