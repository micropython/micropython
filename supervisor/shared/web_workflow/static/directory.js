let new_directory_name = document.getElementById("name");
let files = document.getElementById("files");
let dirs = document.getElementById("dirs");

var url_base = window.location;
var current_path;

// From: https://stackoverflow.com/questions/10420352/converting-file-size-in-bytes-to-human-readable-string
function humanFileSize(bytes) {
  const thresh = 1000;

  if (Math.abs(bytes) < thresh) {
    return bytes + ' B';
  }

  const units = ['kB', 'MB', 'GB', 'TB'];
  let u = -1;
  const r = 10;

  do {
    bytes /= thresh;
    ++u;
  } while (Math.round(Math.abs(bytes) * r) / r >= thresh && u < units.length - 1);


  return bytes.toFixed(1) + ' ' + units[u];
}


function compareValues(a, b) {
    if (a.directory == b.directory && a.name.toLowerCase() === b.name.toLowerCase()) {
      return 0;
    } else if (a.directory != b.directory) {
      return a.directory < b.directory ? 1 : -1;
    } else {
      return a.name.toLowerCase() < b.name.toLowerCase() ? -1 : 1;
    }
}

function set_upload_enabled(enabled) {
    files.disabled = !enabled;
    dirs.disabled = !enabled;
}

async function refresh_list() {
    current_path = window.location.hash.substr(1);
    if (current_path == "") {
        current_path = "/";
    }
    // Do the fetch first because the browser will prompt for credentials.
    const response = await fetch(new URL("/fs" + current_path, url_base),
        {
            headers: {
                "Accept": "application/json"
            },
            credentials: "include"
        }
    );
    const data = await response.json();
    var new_children = [];
    var title = document.querySelector("title");
    title.textContent = current_path;
    var path = document.querySelector("#path");
    path.textContent = current_path;
    var template = document.querySelector('#row');
    var disk_usage = document.querySelector('#usage');

    let used = humanFileSize((data.total - data.free) * data.block_size);
    let total = humanFileSize(data.total * data.block_size);
    disk_usage.textContent = `${used} out of ${total}`;

    let editable = data.writable;
    new_directory_name.disabled = !editable;
    set_upload_enabled(editable);
    let usbwarning = document.querySelector("#usbwarning");
    if (!editable) {
        usbwarning.style.display = "block";
    } else {
        usbwarning.style.display = "none";
    }

    if (current_path != "/") {
        var clone = template.content.cloneNode(true);
        var td = clone.querySelectorAll("td");
        td[0].textContent = "📁";
        var path = clone.querySelector("a");
        let parent = new URL("..", "file://" + current_path);
        path.href = "#" + parent.pathname;
        path.textContent = "..";
        // Remove the delete button
        td[4].replaceChildren();
        td[5].replaceChildren();
        td[6].replaceChildren();
        new_children.push(clone);
    }

    data.files.sort(compareValues);

    for (const f of data.files) {
        // Clone the new row and insert it into the table
        var clone = template.content.cloneNode(true);
        var td = clone.querySelectorAll("td");
        var icon = "⬇️";
        var file_path = current_path + f.name;
        let api_url = new URL("/fs" + file_path, url_base);
        let edit_url = "/edit/#" + file_path;
        if (f.directory) {
            file_path = "#" + file_path + "/";
            api_url += "/";
        } else {
            file_path = api_url;
        }

        var text_file = false;
        if (f.directory) {
            icon = "📁";
        } else if(f.name.endsWith(".txt") ||
                  f.name.endsWith(".toml") ||
                  f.name.endsWith(".py") ||
                  f.name.endsWith(".js") ||
                  f.name.endsWith(".json")) {
            icon = "📄";
            text_file = true;
        } else if (f.name.endsWith(".html")) {
            icon = "🌐";
            text_file = true;
        }
        td[0].textContent = icon;
        td[1].textContent = humanFileSize(f.file_size);
        var path = clone.querySelector("a.path");
        path.href = file_path;
        path.textContent = f.name;
        let modtime = clone.querySelector("td.modtime");
        modtime.textContent = (new Date(f.modified_ns / 1000000)).toLocaleString();
        var delete_button = clone.querySelector("button.delete");
        delete_button.value = api_url;
        delete_button.disabled = !editable;
        delete_button.onclick = del;


        var rename_button = clone.querySelector("button.rename");
        rename_button.value = api_url;
        rename_button.disabled = !editable;
        rename_button.onclick = rename;

        let edit_link = clone.querySelector(".edit_link");
        if (text_file && editable && !f.directory) {
            edit_url = new URL(edit_url, url_base);
            edit_link.href = edit_url
        } else if (f.directory) {
            edit_link.style = "display: none;";
        } else {
            edit_link.querySelector("button").disabled = true;
        }

        new_children.push(clone);
    }
    var tbody = document.querySelector("tbody");
    tbody.replaceChildren(...new_children);
}

