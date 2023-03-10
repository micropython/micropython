
var ws;
var input = document.getElementById("input");
input.value = "";
var title = document.querySelector("title");
var log = document.getElementById("log");

function set_enabled(enabled) {
  input.disabled = !enabled;
  var buttons = document.querySelectorAll("button");
  for (button of buttons) {
    button.disabled = !enabled;
  }
}

set_enabled(false);

function onSubmit() {
    ws.send("\r");
    input.value = "";
    input.focus();
}

ws = new WebSocket("ws://" + window.location.host + "/cp/serial/");

ws.onopen = function() {
  set_enabled(true);
};

var setting_title = false;
var encoder = new TextEncoder();
var left_count = 0;
ws.onmessage = function(e) {
  if (e.data == "\x1b]0;") {
    setting_title = true;
    title.textContent = "";
  } else if (e.data == "\x1b\\") {
    setting_title = false;
  } else if (setting_title) {
    title.textContent += e.data;
  } else if (e.data == "\b") {
    left_count += 1;
  } else if (e.data == "\x1b[K") { // Clear line
    log.textContent = log.textContent.slice(0, -left_count);
    left_count = 0;
  } else {
    log.textContent += e.data;
  }
  document.querySelector("span").scrollIntoView();
};

ws.onclose = function() {
  set_enabled(false);
};

ws.onerror = function(e) {
  set_enabled(false);
};

input.addEventListener("beforeinput", function(e) {
  if (e.inputType == "insertLineBreak") {
    ws.send("\r");
    input.value = "";
    input.focus();
    e.preventDefault();
  } else if (e.inputType == "insertText" || e.inputType == "insertFromPaste") {
    ws.send(e.data);
  } else if (e.inputType == "deleteContentBackward") {
    ws.send("\b");
  } else {
    console.log(e);
  }
});

let ctrl_c = document.querySelector("#c");
ctrl_c.onclick = function() {
  ws.send("\x03");
}

let ctrl_d = document.querySelector("#d");
ctrl_d.onclick = function() {
  ws.send("\x04");
}
