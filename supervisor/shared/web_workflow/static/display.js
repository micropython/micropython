// Shows the device's display. It is sent with the Remote Framebuffer protocol
// (RFC 6143) over the websocket, as described in
// supervisor/shared/web_workflow/display_stream.h. This is a small viewer for
// what the device sends: Raw and Hextile.
// Wrapped so its names don't clash with serial.js on the same page.
(function() {
  var canvas = document.getElementById("screen");
  var context = canvas.getContext("2d");
  var view = document.getElementById("view");
  var status_text = document.getElementById("status");
  var ws;
  // Set when another page took the display over, so this one doesn't take it back.
  var replaced = false;

  var ENCODING_RAW = 0;
  var ENCODING_HEXTILE = 5;
  var ENCODING_DESKTOP_SIZE = -223;

  // Bytes received and not yet used.
  var input = new Uint8Array(0);
  var state;
  var image = null;
  var pixels = null;
  var rects_left = 0;
  var updates = 0;
  var received = 0;
  // Degrees clockwise the picture is turned. Only the page turns it, the device
  // sends the display as it is. Remembered by the browser where it can.
  var rotation = 0;
  try {
    rotation = (parseInt(localStorage.getItem("display_rotation")) || 0) % 360;
  } catch (e) {
  }

  // Fits the turned picture to the width of its column and three quarters of
  // the window's height.
  function layout() {
    var turned = rotation % 180 != 0;
    var shown_width = turned ? canvas.height : canvas.width;
    var shown_height = turned ? canvas.width : canvas.height;
    var scale = Math.min(view.clientWidth / shown_width, window.innerHeight * 0.75 / shown_height);
    view.style.height = shown_height * scale + "px";
    canvas.style.width = canvas.width * scale + "px";
    canvas.style.height = canvas.height * scale + "px";
    canvas.style.left = (view.clientWidth - canvas.width * scale) / 2 + "px";
    canvas.style.top = (shown_height - canvas.height) * scale / 2 + "px";
    canvas.style.transform = "rotate(" + rotation + "deg)";
  }

  function resize(width, height) {
    canvas.width = width;
    canvas.height = height;
    image = context.createImageData(width, height);
    pixels = new Uint32Array(image.data.buffer);
    pixels.fill(0xff000000);
    layout();
  }

  function send(bytes) {
    ws.send(new Uint8Array(bytes));
  }

  function u16(v) {
    return [(v >> 8) & 0xff, v & 0xff];
  }

  function u32(v) {
    return u16(v >>> 16).concat(u16(v & 0xffff));
  }

  function request_update(incremental) {
    send([3, incremental ? 1 : 0].concat(u16(0), u16(0), u16(canvas.width), u16(canvas.height)));
  }

  function take(n) {
    var bytes = input.subarray(0, n);
    input = input.subarray(n);
    return bytes;
  }

  function get16(b, i) {
    return (b[i] << 8) | b[i + 1];
  }

  function get32(b, i) {
    return ((b[i] << 24) | (b[i + 1] << 16) | (b[i + 2] << 8) | b[i + 3]) >>> 0;
  }

  // Pixels are asked for as 32 bits, red in the low byte, so they are canvas
  // pixels (RGBA in little endian memory) once alpha is set.
  function pixel(b, i) {
    return (0xff000000 | (b[i + 2] << 16) | (b[i + 1] << 8) | b[i]) >>> 0;
  }

  function fill_rect(x, y, w, h, v) {
    for (var j = y; j < y + h; j++) {
      pixels.fill(v, j * canvas.width + x, j * canvas.width + x + w);
    }
  }

  // Draws the w x h pixels at offset p of b.
  function put_raw(x, y, w, h, b, p) {
    for (var j = 0; j < h; j++) {
      for (var i = 0; i < w; i++, p += 4) {
        pixels[(y + j) * canvas.width + x + i] = pixel(b, p);
      }
    }
  }

  // Draws the Hextile rectangle after the header at the start of the input.
  // Nothing says how long it is, so returns its length if all of it is here and
  // 0 if not. What it drew by then is the same when it's tried again.
  function draw_hextile(x, y, w, h) {
    var p = 12;
    var background = 0;
    var foreground = 0;
    for (var ty = 0; ty < h; ty += 16) {
      for (var tx = 0; tx < w; tx += 16) {
        var tw = Math.min(16, w - tx);
        var th = Math.min(16, h - ty);
        if (p >= input.length) {
          return 0;
        }
        var mask = input[p++];
        if (mask & 1) {
          if (p + tw * th * 4 > input.length) {
            return 0;
          }
          put_raw(x + tx, y + ty, tw, th, input, p);
          p += tw * th * 4;
          continue;
        }
        var coloured = mask & 16;
        // The colours and the subrectangle count that come before the subrectangles.
        var header = ((mask & 2) ? 4 : 0) + ((mask & 4) ? 4 : 0) + ((mask & 8) ? 1 : 0);
        if (p + header > input.length) {
          return 0;
        }
        if (mask & 2) {
          background = pixel(input, p);
          p += 4;
        }
        if (mask & 4) {
          foreground = pixel(input, p);
          p += 4;
        }
        var count = (mask & 8) ? input[p++] : 0;
        if (p + count * (coloured ? 6 : 2) > input.length) {
          return 0;
        }
        fill_rect(x + tx, y + ty, tw, th, background);
        for (var i = 0; i < count; i++) {
          var v = foreground;
          if (coloured) {
            v = pixel(input, p);
            p += 4;
          }
          var at = input[p++];
          var size = input[p++];
          fill_rect(x + tx + (at >> 4), y + ty + (at & 15), (size >> 4) + 1, (size & 15) + 1, v);
        }
      }
    }
    return p;
  }

  // Uses what it can of the input. Returns false when it needs more.
  function step() {
    if (state == "version") {
      if (input.length < 12) {
        return false;
      }
      take(12);
      send(Array.from(new TextEncoder().encode("RFB 003.008\n")));
      state = "security";
    } else if (state == "security") {
      if (input.length < 1 || input.length < 1 + input[0]) {
        return false;
      }
      var types = take(1 + input[0]).subarray(1);
      if (types.indexOf(1) < 0) {
        throw "no security type this page can use";
      }
      send([1]);
      state = "result";
    } else if (state == "result") {
      if (input.length < 4) {
        return false;
      }
      if (get32(take(4), 0) != 0) {
        throw "refused";
      }
      // Shared, though the device takes over from other viewers anyway.
      send([1]);
      state = "init";
    } else if (state == "init") {
      if (input.length < 24 || input.length < 24 + get32(input, 20)) {
        return false;
      }
      var init = take(24 + get32(input, 20));
      resize(get16(init, 0), get16(init, 2));
      // 32 bits per pixel, depth 24, little endian, true colour, 8 bits each
      // with red lowest.
      send([0, 0, 0, 0, 32, 24, 0, 1].concat(u16(255), u16(255), u16(255), [0, 8, 16, 0, 0, 0]));
      var encodings = [ENCODING_HEXTILE, ENCODING_RAW, ENCODING_DESKTOP_SIZE];
      var message = [2, 0].concat(u16(encodings.length));
      for (var i = 0; i < encodings.length; i++) {
        message = message.concat(u32(encodings[i] >>> 0));
      }
      send(message);
      request_update(false);
      state = "message";
    } else if (state == "message") {
      if (input.length < 1) {
        return false;
      }
      if (input[0] == 0) {
        if (input.length < 4) {
          return false;
        }
        rects_left = get16(take(4), 2);
        state = "rect";
      } else if (input[0] == 2) {
        take(1);
      } else if (input[0] == 3) {
        if (input.length < 8 || input.length < 8 + get32(input, 4)) {
          return false;
        }
        take(8 + get32(input, 4));
      } else {
        throw "unknown message " + input[0];
      }
    } else if (state == "rect") {
      if (rects_left == 0) {
        context.putImageData(image, 0, 0);
        updates++;
        state = "message";
        request_update(true);
        return true;
      }
      if (input.length < 12) {
        return false;
      }
      var x = get16(input, 0), y = get16(input, 2), w = get16(input, 4), h = get16(input, 6);
      var encoding = get32(input, 8) | 0;
      if (encoding == ENCODING_RAW) {
        if (input.length < 12 + w * h * 4) {
          return false;
        }
        put_raw(x, y, w, h, input, 12);
        take(12 + w * h * 4);
      } else if (encoding == ENCODING_HEXTILE) {
        var length = draw_hextile(x, y, w, h);
        if (length == 0) {
          return false;
        }
        take(length);
      } else if (encoding == ENCODING_DESKTOP_SIZE) {
        take(12);
        resize(w, h);
      } else {
        throw "unknown encoding " + encoding;
      }
      rects_left--;
    }
    return true;
  }

  function receive(data) {
    received += data.length;
    var joined = new Uint8Array(input.length + data.length);
    joined.set(input);
    joined.set(data, input.length);
    input = joined;
    try {
      while (step()) {
      }
    } catch (e) {
      status_text.textContent = "Error: " + e;
      ws.close();
    }
  }

  function connect() {
    status_text.textContent = "Connecting";
    replaced = false;
    input = new Uint8Array(0);
    state = "version";
    ws = new WebSocket("ws://" + window.location.host + "/cp/display/");
    ws.binaryType = "arraybuffer";
    ws.onmessage = function(e) {
      receive(new Uint8Array(e.data));
    };
    ws.onclose = function(e) {
      if (e.code == 4000) {
        replaced = true;
        status_text.textContent = "Another viewer took over the display. Refresh to show it here.";
        return;
      }
      status_text.textContent = "Disconnected";
      setTimeout(connect, 2000);
    };
  }

  var KEYSYMS = {
    "Enter": 0xff0d, "Backspace": 0xff08, "Tab": 0xff09, "Escape": 0xff1b,
    "ArrowLeft": 0xff51, "ArrowUp": 0xff52, "ArrowRight": 0xff53, "ArrowDown": 0xff54,
    "Home": 0xff50, "End": 0xff57, "Delete": 0xffff, "Control": 0xffe3
  };

  function key_event(e, down) {
    var keysym = KEYSYMS[e.key];
    if (keysym === undefined && e.key.length == 1) {
      keysym = e.key.charCodeAt(0);
    }
    if (keysym === undefined || !ws || ws.readyState != WebSocket.OPEN || state == "version") {
      return;
    }
    e.preventDefault();
    send([4, down ? 1 : 0, 0, 0].concat(u32(keysym)));
  }

  // Typing on the picture goes to the serial console.
  canvas.tabIndex = 0;
  canvas.addEventListener("keydown", function(e) {
    key_event(e, true);
  });
  canvas.addEventListener("keyup", function(e) {
    key_event(e, false);
  });

  document.getElementById("refresh").onclick = function() {
    if (ws && ws.readyState == WebSocket.OPEN && image != null) {
      request_update(false);
    } else if (replaced) {
      connect();
    }
  };

  function rotate(degrees) {
    rotation = degrees % 360;
    try {
      if (rotation == 0) {
        localStorage.removeItem("display_rotation");
      } else {
        localStorage.setItem("display_rotation", rotation);
      }
    } catch (e) {
    }
    layout();
  }

  document.getElementById("rotate").onclick = function() {
    rotate(rotation + 90);
  };
  // Back to the display as the device sends it.
  document.getElementById("unrotate").onclick = function() {
    rotate(0);
  };
  window.addEventListener("resize", layout);
  layout();

  setInterval(function() {
    if (ws && ws.readyState == WebSocket.OPEN && image != null) {
      status_text.textContent = canvas.width + "x" + canvas.height + ", " +
        updates + " updates/s, " + (received / 1024).toFixed(1) + " KB/s";
    }
    updates = 0;
    received = 0;
  }, 1000);

  connect();
})();
