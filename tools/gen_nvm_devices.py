import sys
import cascadetoml
import pathlib
import typer
from jinja2 import Template


def main(input_template: pathlib.Path, output_path: pathlib.Path):
    flashes = cascadetoml.filter_toml(pathlib.Path("../../data/nvm.toml"), [])

    template = Template(input_template.read_text())

    settings = {"nvms": []}
    for flash in flashes["nvm"]:
        if "sku" not in flash or flash["sku"] == flash["manufacturer"]:
            continue
        settings["nvms"].append(dict(flash))

    output_path.write_text(template.render(settings))


if __name__ == "__main__":
    typer.run(main)
