
var ws;
var input = document.getElementById("input");
var title = document.querySelector("title");

function set_enabled(enabled) {
  input.disabled = !enabled;
  var buttons = document.querySelectorAll("button");
  for (button of buttons) {
    button.disabled = !enabled;
  }
}

set_enabled(false);

function onSubmit() {
  console.log("submit");
  // You can send message to the Web Socket using ws.send.
  ws.send(input.value);
  // output("send: " + input.value);
  input.value = "";
  input.focus();
}

function onCloseClick() {
  console.log("close clicked");
  ws.close();
}

function output(str) {
  var log = document.getElementById("log");
  log.innerHTML += str;
}

// Connect to Web Socket
ws = new WebSocket("ws://cpy-f57ce8.local/cp/serial/");
// ws = new WebSocket("ws://127.0.0.1:9001")

// Set event handlers.
ws.onopen = function() {
  console.log("onopen");
  set_enabled(true);
};

var setting_title = false;
ws.onmessage = function(e) {
  // e.data contains received string.
  if (e.data == "\x1b]0;") {
    setting_title = true;
    title.textContent = "";
  } else if (e.data == "\x1b\\") {
    setting_title = false;
  } else if (setting_title) {
    title.textContent += e.data;
  } else {
    output(e.data);
  }
};

ws.onclose = function() {
  console.log("onclose");
  set_enabled(false);
};

ws.onerror = function(e) {
  // output("onerror");
  console.log(e);
  set_enabled(false);
};

input.onbeforeinput = function(e) {
  console.log(e);
}
