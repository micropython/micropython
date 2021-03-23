import sys
import cascadetoml
import pathlib
import typer
from jinja2 import Template


def main(input_template: pathlib.Path, output_path: pathlib.Path, skus: str = typer.Argument("")):
    if "," in skus:
        skus = skus.split(",")
    else:
        skus = [skus]
    skus = ['sku="{}"'.format(f.strip()) for f in skus]
    flashes = cascadetoml.filter_toml(pathlib.Path("../../data/nvm.toml"), skus)

    if len(skus) == 0:
        print("Set EXTERNAL_FLASH_DEVICES in mpconfigboard.mk with all possible flash skus")
        raise typer.Exit(code=1)

    def all_have(nvms, key):
        for nvm in nvms:
            if not nvm.get(key, False):
                return False
        return True

    def all_match(nvms, key, default=None):
        shared_value = nvms[0].get(key, default)
        for nvm in nvms:
            this_value = nvm.get(key, default)
            if this_value != shared_value:
                print(
                    "{}.{} = {} does not match {}".format(
                        nvm["sku"], key, this_value, shared_value
                    )
                )
                return None
        return shared_value

    quad_enable_status_byte = all_match(flashes["nvm"], "quad_enable_status_byte", None)
    quad_enable_bit_mask = all_match(flashes["nvm"], "quad_enable_bit_mask")
    continuous_status_write = all_have(flashes["nvm"], "01_continuous_status_write")
    split_status_write = all_have(flashes["nvm"], "write_status_register_split")
    e7_quad_word_read = all_have(flashes["nvm"], "e7_quad_word_read")

    quad_ok = quad_enable_status_byte is not None and quad_enable_bit_mask is not None

    max_clock_speed_mhz = min((x.get("max_clock_speed_mhz", 1000) for x in flashes["nvm"]))

    # Check that we have a consistent way to set quad enable.
    if continuous_status_write is None and split_status_write is None:
        print("quad not ok", continuous_status_write, split_status_write)
        quad_ok = False

    clock_divider = 4

    read_command = 0x03
    wait_cycles = 0
    if quad_ok:
        if e7_quad_word_read:
            read_command = 0xE7
            wait_cycles = 2
        else:
            read_command = 0xEB
            wait_cycles = 4

    flash_settings = {
        "quad_ok": quad_ok,
        "quad_enable_status_byte": quad_enable_status_byte,
        "quad_enable_bit_mask": quad_enable_bit_mask,
        "split_status_write": split_status_write,
        "clock_divider": clock_divider,
        "read_command": read_command,
        "wait_cycles": wait_cycles,
    }

    template = Template(input_template.read_text())

    output_path.write_text(template.render(flash_settings))


if __name__ == "__main__":
    typer.run(main)
