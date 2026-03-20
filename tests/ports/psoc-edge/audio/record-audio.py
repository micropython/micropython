import os, subprocess, argparse
import sys

# MicroPython needs to be installed in the PSOC EDGE E84 Kit
# mpremote (1.27) and ffmpeg (6.1.1-3ubuntu5) need to be
# installed on the local machine

# We will generate a file with the configuration to record the audio
# The file will be loaded. The script will be run, the audio will
# be recorded and played. This can be run one by one to check all values.


def load_file_on_target(filename, device):
    """
    Load the file to the target using mpremote:

        mpremote cp $local_filename $device:$filename
    """
    command = f"mpremote {device} fs cp {filename} :{filename}"
    subprocess.run(command, shell=True)


def run_script_on_target(script, device):
    """
    Run the script on the target using mpremote:

        mpremote run $script
    """
    command = f"mpremote {device} run {script}"
    subprocess.run(command, shell=True)


def remove_file_from_host(filename):
    """
    Remove the file from the host using os.remove:

        os.remove($filename)
    """
    if os.path.exists(filename):
        os.remove(filename)


def remove_file_from_target(filename, device):
    """
    Remove the file from the target using mpremote:

        mpremote fs rm $device:$filename
    """
    command = f"mpremote {device} fs rm :{filename}"
    subprocess.run(command, shell=True)


def load_file_from_target(filename, device):
    """
    Load the file from the target using mpremote:

        mpremote cp $device:$filename $local_filename
    """
    command = f"mpremote {device} fs cp :{filename} {filename}"
    subprocess.run(command, shell=True)


def play_audio(file, bits_per_sample, rate, channels):
    """
    Play it using ffplay:

        ffplay -nodisp -f $format -ar $rate -ac $channels -autoexit $file

    ffmpeg (6.1.1-3ubuntu5) needs to be installed on the local machine
    """
    pcm_fmt = "s16le" if bits_per_sample == 16 else "s32le"
    command = f"ffplay -nodisp -f {pcm_fmt} -ar {rate} -ac {channels} -autoexit {file}"
    subprocess.run(command, shell=True)


def cleanup(audio_file, conf_file, device):
    print(">> Removing previous audio file from host...")
    remove_file_from_host(audio_file)
    remove_file_from_host(conf_file)

    print(">> Removing previous audio file from target...")
    remove_file_from_target(audio_file, device)
    remove_file_from_target(conf_file, device)


def create_config_file(
    config_file, audio_file, time_sec, sample_rate, bits_per_sample, audio_format, mode
):
    mode = "blocking" if mode == 0 else "non-blocking"
    config_content = f"""user_config = {{
        "sample_rate": {sample_rate},
        "bits_per_sample": {bits_per_sample},
        "format": {audio_format},
        "mode": "{mode}",
        "audio_file": "{audio_file}",
        "time_sec": {time_sec}
    }}
    """
    with open(config_file, "w") as f:
        f.write(config_content)


def get_param_from_config(config_file, param):
    try:
        namespace = {}
        with open(config_file, "r") as f:
            exec(f.read(), namespace)
        return namespace["user_config"][param]
    except Exception as e:
        sys.exit(
            f"Error: {e}\nNo audio record configuration available.\nMake sure you have run the recording step first."
        )


def record(audio_script, conf_file, device):
    print(">> Loading audio configuration file to target...")
    load_file_on_target(conf_file, device)

    print(">> Running script on target to record audio...")
    run_script_on_target(audio_script, device)


def load(audio_file, device):
    print(">> Loading recorded audio from target...")
    load_file_from_target(audio_file, device)


def play(audio_file, bits_per_sample, sample_rate, format):
    print(">> Playing recorded audio...")
    play_audio(audio_file, bits_per_sample, sample_rate, format)


def cli():
    def _add_common_args(p):
        p.add_argument("-d", "--device", default="a0", help="Device name (default: a0)")

    def _add_recording_args(p):
        _add_common_args(p)
        p.add_argument(
            "-o", "--audio-file", default="audio.raw", help="Audio file name (default: audio.raw)"
        )
        p.add_argument(
            "-s",
            "--audio-script",
            default="record-target.py",
            help="Audio script name (default: record-target.py)",
        )
        p.add_argument(
            "-t", "--time-sec", type=int, default=5, help="Recording time in seconds (default: 5)"
        )
        p.add_argument(
            "-r",
            "--sample-rate",
            type=int,
            default=16000,
            choices=[8000, 16000, 32000, 44100, 48000],
            help="Sample rate in Hz (default: 16000)",
        )
        p.add_argument(
            "-b",
            "--bits-per-sample",
            type=int,
            default=16,
            choices=[16, 32],
            help="Bits per sample (default: 16)",
        )
        p.add_argument(
            "-f",
            "--format",
            type=int,
            default=2,
            choices=[1, 2],
            help="Audio format: 1=MONO, 2=STEREO (default: 2)",
        )
        p.add_argument(
            "-m",
            "--mode",
            type=int,
            default=0,
            choices=[0, 1],
            help="Mode: 0=blocking, 1=non-blocking (default: 0)",
        )

    parser = argparse.ArgumentParser(
        description="Record audio on MicroPython device and play it on the host"
    )
    _add_recording_args(parser)

    subparsers = parser.add_subparsers(dest="command")

    record_parser = subparsers.add_parser("record", help="Record audio on the target")
    _add_recording_args(record_parser)

    load_parser = subparsers.add_parser(
        "load", help="Load recorded audio from the target to the host"
    )
    _add_common_args(load_parser)

    play_parser = subparsers.add_parser("play", help="Play recorded audio on the host")
    _add_common_args(play_parser)

    clean_parser = subparsers.add_parser("clean", help="Clean audio files from host and target")

    return parser.parse_args()


def main(args, conf_file):
    cleanup(args.audio_file, conf_file, args.device)
    create_config_file(
        conf_file,
        args.audio_file,
        args.time_sec,
        args.sample_rate,
        args.bits_per_sample,
        args.format,
        args.mode,
    )
    record(args.audio_script, conf_file, args.device)
    load(args.audio_file, args.device)
    play(args.audio_file, args.bits_per_sample, args.sample_rate, args.format)


if __name__ == "__main__":
    config_file = "audio_conf.py"

    args = cli()

    if args.command == "record":
        create_config_file(
            config_file,
            args.audio_file,
            args.time_sec,
            args.sample_rate,
            args.bits_per_sample,
            args.format,
            args.mode,
        )
        record(args.audio_script, config_file, args.device)
    elif args.command == "load":
        audio_file = get_param_from_config(config_file, "audio_file")
        load(audio_file, args.device)
    elif args.command == "play":
        audio_file = get_param_from_config(config_file, "audio_file")
        bits_per_sample = get_param_from_config(config_file, "bits_per_sample")
        sample_rate = get_param_from_config(config_file, "sample_rate")
        format = get_param_from_config(config_file, "format")
        play(audio_file, bits_per_sample, sample_rate, format)
    elif args.command == "clean":
        cleanup(get_param_from_config(config_file, "audio_file"), config_file, args.device)
    else:
        main(args, config_file)
