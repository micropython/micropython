console.log("Hello world");

function mkdir(e) {
    console.log("mkdir");
    console.log(e);
}

function upload(e) {
    console.log("upload");
    console.log(e);
}

function del(e) {
    console.log("delete");
    console.log(e);
    if (confirm("Delete" + e.target.value + "?")) {
        console.log("do it");
    }
}

document.getElementById("mkdir").onclick = mkdir;
document.getElementById("upload").onclick = upload;

const collection = document.getElementsByClassName("delete");
for (let i = 0; i < collection.length; i++) {
  collection[i].onclick = del;
}
