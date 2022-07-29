var url_base = window.location;
var current_path;

var mdns_works = window.location.hostname.endsWith(".local");

async function find_devices() {
    var version_response = await fetch("/cp/version.json");
    if (version_response.ok) {
        url_base = new URL("/", window.location).href;
    } else {
        // TODO: Remove this when we've settled things. It is only used when this file isn't hosted
        // by a CP device.
        version_response = await fetch("http://circuitpython.local/cp/version.json", {mode: "cors"});
        mdns_works = mdns_works || version_response.redirected;
        if (!version_response.ok && version_response.redirected) {
            version_response = await fetch(version_response.url);
        }
        let url = new URL("/", version_response.url);
        url_base = url.href;
    }
    const version_info = await version_response.json();
    let version_span = document.querySelector("#version");
    version_span.textContent = version_info.version;
    let board_link = document.querySelector("#board");
    board_link.href = "https://circuitpython.org/board/" + version_info.board_id + "/";
    board_link.textContent = version_info.board_name;
    let hostname = document.querySelector("#hostname");
    var port = "";
    if (version_info.port != 80) {
        port = ":" + version_info.port;
    }
    hostname.href = "http://" + version_info.hostname + ".local" + port + "/";
    hostname.textContent = version_info.hostname;
    let ip = document.querySelector("#ip");
    ip.href = "http://" + version_info.ip + port + "/";
    ip.textContent = version_info.ip;
    const response = await fetch(new URL("/cp/devices.json", url_base));
    const data = await response.json();
    let device_list = document.querySelector("#devices");
    let new_devices = [];
    for (device of data.devices) {
        let li = document.createElement("li");
        let a = document.createElement("a");
        li.appendChild(a);
        var port = "";
        if (device.port != 80) {
            port = ":" + device.port;
        }
        var server;
        if (mdns_works) {
            server = device.hostname + ".local";
        } else {
            server = device.ip;
        }
        a.href = "http://" + server + port + "/";
        a.textContent = device.instance_name + " (" + device.hostname + ")";
        new_devices.push(li);
    }
    device_list.replaceChildren(...new_devices);
}

find_devices();
