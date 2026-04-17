var g; 
var canvas = document.getElementById('mycanvas');

const EMPTY = 0;
const TREE  = 1;
const TENT  = 2;
const GRASS = 3;
const BIGTREE = 4;

const marge = 40; //marge pour les excpected nb tents


Module.onRuntimeInitialized = () => {
    start();
};

function start() {
    console.log("Game started");
    g = Module._new_default(); 

    // bouton random
    document.getElementById('btnRandom').onclick = function() {
        let rows = parseInt(document.getElementById('inputRows').value);
        let cols = parseInt(document.getElementById('inputCols').value);
        let wrapping = document.getElementById('checkWrapping').checked;
        
        // les arbres occupent 18.75% de la grille (on a calculé le ration en se basant sur game_default)
        let totalTrees = Math.max(1, Math.floor((rows * cols) * 0.1875));
        let nbBigTrees = Math.floor(totalTrees * 0.15);  //15% des arbres seront des BIGTREE
        let nbTrees = totalTrees - nbBigTrees; //le reste des TREE

        if (g) Module._delete(g); 
        g = Module._new_random(rows, cols, wrapping, nbTrees, nbBigTrees, false);
        
        drawGame();
    };

    // options du jeu
    document.getElementById('restart').onclick = () => { Module._restart(g); drawGame(); };
    document.getElementById('undo').onclick    = () => { Module._undo(g); drawGame(); };
    document.getElementById('redo').onclick    = () => { Module._redo(g); drawGame(); };
    document.getElementById('solve').onclick   = () => { Module._solve(g); drawGame(); };
    document.getElementById('fillgrass').onclick = () => { Module._fill_grass(g); drawGame(); };

    // clicks
    canvas.addEventListener('click', canvasClick);
    canvas.addEventListener('contextmenu', (e) => { e.preventDefault(); canvasClick(e); });

    drawGame();
}


