import array

try:
    from synthio import MidiTrack
    from audiocore import get_buffer, get_structure
except ImportError:
    print("SKIP")
    raise SystemExit

SCORE = b"\0\x90@\0\x20\x90b\0\x20\x80@\0\0\x80\b\0"

with MidiTrack(SCORE, sample_rate=8000, tempo=640) as m:
    print(get_structure(m))
    p, q = get_buffer(m)
    print(p, list(q))

with MidiTrack(
    SCORE, sample_rate=8000, tempo=640, waveform=array.array("h", [0, 32767, 0, -32768])
) as m:
    print(get_structure(m))
    p, q = get_buffer(m)
    print(p, list(q))
