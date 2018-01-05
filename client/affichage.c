#include "../libs/include.h"

extern WINDOW *mainWin, *textWin, *messWin, *messWinBox, *inputWin;

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
	drawChatWin();
	drawInputWin();
	/*drawInfoLines();
	asciiSplash();*/
	wcursyncup(inputWin);
	wrefresh(mainWin);
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

void drawChatWin() {
   // Create window for chat box, draw said box 
   messWinBox = subwin(mainWin, (LINES * 0.8), COLS, 0, 0);
   box(messWinBox, 0, 0);
   // Draw a slick title on it
   mvwaddch(messWinBox, 0, (COLS * 0.5) - 6, ACS_RTEE);
   wattron(messWinBox, COLOR_PAIR(3));
   mvwaddstr(messWinBox, 0, (COLS * 0.5) - 5, " Chat 2i " );
   wattroff(messWinBox, COLOR_PAIR(3));
   mvwaddch(messWinBox, 0, (COLS * 0.5) + 4, ACS_LTEE );
   wrefresh(messWinBox);
   // Create sub window in box to hold text
   messWin = subwin(messWinBox, (LINES * 0.8 - 2), COLS - 2, 1, 1);
   // Enable text scrolling
   scrollok(messWin, TRUE);
}

/* Draw input box and window */
void drawInputWin() {
   // Create input box and window
   textWin = subwin(mainWin, (LINES * 0.2) - 1, COLS, (LINES * 0.8) + 1, 0);
   box(textWin, 0, 0);
   inputWin = subwin(textWin, (LINES * 0.2) - 3, COLS - 2, (LINES * 0.8) + 2, 1);
}

void afficherLigne(WINDOW *win, char* texte){
	wprintw(win, "%s",texte);
	wrefresh(win);
	wcursyncup(inputWin);
	wrefresh(mainWin);
}

void recupererMessage(char *message) {
    int c;
    //char message[BUFF_MAX];
    while(1)
    {
        c = wgetch(mainWin);
        if(c==10) {
            strcat(message, "\n");
            wclear(inputWin);
            wrefresh(inputWin);
            return;
        }
        if(c==263) { //backspace
            if(strlen(message)>0) {
                char temp[BUFF_MAX] = "";
                strncpy(temp, message, strlen(message) - 1);
                strcpy(message, temp);
            }
        } else if(c>=32 && c<=255) {
            char ch[1];
            sprintf(ch, "%c", c);
            strcat(message, ch);
        }

        wclear(inputWin);
        mvwprintw(inputWin, 0, 0, message);
        wrefresh(inputWin);
    }
	//return NULL;
}

void fermerCurses(){
	endwin();
}