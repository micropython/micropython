let $editor = document.querySelector("#code_textarea");
let filename = location.hash.substring(1);
let $output_text = document.querySelector("#output_text");

fetch(`/fs/${filename}`)
    .then(function (response) {
        $output_text.innerText = `Loading Status: ${response.status}`;
        return response.status === 200 ? response.text() : "";
    })
    .then(function (data) {
        $editor.value = data;
    });

function save() {
    $output_text.innerText = "Saving..."
    const requestOptions = {
        method: 'PUT',
        body: $editor.value
    };
    fetch(`/fs/${filename}`, requestOptions)
        .then(function (response) {
            $output_text.innerText = `Saving Status: ${response.status}`;
            return response.text();
        })
        .then(function (data) {
            console.log("after fetch: " + data);
        });
}

document.querySelector("#save_btn").onclick = function () {
    console.log("Click Save!");
    save();
}

let isCtrl = false;
document.onkeyup=function(e){
    if(e.keyCode === 17) isCtrl=false;
}

document.onkeydown=function(e){
    if(e.keyCode === 17) isCtrl=true;
    if(e.keyCode === 83 && isCtrl === true) {
        //ctrl-s pressed
        save();
        return false;
    }
}
