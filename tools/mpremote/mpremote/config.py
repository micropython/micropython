import os
from contextlib import contextmanager


@contextmanager
def cwd_as(path):
    prev_cwd = os.getcwd()
    os.chdir(path)
    try:
        yield
    finally:
        os.chdir(prev_cwd)


def load_script(prog: str) -> dict:
    """Executes the ~/.config/mpremote/config.py configuration script to derive a configuration."""
    config = {
        'commands': {},
    }
    # Get config file name.
    path = os.getenv("XDG_CONFIG_HOME")
    if path is None:
        path = os.getenv("HOME")
        if path is None:
            return config
        path = os.path.join(path, ".config")
    path = os.path.join(path, prog)
    config_file = os.path.join(path, "config.py")

    # Check if config file exists.
    if not os.path.exists(config_file):
        return config

    # Exec the config file in its directory.
    with open(config_file) as f:
        config_data = f.read()
    with cwd_as(path):
        exec(config_data, config)

    return config


def load_environ(prog: str) -> dict:
    """Filters environment variables for and strips off their 'MPREMOTE_' prefix,
    and processes 'MPREMOTE_CMD_' prefixed vars into command-specific config entries.
    """
    env_prefix = prog.upper() + '_'
    cmd_prefix = env_prefix + 'CMD_'

    config = {}
    commands = {}

    for k in os.environ.keys():
        if k.startswith(cmd_prefix):
            commands[k.removeprefix(cmd_prefix).lower()] = os.environ[k]
        elif k.startswith(env_prefix):
            config[k.removeprefix(env_prefix).lower()] = os.environ[k]

    if commands:
        config['commands'] = commands
    return config


class Config:
    def __init__(self, prog='mpremote'):
        self.__dict__.update(load_script(prog))
        self.__dict__.update(load_environ(prog))
