console.log("Hello world");

let new_directory_name = document.getElementById("name");
let files = document.getElementById("files");


function mkdir(e) {
    console.log("mkdir");
    console.log(e);
}

function upload(e) {
    console.log("upload");
    console.log(files.files);
}

function del(e) {
    console.log("delete");
    console.log(e);
    if (confirm("Delete " + e.target.value + " and all of its contents?")) {
        console.log("do it");
    }
}

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

const collection = document.getElementsByClassName("delete");
for (let i = 0; i < collection.length; i++) {
  collection[i].onclick = del;
}
