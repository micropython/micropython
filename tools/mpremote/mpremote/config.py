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
    import platformdirs

    config = {
        'commands': {},
    }
    path = platformdirs.user_config_dir(appname=prog, appauthor=False)
    config_file = os.path.join(path, "config.py")

    if not os.path.exists(config_file):
        return config

    # pass in the config path so that the config file can use it
    config["config_path"] = path
    config["__file__"] = config_file

    # Exec the config file in its directory.
    with open(config_file) as f:
        config_data = f.read()
    with cwd_as(path):
        exec(config_data, config)

    return config


def load_dotenv(prog: str) -> dict:
    """Searches up the directory tree for a file called '.mpremote' and parses it with .env syntax."""
    import dotenv

    path = dotenv.find_dotenv(f'.{prog}', usecwd=True)
    if path is None:
        return {}

    return dotenv.dotenv_values(path)


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
        self.__dict__.update(load_dotenv(prog))
        self.__dict__.update(load_environ(prog))
