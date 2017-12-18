#include "../include.h"

extern WINDOW *mainWin;

void initialiserCurses() {
	//Lancement de la fenetre principale
	if ((mainWin = initscr()) == NULL) { exit(1); }
	//On désactive l'affichage des frappes
	noecho();
	//On lit les input cara par cara
	cbreak();
	//On capture les cara spéciaux
	keypad(mainWin, TRUE);
	//Lancement des couleurs
	initialiserCouleurs();

//********TODO********//
	/*drawChatWin();
	drawInputWin();
	drawInfoLines();
	asciiSplash();*/
}

void initialiserCouleurs(){
	//Lancement des couleurs
	start_color();
	//Utilisation des coleurs par défaut
	use_default_colors();

	//Initialisation des paires de couleur
	init_pair(1, -1, -1);                           
	init_pair(2, COLOR_CYAN, -1);
	init_pair(3, COLOR_YELLOW, -1);
	init_pair(4, COLOR_RED, -1);
	init_pair(5, COLOR_BLUE, -1);
	init_pair(6, COLOR_MAGENTA, -1);
	init_pair(7, COLOR_GREEN, -1);
	init_pair(8, COLOR_WHITE, COLOR_RED);
}

void fermerCurses(){
	endwin();
}