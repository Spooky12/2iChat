/**
 * @file client/affichage.c
 * @brief Gestion de l'affichage.
 * @author Corentin A. & Thomas T.
 * @date 14 janvier 2018
 *
 * Ce fichier contient toutes les fonction servant à gérer l'affichage du client
 *
 */
#include <curses.h>
#include "../libs/include.h"

extern WINDOW *mainWin, *textWin, *messWin, *messWinBox, *inputWin;
extern int quitter;

char titreFenetre[BUFF_MAX] = "CHAT 2i";

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

	drawChatWin();
	drawInputWin();
    //Mise a jour de la fenetre
	wcursyncup(inputWin);
	wrefresh(mainWin);
}

/**
 * @name reloadTerminal
 * @brief Fonction permettant de recharger la fenetre
 */
 void reloadTerminal(){
	endwin();
    refresh();
    clear();
    
	drawChatWin();
	drawInputWin();
	
	wrefresh(messWin);
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
    char titre[BUFF_MAX] = " ";
    strcat(titre, titreFenetre);
    strcat(titre, " ");
   // On crée la bordure de la fenetre d'affichage
   messWinBox = subwin(mainWin, (LINES * 0.8), COLS, 0, 0);
   box(messWinBox, 0, 0);
    int impaire = strlen(titre) % 2;
   // On affiche le titre de la fenetre
   mvwaddch(messWinBox, 0, (COLS * 0.5) - strlen(titre)/2 -1 - impaire, ACS_RTEE);
   wattron(messWinBox, COLOR_PAIR(3));
   mvwaddstr(messWinBox, 0, (COLS * 0.5) - strlen(titre)/2 - impaire, titre );
   wattroff(messWinBox, COLOR_PAIR(3));
   mvwaddch(messWinBox, 0, (COLS * 0.5) + strlen(titre)/2, ACS_LTEE );
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
 * @name splashScreen
 * @brief Fonction affichant un message lors de l'initialisation du client
 */
void splashScreen() {
   	char str[BUFF_MAX] = "Bienvenue dans 2iChat\n";
	strcat(str, "Vous êtes actuellement dans l'accueil\n");
	strcat(str, "Faites un \\help pour voir les commandes disponibles");
	afficherMessageServeur(messWin, str);
}

void changerTitre(char *nouveauTitre) {
    endwin();
    refresh();
    clear();
    strcpy(titreFenetre, nouveauTitre);
    reloadTerminal();
    wrefresh(mainWin);
}

void sortieSalon(WINDOW *win) {
    changerTitre("Accueil");
    afficherMessageServeur(win, "Vous avez bien quitté le salon");
}

void entreeSalon(WINDOW *win, char *nomSalon, int creationSalon) {
    changerTitre(nomSalon);
    char message[BUFF_MAX] = "Le salon ";
    strcat(message, nomSalon);
    if(creationSalon) {
        strcat(message, " a bien été crée !");
    } else {
        strcat(message, " a bien été rejoint !");
    }
    afficherMessageServeur(win, message);
}

/**
 * @name afficherMessageServeur
 * @brief Fonction permettant d'afficher les messages serveur
 * @param win
 * @param text
 */
void afficherMessageServeur(WINDOW *win, char *text) {
    afficherMessage(win, "SERVEUR", text, 9);
}

/**
 * @name afficherErreur
 * @brief Fonction permettant d'afficher les erreurs
 * @param win
 * @param erreur
 * @param text
 */
void afficherErreur(WINDOW *win, char* erreur ,char *text) {
    afficherMessage(win, erreur, text, 8);
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
    while(!quitter)
    {
        c = wgetch(mainWin);
        if(c==10) { //retour chariot
            //Test pour ne pas retourner des chaines vide ou rempli seulement d'espace
            if(strcmp(message, "")) {
                int i=0;
                while(message[i]==' ') {
                    i++;
                };
                if(message[i]!='\0') {
                    strcat(message, "\n");
                    wclear(inputWin);
                    wrefresh(inputWin);
                    return;
                }
            }
        }else if(c == KEY_RESIZE){
			continue;
		}
        if(c==263) { //backspace
            if(strlen(message)>0) {
                char temp[BUFF_MAX] = "";
                strncpy(temp, message, strlen(message) - 1);
                strcpy(message, temp);
            }
        } else if(c>=32 && c<=255) { //charactere ascii etendu
            if(strlen(message)<500) { //Pas de message de plus de 500 caracteres
                char ch[1];
                sprintf(ch, "%c", c);
                strcat(message, ch);
            }
        }
        wclear(inputWin);
        mvwprintw(inputWin, 0, 0, message);
        //Mise a jour de la fenetre
        wrefresh(inputWin);
    }
}

/**
 * @name help
 * @brief Affiche un message listant les commandes possibles
 */
void help(){
	char str[BUFF_MAX] = "Bienvenue dans 2iChat\n";
	strcat(str, "Lors de la connection, vous accédez directement a l'accueil\n");
	strcat(str, "Commandes disponibles:\n");
	strcat(str, "\\ping renvoie un pong pour tester la connectivité\n");
	strcat(str, "\\list affiche la liste des serveurs sur l'accueil ou la liste des utilisateurs sur le salon\n");
	strcat(str, "\\pseudo NEW_PSEUDO vous permet de changer de pseudo\n");
	strcat(str, "\\couleur ID_COULEUR vous permet de choisir la couleur de vos messages\n");
	strcat(str, "\\me MESSAGE permet d'afficher un message sous la forme \"pseudo message\" sans le \":\"\n");
	strcat(str, "\\rand 0 10 renvoie un nombre aléatoire entre 0 et 10\n");
	strcat(str, "\\create NOM_SALON permet de créer un nouveau salon\n");
	strcat(str, "\\connect NOM_SALON permet de rejoindre un salon existant\n");
	strcat(str, "\\leave permet de quitter un salon pour rejoindre l'accueil\n");
	strcat(str, "\\prive USER permet de lancer une conversation privée avec un utilisateur\n");
	strcat(str, "\\exit permet de quitter le programme\n");
	afficherMessageServeur(messWin, str);
}

/**
 * @name fermerCurses
 * @brief Fonction permettant de fermer correctement ncurses
 */
void fermerCurses(){
	endwin();
}