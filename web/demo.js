var g;
var canvas = document.getElementById('mycanvas');
 
const EMPTY   = 0;
const TREE    = 1;
const TENT    = 2;
const GRASS   = 3;
const BIGTREE = 4;
 
/* ── Chargement des images ──────────────────────────────────── */
 
const IMGS = {};
const IMG_SRCS = {
    background: 'res/background.png',
    tree:       'res/tree.png',
    bigtree:    'res/bigtree.png',
    tent:       'res/tent.png',
    grass:      'res/grass.png',
};
 
function loadImages(callback) {
    let remaining = Object.keys(IMG_SRCS).length;
    for (const [key, src] of Object.entries(IMG_SRCS)) {
        const img = new Image();
        img.onload  = () => { if (--remaining === 0) callback(); };
        img.onerror = () => { console.warn('Image introuvable:', src); if (--remaining === 0) callback(); };
        img.src = src;
        IMGS[key] = img;
    }
}
 
/* ── Initialisation ─────────────────────────────────────────── */
 
Module.onRuntimeInitialized = () => {
    loadImages(() => { start(); });
};
 
function start() {
    console.log("Jeu démarré");
 
    resizeCanvas();
    g = Module._new_default();
 
    /* boutons */
    document.getElementById('btnRandom').onclick = function () {
        const rows     = parseInt(document.getElementById('inputRows').value)  || 8;
        const cols     = parseInt(document.getElementById('inputCols').value)  || 8;
        const wrapping = document.getElementById('checkWrapping').checked;
 
        const total      = Math.max(1, Math.floor(rows * cols * 0.1875));
        const nbBigTrees = Math.floor(total * 0.15);
        const nbTrees    = total - nbBigTrees;
 
        if (g) Module._delete(g);
        g = Module._new_random(rows, cols, wrapping, nbTrees, nbBigTrees, false);
        resizeCanvas();
        drawGame();
    };
 
    document.getElementById('restart').onclick   = () => { Module._restart(g);    wonState = false; drawGame(); };
    document.getElementById('undo').onclick      = () => { Module._undo(g);       drawGame(); };
    document.getElementById('redo').onclick      = () => { Module._redo(g);       drawGame(); };
    document.getElementById('solve').onclick     = () => { Module._solve(g);      wonState = true;  drawGame(); };
    document.getElementById('fillgrass').onclick = () => { Module._fill_grass(g); drawGame(); };
 
    canvas.addEventListener('click',        canvasClick);
    canvas.addEventListener('contextmenu', (e) => { e.preventDefault(); canvasClick(e); });
 
    window.addEventListener('resize', () => { resizeCanvas(); drawGame(); });
 
    drawGame();
}
 
/* ── État victoire (comme env->won dans model.c) ────────────── */
var wonState = false;
 
/* ── Redimensionnement ─────────────────────────────────────── */
 
function resizeCanvas() {
    const size = Math.max(300, Math.min(
        window.innerWidth  - 32,
        window.innerHeight - 220,
        700
    ));
    canvas.width  = size;
    canvas.height = size;
}
 
/* ── Rendu principal (calque model.c / render()) ────────────── */
 
