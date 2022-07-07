
var ws;

function onSubmit() {
  var input = document.getElementById("input");
  // You can send message to the Web Socket using ws.send.
  ws.send(input.value);
  // output("send: " + input.value);
  input.value = "";
  input.focus();
}

function onCloseClick() {
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
};

ws.onmessage = function(e) {
  // e.data contains received string.
  output(e.data);
};

ws.onclose = function() {
  console.log("onclose");
};

ws.onerror = function(e) {
  // output("onerror");
  console.log(e)
};
