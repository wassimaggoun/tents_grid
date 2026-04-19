var g;//Déclaration de variable globale g et canvas
var canvas = document.getElementById('mycanvas');


//Types de cases de la grille(les constantes)
const EMPTY   = 0;
const TREE    = 1;
const TENT    = 2;
const GRASS   = 3;
const BIGTREE = 4;


 
const IMGS = {};//sert a stocker les images chargees
//contient les chemins des images dans le dossier res
const IMG_SRCS = {
    background: 'res/background.png',
    tree:       'res/tree.png',
    bigtree:    'res/bigtree.png',
    tent:       'res/tent.png',
    grass:      'res/grass.png',
};
 
//Charge tous les images, puis appelle callback quand le chargement terminé
function loadImages(callback) {
    let remaining = Object.keys(IMG_SRCS).length;
    for (const [key, src] of Object.entries(IMG_SRCS)) {//parcours de toutes les images a charger
        const img = new Image();
        img.onload  = () => { if (--remaining === 0) callback(); };//si l'image etati dernier, on appelle callback
        img.onerror = () => { console.warn('Image introuvable:', src); if (--remaining === 0) callback(); };
        img.src = src;
        IMGS[key] = img;//stocke l'image chargee dans IMGS
    }
}
 

 //Attendre pour wasm, puis charger les images et démarrer le jeu
Module.onRuntimeInitialized = () => {
    loadImages(() => { start(); });
};
 
function start() {
    console.log("Jeu démarré");
 
    resizeCanvas();//ajuste la taille du canvas
    g = Module._new_default();//création du jeu (via wasm)
 
    //créer une nouvelle grille aléatoire a partir des paramteres saisis
    document.getElementById('btnRandom').onclick = function () {
        const rows     = parseInt(document.getElementById('inputRows').value)  || 8;//on recupere le nb de lignes depuis l'input , ||8 (si c'est empty ou invalid)
        const cols     = parseInt(document.getElementById('inputCols').value)  || 8;
        const wrapping = document.getElementById('checkWrapping').checked;
 
        const total      = Math.max(1, Math.floor(rows * cols * 0.1875));//dans notre grille, il y aura minimum 1 arbre, on va determiner le nombre d'arbres en fonction de la taille de la grille(18,75%)
        const nbBigTrees = Math.floor(total * 0.15);//puis calcule la part de grands arbres(15%)
        const nbTrees    = total - nbBigTrees;
 
        if (g) Module._delete(g);
        g = Module._new_random(rows, cols, wrapping, nbTrees, nbBigTrees, false);//création d'un jeu aléatoire avec les parametres
        resizeCanvas();
        drawGame();
    };
 
    //associe les boutons aux actions du jeu
    document.getElementById('restart').onclick   = () => { Module._restart(g);    wonState = false; drawGame(); };
    document.getElementById('undo').onclick      = () => { Module._undo(g);       drawGame(); };
    document.getElementById('redo').onclick      = () => { Module._redo(g);       drawGame(); };
    document.getElementById('solve').onclick     = () => { Module._solve(g);      wonState = true;  drawGame(); };
    document.getElementById('fillgrass').onclick = () => { Module._fill_grass(g); drawGame(); };
 
    //gère les clics gauche et droit sur le canvas(empeche le menu pour le clic droit)
    canvas.addEventListener('click',        canvasClick);
    canvas.addEventListener('contextmenu', (e) => { e.preventDefault(); canvasClick(e); });
 
    window.addEventListener('resize', () => { resizeCanvas(); drawGame(); });
 
    drawGame();
}
 
//état de victoire  
var wonState = false;