function drawGame() {
    if (!g) return;
 
    const ctx  = canvas.getContext('2d');
    const W    = canvas.width;
    const H    = canvas.height;
    const rows = Module._nb_rows(g);
    const cols = Module._nb_cols(g);
 
    /* Même calcul que SDL : w/(cols+1), h/(rows+1) */
    const cellW = W / (cols + 1);
    const cellH = H / (rows + 1);
    const gridW = cols * cellW;
    const gridH = rows * cellH;
    const ox = 0, oy = 0;   /* offset_x, offset_y = 0 comme dans model.c */
 
    /* ── 1. Fond (background.png) ── */
    ctx.clearRect(0, 0, W, H);
    if (IMGS.background && IMGS.background.complete && IMGS.background.naturalWidth) {
        ctx.drawImage(IMGS.background, 0, 0, W, H);
    } else {
        ctx.fillStyle = '#6b8f4e';
        ctx.fillRect(0, 0, W, H);
    }
 
    /* ── 2. Images des cases ── */
    for (let i = 0; i < rows; i++) {
        for (let j = 0; j < cols; j++) {
            const sq = Module._get_square(g, i, j);
            const x  = ox + j * cellW;
            const y  = oy + i * cellH;
 
            let imgKey = null;
            if      (sq === TREE)    imgKey = 'tree';
            else if (sq === BIGTREE) imgKey = 'bigtree';
            else if (sq === TENT)    imgKey = 'tent';
            else if (sq === GRASS)   imgKey = 'grass';
 
            if (imgKey && IMGS[imgKey] && IMGS[imgKey].complete) {
                ctx.drawImage(IMGS[imgKey], x, y, cellW, cellH);
            }
        }
    }
 
    /* ── 3. Lignes de grille (rouges, comme model.c) ── */
    ctx.strokeStyle = 'rgb(255, 0, 0)';
    ctx.lineWidth   = 1;
 
    for (let i = 0; i <= cols; i++) {
        const px = ox + i * cellW;
        ctx.beginPath();
        ctx.moveTo(px, oy);
        ctx.lineTo(px, oy + gridH);
        ctx.stroke();
    }
    for (let j = 0; j <= rows; j++) {
        const py = oy + j * cellH;
        ctx.beginPath();
        ctx.moveTo(ox, py);
        ctx.lineTo(ox + gridW, py);
        ctx.stroke();
    }
 
    /* ── 4. Compteurs lignes (à droite, comme model.c) ── */
    const fontSize = Math.max(10, Math.min(cellW * 0.55, cellH * 0.55, 32));
    ctx.font         = 'bold ' + fontSize + 'px Arial';
    ctx.textAlign    = 'center';
    ctx.textBaseline = 'middle';
 
    for (let i = 0; i < rows; i++) {
        const exp = Module._get_expected_nb_tents_row(g, i);
        const cur = Module._get_current_nb_tents_row(g, i);
        const ok  = (cur === exp);
        const x   = ox + gridW + cellW / 2;
        const y   = oy + i * cellH + cellH / 2;
        ctx.fillStyle = ok ? 'white' : 'rgb(255, 50, 50)';
        ctx.fillText(String(exp), x, y);
    }
 
    /* ── 5. Compteurs colonnes (en bas, comme model.c) ── */
    for (let j = 0; j < cols; j++) {
        const exp = Module._get_expected_nb_tents_col(g, j);
        const cur = Module._get_current_nb_tents_col(g, j);
        const ok  = (cur === exp);
        const x   = ox + j * cellW + cellW / 2;
        const y   = oy + gridH + cellH / 2;
        ctx.fillStyle = ok ? 'white' : 'rgb(255, 50, 50)';
        ctx.fillText(String(exp), x, y);
    }
 
    /* ── 6. Overlay rouge sur tentes invalides ── */
    ctx.save();
    ctx.globalAlpha = 120 / 255;   /* SDL_ALPHA 120 */
    ctx.fillStyle   = 'rgb(255, 0, 0)';
    for (let i = 0; i < rows; i++) {
        for (let j = 0; j < cols; j++) {
            if (Module._get_square(g, i, j) === TENT &&
                !Module._check_move(g, i, j, TENT)) {
                ctx.fillRect(ox + j * cellW, oy + i * cellH, cellW, cellH);
            }
        }
    }
    ctx.restore();
 
    /* ── 7. Message victoire (CONGRATS!!!) ── */
    if (wonState) {
        const msgW = Math.min(W * 0.55, 300);
        const msgH = Math.min(H * 0.1, 50);
        const mx   = W / 2 - msgW / 2;
        const my   = H / 2 - msgH / 2;
 
        ctx.font         = 'bold ' + Math.min(msgH * 0.7, 36) + 'px Arial';
        ctx.textAlign    = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillStyle    = 'rgb(0, 0, 255)';
        ctx.fillText('CONGRATS!!!', W / 2, H / 2);
    }
}
 
/* ── Gestion des clics (calque model.c / process()) ─────────── */
 
function canvasClick(e) {
    if (!g || wonState) return;
 
    const rect   = canvas.getBoundingClientRect();
    const scaleX = canvas.width  / rect.width;
    const scaleY = canvas.height / rect.height;
    const px     = (e.clientX - rect.left) * scaleX;
    const py     = (e.clientY - rect.top)  * scaleY;
 
    const rows  = Module._nb_rows(g);
    const cols  = Module._nb_cols(g);
    const cellW = canvas.width  / (cols + 1);
    const cellH = canvas.height / (rows + 1);
 
    const col = Math.floor(px / cellW);
    const row = Math.floor(py / cellH);
 
    if (row < 0 || row >= rows || col < 0 || col >= cols) return;
 
    const sq      = Module._get_square(g, row, col);
    const isRight = (e.type === 'contextmenu' || e.button === 2);
 
    if (!isRight) {
        /* clic gauche : TENT ↔ EMPTY  (comme model.c) */
        const newSq = (sq === TENT) ? EMPTY : TENT;
        if (Module._check_move(g, row, col, newSq) || newSq === EMPTY) {
            Module._play_move(g, row, col, newSq);
        }
    } else {
        /* clic droit : GRASS ↔ EMPTY  (comme model.c) */
        const newSq = (sq === GRASS) ? EMPTY : GRASS;
        if (Module._check_move(g, row, col, newSq) || newSq === EMPTY) {
            Module._play_move(g, row, col, newSq);
        }
    }
 
    if (!wonState && Module._won(g)) {
        wonState = true;
    }
 
    drawGame();
}



