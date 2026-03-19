import os, subprocess

# MicroPython needs to be installed in the PSOC EDGE E84 Kit
# mpremote (1.27) and ffmpeg (6.1.1-3ubuntu5) need to be
# installed on the local machine

# Parameters for recording
# format=s16le
# rate=16000
# channels=2
# file=sample2.raw

# sample_rate=(8000 16000 32000 44100 48000)
# bits_per_sample=(8 16 24 32)
# channels=(1 2)

# We will generate a file with the configuration to record the audio
# The file will be loaded. The script will be run, the audio will
# be recorded and played. This can be run one by one to check all values.


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


def play_audio(file, format, rate, channels):
    """
    Play it using ffplay:

        ffplay -nodisp -f $format -ar $rate -ac $channels -autoexit $file

    ffmpeg (6.1.1-3ubuntu5) needs to be installed on the local machine
    """
    command = f"ffplay -nodisp -f {format} -ar {rate} -ac {channels} -autoexit {file}"
    subprocess.run(command, shell=True)


def main(audio_file, device):
    print(">> Removing previous audio file from host...")
    remove_file_from_host(audio_file)

    print(">> Removing previous audio file from target...")
    remove_file_from_target(audio_file, device)

    print(">> Running script on target to record audio...")
    run_script_on_target("record-target.py", device)

    print(">> Loading recorded audio from target...")
    load_file_from_target(audio_file, device)

    print(">> Playing recorded audio...")
    play_audio(audio_file, "s16le", 16000, 2)


if __name__ == "__main__":
    # Add a simple option to run only play audio without recording
    # if the audio file already exists
    audio_file = "audio.raw"
    device = "a0"

    args = os.sys.argv[1:]
    if len(args) > 0 and args[0] == "play":
        print(">> Playing recorded audio...")
        play_audio(audio_file, "s16le", 16000, 2)
    elif len(args) > 0 and args[0] == "record":
        print(">> Running script on target to record audio...")
        run_script_on_target("record-target.py", device)
    elif len(args) > 0 and args[0] == "load":
        print(">> Loading recorded audio from target...")
        load_file_from_target(audio_file, device)
    else:
        main(audio_file, device)
