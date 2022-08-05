/*
 *   This content is licensed according to the W3C Software License at
 *   https://www.w3.org/Consortium/Legal/2015/copyright-software-and-document
 *
 *   File:   sortable-table.js
 *
 *   Desc:   Adds sorting to a HTML data table that implements ARIA Authoring Practices
 */

// 'use strict';

var columnIndex = 2;
const sortEvent = new Event('sort');

class SortableTable {
  constructor(tableNode) {
    this.tableNode = tableNode;
  }

  setColumnHeaderSort(columnIndex) {
    function compareValues(a, b) {
        if (a.value === b.value) {
          return 0;
        } else {
          return a.value < b.value ? -1 : 1;
        }
    }

    var tbodyNode = this.tableNode.querySelector('tbody');
    var rowNodes = [];
    var dataCells = [];

    var rowNode = tbodyNode.firstElementChild;

    var index = 0;
    while (rowNode) {
      rowNodes.push(rowNode);
      var rowCells = rowNode.querySelectorAll('th, td');
      var dataCell = rowCells[columnIndex];

      var data = {};
      data.index = index;
      data.value = dataCell.textContent.toLowerCase().trim();
      dataCells.push(data);
      rowNode = rowNode.nextElementSibling;
      index += 1;
    }

    dataCells.sort(compareValues);

    // remove rows
    while (tbodyNode.firstChild) {
      tbodyNode.removeChild(tbodyNode.lastChild);
    }

    // add sorted rows
    for (var i = 0; i < dataCells.length; i += 1) {
      tbodyNode.appendChild(rowNodes[dataCells[i].index]);
    }
  }
}

var sortable_directory = document.querySelector('table.sortable');
const sd_class = {sortable_dir: new SortableTable(sortable_directory)};
sortable_directory.addEventListener('sort', function () { sd_class["sortable_dir"].setColumnHeaderSort(columnIndex); } );

let new_directory_name = document.getElementById("name");
let files = document.getElementById("files");

var url_base = window.location;
var current_path;
var editable = undefined;

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

    if (editable === undefined) {
        const status = await fetch(new URL("/fs/", url_base),
            {
                method: "OPTIONS",
                credentials: "include"
            }
        );
        editable = status.headers.get("Access-Control-Allow-Methods").includes("DELETE");
        new_directory_name.disabled = !editable;
        files.disabled = !editable;
        if (!editable) {
            let usbwarning = document.querySelector("#usbwarning");
            usbwarning.style.display = "block";
        }
    }

    if (window.location.path != "/fs/") {
        var clone = template.content.cloneNode(true);
        var td = clone.querySelectorAll("td");
        td[0].textContent = "🗀";
        var path = clone.querySelector("a");
        let parent = new URL("..", "file://" + current_path);
        path.href = "#" + parent.pathname;
        path.textContent = "..";
        // Remove the delete button
        td[4].replaceChildren();
        new_children.push(clone);
    }

    for (const f of data) {
        // Clone the new row and insert it into the table
        var clone = template.content.cloneNode(true);
        var td = clone.querySelectorAll("td");
        var icon = "⬇";
        var file_path = current_path + f.name;
        let api_url = new URL("/fs" + file_path, url_base);
        let edit_url = "/edit/#" + file_path;
        if (f.directory) {
            file_path = "#" + file_path + "/";
            api_url += "/";
        } else {
            file_path = api_url;
        }

        if (f.directory) {
            icon = "🗀";
        } else if(f.name.endsWith(".txt") ||
                  f.name.endsWith(".py") ||
                  f.name.endsWith(".js") ||
                  f.name.endsWith(".json")) {
            icon = "🖹";
        } else if (f.name.endsWith(".html")) {
            icon = "🌐";
        }
        td[0].textContent = icon;
        td[1].textContent = f.file_size;
        var path = clone.querySelector("a");
        path.href = file_path;
        path.textContent = f.name;
        td[3].textContent = (new Date(f.modified_ns / 1000000)).toLocaleString();
        var delete_button = clone.querySelector("button.delete");
        delete_button.value = api_url;
        delete_button.disabled = !editable;
        delete_button.onclick = del;

        if (editable) {
            edit_url = new URL(edit_url, url_base);
            let edit_link = clone.querySelector(".edit_link");
            edit_link.href = edit_url
        }

        new_children.push(clone);
    }
    var tbody = document.querySelector("tbody");
    tbody.replaceChildren(...new_children);

    sortable_directory.dispatchEvent(sortEvent);
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
    for (const file of files.files) {
        let file_path = new URL("/fs" + current_path + file.name, url_base);
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
            files.value = "";
            upload_button.disabled = true;
        }
    }
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

find_devices();

let mkdir_button = document.getElementById("mkdir");
mkdir_button.onclick = mkdir;

let upload_button = document.getElementById("upload");
upload_button.onclick = upload;

upload_button.disabled = files.files.length == 0;

files.onchange = () => {
    upload_button.disabled = files.files.length == 0;
}

mkdir_button.disabled = new_directory_name.value.length == 0;

new_directory_name.oninput = () => {
    mkdir_button.disabled = new_directory_name.value.length == 0;
}

window.onhashchange = refresh_list;
