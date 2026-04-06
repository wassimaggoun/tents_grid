// demo.js

Module.onRuntimeInitialized = () => { start(); }

const EMPTY = 0;    // empty square 
const TREE = 1;     // tree square 
const TENT = 2;     // tent square 
const GRASS = 3;    // grass square 
const BIGTREE = 4;  // big tree square 

let _square2str = [" ", "x", "*", "-", "#"];

function square2str(s) {
    return _square2str[s];
}

function printGame(g) {
    var text = "";
    var nb_rows = Module._nb_rows(g);
    var nb_cols = Module._nb_cols(g);
    for (var row = 0; row < nb_rows; row++) {
        for (var col = 0; col < nb_cols; col++) {
            var s = Module._get_square(g, row, col);
            text += square2str(s);
        }
        text += "\n";
    }

    // put this text in <div> element with ID 'result'
    var elm = document.getElementById('result');
    elm.innerHTML = text;
}

function start() {
    console.log("call start routine");
    var g = Module._new_default();
    Module._play_move(g, 1, 0, TENT);
    printGame(g);
    Module._delete(g);
}

