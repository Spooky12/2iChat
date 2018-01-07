#include "../libs/include.h"

extern WINDOW *mainWin, *textWin, *messWin, *messWinBox, *inputWin;

/**
 * @name initialiserCurses
 * @brief Fonction initialisant les différents parametres de  curses
 */
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

//********TODO: affichage d'un message d'info au lancement ********//
	drawChatWin();
	drawInputWin();
	//splashScreen();
    //Mise a jour de la fenetre
	wcursyncup(inputWin);
	wrefresh(mainWin);
}

/**
 * @name initialiserCouleurs
 * @brief Fonction initialisant les 10 paires de couleurs
 */
void initialiserCouleurs(){
	//Lancement des couleurs
	start_color();
	//Utilisation des coleurs par défaut
	use_default_colors();

	//Initialisation des paires de couleur
	init_pair(0, -1, -1);
	init_pair(1, -1, -1);
	init_pair(2, COLOR_CYAN, -1);
	init_pair(3, COLOR_YELLOW, -1);
	init_pair(4, COLOR_RED, -1);
	init_pair(5, COLOR_BLUE, -1);
	init_pair(6, COLOR_MAGENTA, -1);
	init_pair(7, COLOR_GREEN, -1);
	init_pair(8, COLOR_WHITE, COLOR_RED);
	init_pair(9, COLOR_WHITE, COLOR_GREEN);
}

/**
 * @name drawChatWin
 * @brief Fonction créant la fenetre utilisée pour afficher les messages
 */
void drawChatWin() {
   // On crée la bordure de la fenetre d'affichage
   messWinBox = subwin(mainWin, (LINES * 0.8), COLS, 0, 0);
   box(messWinBox, 0, 0);
   // On affiche le titre de la fenetre
   mvwaddch(messWinBox, 0, (COLS * 0.5) - 6, ACS_RTEE);
   wattron(messWinBox, COLOR_PAIR(3));
   mvwaddstr(messWinBox, 0, (COLS * 0.5) - 5, " Chat 2i " );
   wattroff(messWinBox, COLOR_PAIR(3));
   mvwaddch(messWinBox, 0, (COLS * 0.5) + 4, ACS_LTEE );
   wrefresh(messWinBox);
   // On crée la fenetre d'affichage des messages
   messWin = subwin(messWinBox, (LINES * 0.8 - 2), COLS - 2, 1, 1);
   // On active le scroll dans cette fenetre
   scrollok(messWin, TRUE);
}

/**
 * @name drawInputWin
 * @brief Fonction créant la fenetre utilisée pour taper les messages
 */
void drawInputWin() {
   // Création de la fenetre d'entrée et de sa bordure
   textWin = subwin(mainWin, (LINES * 0.2) - 1, COLS, (LINES * 0.8) + 1, 0);
   box(textWin, 0, 0);
   inputWin = subwin(textWin, (LINES * 0.2) - 3, COLS - 2, (LINES * 0.8) + 2, 1);
}

/**
 * @name afficherMessage
 * @brief Fonction permettant d'afficher un message
 * @param win
 * @param pseudo
 * @param texte
 * @param couleur
 */
void afficherMessage(WINDOW *win, char* pseudo, char* texte, int couleur) {
    wattron(win, COLOR_PAIR(couleur));
    wprintw(win, "%s", pseudo);
    wprintw(win, " : ");
    wprintw(win, "%s", texte);
    wprintw(win, "\n");
    wattroff(win, COLOR_PAIR(couleur));
    //Mise a jour de la fenetre
    wrefresh(win);
    wcursyncup(inputWin);
    wrefresh(mainWin);
}

/**
 * @name afficherLigne
 * @brief Fonction permettant d'afficher une ligne de texte
 * @param win
 * @param texte
 */
void afficherLigne(WINDOW *win, char* texte){
	wprintw(win, "%s",texte);
    //Mise a jour de la fenetre
	wrefresh(win);
	wcursyncup(inputWin);
	wrefresh(mainWin);
}

/**
 * @name recupererMessage
 * @brief Fonction permettant de récupérer les frappes clavier
 * @param message
 */
void recupererMessage(char *message) {
    int c;
    while(1)
    {
        c = wgetch(mainWin);
        if(c==10) { //retour chariot
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
        } else if(c>=32 && c<=255) { //charactere ascii etendu
            char ch[1];
            sprintf(ch, "%c", c);
            strcat(message, ch);
        }
        wclear(inputWin);
        mvwprintw(inputWin, 0, 0, message);
        //Mise a jour de la fenetre
        wrefresh(inputWin);
    }
}

/**
 * @name fermerCurses
 * @brief Fonction permettant de fermer correctement ncurses
 */
void fermerCurses(){
	endwin();
}