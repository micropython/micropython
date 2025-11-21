#!/usr/bin/env python3
"""
MicroPython Unified CLI Tool
A single entry point for common MicroPython development tasks.
"""

import argparse
import sys
import os
import subprocess
import shutil

# Add the tools directory to sys.path to allow importing other scripts if needed
TOOLS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(TOOLS_DIR)

def run_command(cmd):
    """Runs a shell command and passes through input/output."""
    try:
        subprocess.check_call(cmd)
    except subprocess.CalledProcessError as e:
        sys.exit(e.returncode)
    except KeyboardInterrupt:
        sys.exit(1)

def cmd_flash(args):
    """Wrapper for upload_firmware.py"""
    print("Invoking Firmware Uploader...")
    uploader_path = os.path.join(TOOLS_DIR, "upload_firmware.py")
    cmd = [sys.executable, uploader_path] + args.extra_args
    run_command(cmd)

def cmd_repl(args):
    """Wrapper for mpremote (or pyboard.py as fallback)"""
    print("Starting REPL...")
    # Try to use mpremote if installed or available in tools
    # Check if mpremote is in tools/mpremote
    mpremote_dir = os.path.join(TOOLS_DIR, "mpremote")
    if os.path.exists(mpremote_dir):
        # Run mpremote from source
        cmd = [sys.executable, "-m", "mpremote"] + args.extra_args
        # We need to set PYTHONPATH to include tools/ for mpremote to run from source
        env = os.environ.copy()
        env["PYTHONPATH"] = TOOLS_DIR + os.pathsep + env.get("PYTHONPATH", "")
        try:
            subprocess.check_call(cmd, env=env)
        except subprocess.CalledProcessError as e:
            sys.exit(e.returncode)
    else:
        # Fallback to pyboard.py
        print("mpremote not found, falling back to pyboard.py")
        pyboard_path = os.path.join(TOOLS_DIR, "pyboard.py")
        cmd = [sys.executable, pyboard_path] + args.extra_args
        run_command(cmd)

def cmd_init(args):
    """Scaffolds a new MicroPython project."""
    project_name = args.name
    if os.path.exists(project_name):
        print(f"Error: Directory '{project_name}' already exists.")
        sys.exit(1)

    print(f"Creating new project: {project_name}")
    os.makedirs(project_name)
    os.makedirs(os.path.join(project_name, "lib"))
    
    # Create main.py
    with open(os.path.join(project_name, "main.py"), "w") as f:
        f.write("# MicroPython main.py\n")
        f.write("import time\n\n")
        f.write("print('Hello from MicroPython!')\n\n")
        f.write("while True:\n")
        f.write("    # time.sleep(1)\n")
        f.write("    pass\n")

    # Create boot.py
    with open(os.path.join(project_name, "boot.py"), "w") as f:
        f.write("# boot.py - runs on boot-up\n")
        f.write("# This file is executed on every boot (including wake-boot from deepsleep)\n")

    # Create .gitignore
    with open(os.path.join(project_name, ".gitignore"), "w") as f:
        f.write("__pycache__/\n")
        f.write("*.mpy\n")
        f.write(".vscode/\n")
        f.write("venv/\n")

    print("Project created successfully!")
    print(f"cd {project_name}")

def cmd_install(args):
    """Wrapper for mip (via mpremote or micropython-lib)."""
    print("Installing packages...")
    # Use mpremote mip
    cmd = [sys.executable, "-m", "mpremote", "mip", "install"] + args.packages
    env = os.environ.copy()
    env["PYTHONPATH"] = TOOLS_DIR + os.pathsep + env.get("PYTHONPATH", "")
    try:
        subprocess.check_call(cmd, env=env)
    except subprocess.CalledProcessError as e:
        sys.exit(e.returncode)

def main():
    parser = argparse.ArgumentParser(description="MicroPython Unified CLI")
    subparsers = parser.add_subparsers(dest="command", help="Command to run")

    # Flash command
    flash_parser = subparsers.add_parser("flash", help="Flash firmware to a device")
    flash_parser.add_argument("extra_args", nargs=argparse.REMAINDER, help="Arguments passed to upload_firmware.py")
    flash_parser.set_defaults(func=cmd_flash)

    # REPL command
    repl_parser = subparsers.add_parser("repl", help="Open a REPL connection")
    repl_parser.add_argument("extra_args", nargs=argparse.REMAINDER, help="Arguments passed to mpremote")
    repl_parser.set_defaults(func=cmd_repl)

    # Init command
    init_parser = subparsers.add_parser("init", help="Initialize a new project")
    init_parser.add_argument("name", help="Project name (directory to create)")
    init_parser.set_defaults(func=cmd_init)

    # Install command
    install_parser = subparsers.add_parser("install", help="Install packages using mip")
    install_parser.add_argument("packages", nargs="+", help="Packages to install")
    install_parser.set_defaults(func=cmd_install)

    args = parser.parse_args()

    if args.command:
        args.func(args)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