async function find_devices() {
    const version_response = await fetch("/cp/version.json");
    if (version_response.ok) {
        url_base = new URL("/", window.location).href;
    } else {
        // TODO: Remove this when we've settled things. It is only used when this file isn't hosted
        // by a CP device.
        const response = await fetch("http://circuitpython.local/cp/devices.json");
        let url = new URL("/", response.url);
        url_base = url.href;
        const data = await response.json();
    }
    refresh_list();
}

async function mkdir(e) {
    const response = await fetch(
        new URL("/fs" + current_path + new_directory_name.value + "/", url_base),
        {
            method: "PUT",
            headers: {
                'X-Timestamp': Date.now()
            }
        }
    );
    if (response.ok) {
        refresh_list();
        new_directory_name.value = "";
        mkdir_button.disabled = true;
    }
}

async function upload(e) {
    set_upload_enabled(false);
    let progress = document.querySelector("#progress");
    let made_dirs = new Set();
    let total = files.files.length + dirs.files.length;

    var uploaded = 0;
    var failed = 0;
    progress.textContent = `Uploaded ${uploaded} out of ${total} files`;
    for (const file of [...files.files, ...dirs.files]) {
        let file_name = file.name;
        if (file.webkitRelativePath) {
            file_name = file.webkitRelativePath;
            let components = file_name.split("/");
            components.pop();
            let parent_dir = components.join("/");
            if (!made_dirs.has(parent_dir)) {
                new_directory_name.value = parent_dir;
                await mkdir(null);
                made_dirs.add(parent_dir);
            }
        }
        let file_path = new URL("/fs" + current_path + file_name, url_base);
        const response = await fetch(file_path,
            {
                method: "PUT",
                headers: {
                    'Content-Type': 'application/octet-stream',
                    'X-Timestamp': file.lastModified
                },
                body: file
            }
        )
        if (response.ok) {
            refresh_list();
            uploaded += 1;
        } else {
            failed += 1;
        }
        progress.textContent = `Uploaded ${uploaded} out of ${total} files`;
    }
    var s = "";
    if (failed > 0) {
        if (failed > 1) {
            s = "s";
        }
        progress.textContent = `${failed} upload${s} failed`;
    }
    files.value = "";
    dirs.value = "";
    set_upload_enabled(true);
}

async function del(e) {
    let fn = new URL(e.target.value);
    var prompt = "Delete " + fn.pathname.substr(3);
    if (e.target.value.endsWith("/")) {
        prompt += " and all of its contents?";
    } else {
        prompt += "?";
    }
    if (confirm(prompt)) {
        const response = await fetch(e.target.value,
            {
                method: "DELETE"
            }
        )
        if (response.ok) {
            refresh_list();
        }
    }
}

async function rename(e) {
    let fn = new URL(e.target.value);
    var new_fn = prompt("Rename to ", fn.pathname.substr(3));
    if (new_fn === null) {
        return;
    }
    let new_uri = new URL("/fs" + new_fn, fn);
    const response = await fetch(e.target.value,
        {
            method: "MOVE",
            headers: {
                'X-Destination': new_uri.pathname,
            },
        }
    )
    if (response.ok) {
        refresh_list();
    }
}

find_devices();

let mkdir_button = document.getElementById("mkdir");
mkdir_button.onclick = mkdir;

files.onchange = upload;
dirs.onchange = upload;

mkdir_button.disabled = new_directory_name.value.length == 0;

new_directory_name.oninput = () => {
    mkdir_button.disabled = new_directory_name.value.length == 0;
}

window.onhashchange = refresh_list;

window.addEventListener("pageshow", function (event) {
  var historyTraversal = event.persisted ||
                         (typeof window.performance != "undefined" &&
                              window.performance.navigation.type === 2);
  if (historyTraversal) {
    // Handle page restore.
    window.location.reload();
  }
});
