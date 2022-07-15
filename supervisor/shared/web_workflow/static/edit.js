let editor;
require(["ace/ace", "ace/ext/settings_menu"], function (ace) {
    editor = ace.edit("code_textarea");
    ace.config.set('basePath', 'https://cdnjs.cloudflare.com/ajax/libs/ace/1.6.0/');
    console.log("after create editor");
    console.log(editor);

    editor.session.setMode("ace/mode/python");
    ace.require('ace/ext/settings_menu').init(editor);
    editor.commands.addCommands([{
        name: "showSettingsMenu",
        bindKey: {win: "Ctrl-e", mac: "Ctrl-e"},
        exec: function (editor) {
            console.log("ctrl-e")
            editor.showSettingsMenu();
        },
        readOnly: true
    }, {
        name: "infoDocsSearch",
        bindKey: {win: "Ctrl-i", mac: "Ctrl-i"},
        exec: function (editor) {
            window.open(`https://docs.circuitpython.org/en/latest/search.html?q=${editor.getSelectedText()}`, '_blank');
        },
        readOnly: true
    },{
        name: 'Save',
        bindKey: {win: 'Ctrl-S', mac: 'Command-S'},
        exec: function (editor) {
            console.log("ctrl-s save");
            save();

        }
    },{
        name: "replaceCtrlR",
        bindKey: {win: "Ctrl-r", mac: "Ctrl-r"},
        exec: function (editor_arg) {
            console.log("override ctrl r");
            editor.execCommand('replace');
            console.log(editor);
        },
        readOnly: true
    }]);


});

let filename = location.hash.substring(1);
let $output_text = document.querySelector("#output_text");
/*let $code_text = document.querySelector("#code_textarea");*/

fetch(`/fs/${filename}`)
    .then(function (response) {
        $output_text.innerText = `Loading Status: ${response.status}`;
        return response.status === 200 ? response.text() : "";
    })
    .then(function (data) {
        editor.setValue(data, -1)
    });

function save() {
    $output_text.innerText = "Saving..."
    const requestOptions = {
        method: 'PUT',
        body: editor.getValue()
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
document.querySelector("#docs_btn").onclick = function () {
    window.open(`https://docs.circuitpython.org/en/latest/search.html?q=${editor.getSelectedText()}`, '_blank');
}

document.querySelector("#undo_btn").onclick = function () {
    editor.undo();
}

document.querySelector("#redo_btn").onclick = function () {
    editor.redo();
}