//adapte la taille du canvas a la fenetre avec des limits
function resizeCanvas() {
    const size = Math.max(300, Math.min(
        window.innerWidth  - 32,
        window.innerHeight - 220,
        700
    ));
    canvas.width  = size;
    canvas.height = size;
}
 

 
function drawGame() {
    if (!g) return;
 
    const ctx  = canvas.getContext('2d');//récupere le contexte 2D pour dessiner sur le canvas
    const W    = canvas.width;
    const H    = canvas.height;
    const rows = Module._nb_rows(g);
    const cols = Module._nb_cols(g);
 
   
    const cellW = W / (cols + 1);//calcule la taille du chaque case(avec espace pour les indices)
    const cellH = H / (rows + 1);
    const gridW = cols * cellW;
    const gridH = rows * cellH;
    const ox = 0, oy = 0;//Position départ de la grille    

    ctx.clearRect(0, 0, W, H);//efface le canvas avant de redessiner
    //affiche l'image si elle est chargee, sinon on remplit avec une couleur
    if (IMGS.background && IMGS.background.complete && IMGS.background.naturalWidth) {
        ctx.drawImage(IMGS.background, 0, 0, W, H);
    } else {
        ctx.fillStyle = '#6b8f4e';
        ctx.fillRect(0, 0, W, H);
    }
 
   //parcours toutes les cases de la grille et dessine l'image correspondante
    for (let i = 0; i < rows; i++) {
        for (let j = 0; j < cols; j++) {
            const sq = Module._get_square(g, i, j);
            const x  = ox + j * cellW;//position de la case dans le canvas 
            const y  = oy + i * cellH;
 
            let imgKey = null;
            if      (sq === TREE)    imgKey = 'tree';
            else if (sq === BIGTREE) imgKey = 'bigtree';
            else if (sq === TENT)    imgKey = 'tent';
            else if (sq === GRASS)   imgKey = 'grass';
 
            //dessine l'image si elle existe et chargée
            if (imgKey && IMGS[imgKey] && IMGS[imgKey].complete) {
                ctx.drawImage(IMGS[imgKey], x, y, cellW, cellH);
            }
        }
    }
 
    //couleur et épaisseur des lignes
    ctx.strokeStyle = 'rgb(255, 0, 0)';
    ctx.lineWidth   = 1;
 
    //Dessine la grille en tracant les lignes verticales et horizontales
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
 
    //configure le style du texte(taille et center)
    const fontSize = Math.max(10, Math.min(cellW * 0.55, cellH * 0.55, 32));
    ctx.font         = 'bold ' + fontSize + 'px Arial';
    ctx.textAlign    = 'center';
    ctx.textBaseline = 'middle';
 
    //affiche les expected_nb_tents pour chaque ligne et colonne
    for (let i = 0; i < rows; i++) {
        const exp = Module._get_expected_nb_tents_row(g, i);
        const cur = Module._get_current_nb_tents_row(g, i);
        const ok  = (cur === exp);
        const x   = ox + gridW + cellW / 2;
        const y   = oy + i * cellH + cellH / 2;
        ctx.fillStyle = ok ? 'white' : 'rgb(255, 50, 50)';//rouge si la condition n'est pas respectée
        ctx.fillText(String(exp), x, y);
    }
 
    for (let j = 0; j < cols; j++) {
        const exp = Module._get_expected_nb_tents_col(g, j);
        const cur = Module._get_current_nb_tents_col(g, j);
        const ok  = (cur === exp);
        const x   = ox + j * cellW + cellW / 2;
        const y   = oy + gridH + cellH / 2;
        ctx.fillStyle = ok ? 'white' : 'rgb(255, 50, 50)';
        ctx.fillText(String(exp), x, y);
    }
 
    //met en évidence les tentes invalides en rouge
    ctx.save();
    ctx.globalAlpha = 120 / 255;   
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

    //affiche un message de victoire au centre si le jeu est gagné
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
 

 //gére les clics sur le canvas
function canvasClick(e) {
    if (!g || wonState) return;
 
    //convertit la position du clic en coordonnes dans le canvas
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
 
    //ignore le clic si en dehors de la grille
    if (row < 0 || row >= rows || col < 0 || col >= cols) return;
 
    const sq      = Module._get_square(g, row, col);
    const isRight = (e.type === 'contextmenu' || e.button === 2);
 
    //determine si on va mettre tent ou grass
    if (!isRight) {
       
        const newSq = (sq === TENT) ? EMPTY : TENT;
        if (Module._check_move(g, row, col, newSq) || newSq === EMPTY) {
            Module._play_move(g, row, col, newSq);
        }
    } else {
      
        const newSq = (sq === GRASS) ? EMPTY : GRASS;
        if (Module._check_move(g, row, col, newSq) || newSq === EMPTY) {
            Module._play_move(g, row, col, newSq);
        }
    }
 
    //verifie si le jeu est gagné
    if (!wonState && Module._won(g)) {
        wonState = true;
    }
 
    drawGame();
}



