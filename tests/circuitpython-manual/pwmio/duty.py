import math
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image
from PIL import Image
from PIL import ImageFont
from PIL import ImageDraw


def read(
    filename,
    image_filename=None,
    height=480,
    width=640,
    f_min=10,
    f_max=1e8,
    title="",
    subtitle="",
    version="",
    duty_labels=(0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9),
    freq_calls=tuple(),
    margin=0.01,
    duty_color=(255, 0, 0),
    freq_color=(0, 255, 0),
    calls_color=(0, 255, 255),
    title_color=(255, 255, 255),
):
    """Read a one channel logic analyzer raw csv data file and generate a plot visualizing the PWM signal
    captured in the file.  Each line of the file is a <time, level> pair indicating the times (in seconds)
    at which the signal transitioned to that level.  For example:
                           1.726313020,0
                           1.726313052,1
                           1.726313068,0
                           1.727328804,1
    """
    left = 80
    right = 80
    bottom = 20
    top = 60
    x0 = left
    y0 = top
    y1 = height - bottom
    x1 = width - right
    rising_edge = None
    falling_edge = None
    pixels = np.zeros((height, width, 3), dtype=np.uint8) * 255
    t0 = None
    t1 = None
    val = None
    with open(filename, "r") as f:
        first = True
        for line in f:  # find min and max t, excluding first and last values
            if val is not None:
                if not first:
                    if t0 is None or t < t0:
                        t0 = t
                    if t1 is None or t > t1:
                        t1 = t
                else:
                    first = False
            t, val = line.split(",")
            try:
                t = float(t)
                val = int(val)
            except ValueError:
                val = None
    print("plotting", t1 - t0, "seconds")

    with open(filename, "r") as f:
        pts = 0
        f_log_max = int(math.log10(f_max))
        f_log_min = int(math.log10(f_min))
        f_log_span = f_log_max - f_log_min
        for line in f:
            t, val = line.split(",")
            try:
                t = float(t)
                val = int(val)
            except ValueError:
                val = None
            if val == 1:
                if falling_edge is not None and rising_edge is not None:
                    period = t - rising_edge
                    frequency = 1 / period
                    duty_cycle = (falling_edge - rising_edge) / period
                    x = int((x1 - x0) * (t - t0) / (t1 - t0)) + x0
                    y_duty = int((1 - duty_cycle) * (y1 - y0)) + y0
                    y_freq = (
                        int((y1 - y0) * (1 - (math.log10(frequency) - f_log_min) / f_log_span))
                        + y0
                    )
                    x = max(x0, min(x, x1 - 1))
                    y_duty = max(y0, min(y_duty, y1 - 1))
                    y_freq = max(y0, min(y_freq, y1 - 1))
                    pixels[y_duty, x] = duty_color
                    pixels[y_freq, x] = freq_color
                    pts += 1
                rising_edge = t
            elif val == 0:
                falling_edge = t
        image = Image.fromarray(pixels)
        draw = ImageDraw.Draw(image)
        draw.text((left - 10, top), "Duty", duty_color, anchor="rt")
        draw.text((0, top), "Calls", calls_color, anchor="lt")
        draw.text((width - right / 2, top), "Freq", freq_color, anchor="mt")

        for duty in duty_labels:
            draw.text(
                (left - 10, y0 + (y1 - y0) * (1 - duty)),
                f"{int(100*duty):d}%",
                duty_color,
                anchor="rm",
            )
        for exponent in range(f_log_min + 1, f_log_max):
            draw.text(
                (width - right / 2, y0 + (y1 - y0) * (1 - (exponent - f_log_min) / (f_log_span))),
                str(10**exponent) + " Hz",
                freq_color,
                anchor="mm",
            )
        for freq, count in freq_calls:
            draw.text(
                (0, y0 + (y1 - y0) * (1 - (math.log10(freq) - f_log_min) / (f_log_span))),
                f"{count} Hz",
                calls_color,
                anchor="lm",
            )
        subtitle += f", showing {pts} PWM cycles"
        draw.text((width * 0.5, height * margin), title, title_color, anchor="mm")
        draw.text((width * 0.5, height * 4 * margin), version, title_color, anchor="mm")
        draw.text((width * 0.5, height * 8 * margin), subtitle, title_color, anchor="mm")
        image.show()
        if image_filename is not None:
            image.save(image_filename)
    return image
