/**
 * @file client.c
 * @brief Programme client
 * @author Corentin A. & Thomas T.
 * @date 14 janvier 2018
 *
 * Programme principal du client
 *
 */
#include <curses.h>
#include "../libs/include.h"

WINDOW *mainWin, *textWin, *messWin, *messWinBox, *inputWin;
int quitter, socPm;
char pseudoPm[BUFF_MAX];

/**
 * @name deroute
 * @brief Fonction permettant de gérer les signaux dérouté et de faire le traitement associé
 * @param signal_number
 */
void deroute (int signal_number)
{
    switch (signal_number) {
        case SIGINT : quitter =1;
            break;
        case SIGWINCH : reloadTerminal();
            break;
    }
}

/**
 * @name gestion_envoie
 * @brief Boucle permettant de s'occuper du traitement et de l'envoie au serveur des inputs du client
 * @param soc
 */
void* gestion_envoie(void *soc) {
    int sock = *(int*) soc;
    char req[BUFF_MAX];
	char message[BUFF_MAX]="";

    recupererMessage(message);
    while(strcmp(message,"\\exit\n")!=0 && !quitter){
        if(socPm != -1){
            if(strcmp(message, "\\leave\n")==0){
                afficherMessageServeur(messWin, "La discussion privée est terminée");
                sprintf(req,"%i\n",1);
                envoyer_requete(socPm, req);
                socPm=-1;
                strcpy(pseudoPm, "");
            }else {
                envoyer_messagePrive(req, message);
            }
        } else {
            if(strcmp(message, "\\help\n")==0){
                help();
            }else if(strcmp(message,"\\accept\n")==0 && strcmp(pseudoPm, "") != 0) {
                startPrive(sock, req);
            } else if(message[0] == '\\') {
                if(strcmp(pseudoPm, "") != 0) { //refus de la discussion privé
                    sprintf(req,"%i\n%s\n",302, pseudoPm);
                    envoyer_requete(sock, req);
                    strcpy(pseudoPm, "");
                }
                envoyer_commande(sock, req, message);
            } else {
                if(strcmp(pseudoPm, "") != 0) { //refus de la discussion privé
                    sprintf(req,"%i\n%s\n",302, pseudoPm);
                    envoyer_requete(sock, req);
                    strcpy(pseudoPm, "");
                }
                envoyer_message(sock, req, message);
            }
        }
        strcpy(message, "");
        recupererMessage(message);
    }

    //fermeture des mp si il y a une discussion en cours
    if(socPm!=-1) {
        sprintf(req,"%i\n",1);
        envoyer_requete(socPm, req);
    }
	sprintf(req,"%i\n",0);
    afficherLigne(messWin, "Envoie 0\n");
	envoyer_requete(sock, req);
    afficherLigne(messWin, "exit envoie\n");
    pthread_exit(0);
}


/**
 * @name gestion_lecture
 * @brief Boucle permettant de s'occuper de la réception et du traitement des requêtes du serveur
 * @param soc
 */
void* gestion_lecture(void *soc) {
    int sock = *(int*) soc;
    while(!quitter) {
        lire_reponse(sock);
    }
    pthread_exit(0);
}

/**
 * @name gestion_lecturePm
 * @brief Boucle permettant de s'occuper de la réception et du traitement des requêtes du lors d'une discussion privée
 */
void* gestion_lecturePm() {
    afficherMessageServeur(messWin, "La discussion privé a commencé (\\leave pour quitter)");
    while(socPm!=-1 && !quitter) {
        char rep[BUFF_MAX];
        int repID=-1;
        //On vide le buffer
        strcpy(rep, "");

        //On lit la réponse
        CHECK(read(socPm, rep, BUFF_MAX), "ERREUR READ")

        //TEST affiche les requêtes entrente
        /*char temp[BUFF_MAX];
        sprintf(temp ,"Requete recu : '%s'\n", rep);
        afficherLigne(messWin, temp);*/

        char params[BUFF_MAX]="";
        char repIDc[BUFF_MAX];

        //recuperation du code de la requete
        stpcpy(params, rep);
        split(repIDc, params);
        if(strcmp(repIDc,"")==0) {
            afficherLigne(messWin, "Requête vide : exit thread\n");
            pthread_exit(0);
        }
        repID = atoi(repIDc);

        //switch pour faire le traitement associe au code de la requete
        switch(repID){
            case 1 : //Quitter
                afficherMessageServeur(messWin, "La discussion privée est terminée");
                socPm=-1;
                strcpy(pseudoPm, "");
                pthread_exit(0);
                break;
            case 212: //Message
            {
                char texte[BUFF_MAX];
                split(texte, params);
                //couleur au pif en attendant de mettre une couleur spéciale PM
                afficherMessage(messWin, pseudoPm, texte, 2);
                break;
            }
            default: //Code requete inconnu
                afficherErreur(messWin, "REQUETTE INCONNUE", repIDc);
                break;
        }

    }
    pthread_exit(0);
}

/**
 * @name split
 * @brief Fonction qui permet de déplacer params dans dest jusqu'au premier \n
 * @param dest
 * @param params
 */
void split(char *dest, char *params) {
    strcpy(dest, "");
    if(params==NULL) {
        return;
    }
    char *d = strstr(params, "\n");
    if(d==NULL) {
        strcpy(dest, params);
        strcpy(params, "");
        return;
    }
    int pos = d - params;

    strncpy(dest, params, pos);
    dest[pos]='\0';
    strcpy(params, d+1);
    if(strcmp(dest, "") == 0) {
        split(dest, params);
    }
}

/**
 * @name lire_reponse
 * @brief Fonction permettant de s'occuper de la réception et du traitement des requêtes du serveur
 * @param soc
 */
void lire_reponse(int soc){
    char rep[BUFF_MAX];
    int repID=-1;
    //On vide le buffer
    strcpy(rep, "");

    //On lit la réponse
    CHECK(read(soc, rep, BUFF_MAX), "ERREUR READ")

    //TEST affiche les requêtes entrente
	/*char temp[BUFF_MAX];
	sprintf(temp ,"Requete recu : '%s'\n", rep);
	afficherLigne(messWin, temp);*/

    char params[BUFF_MAX]="";
    char repIDc[BUFF_MAX];

    //recuperation du code de la requete
    stpcpy(params, rep);
    split(repIDc, params);
    if(strcmp(repIDc,"")==0) {
        afficherLigne(messWin, "Requête vide : exit thread\n");
        pthread_exit(0);
    }
    repID = atoi(repIDc);

    //switch pour faire le traitement associe au code de la requete
    switch(repID){
        case 0 : //Quitter
            afficherLigne(messWin, "Réponse 0\n");
            pthread_exit(0);
            break;
        case 200: //Message
        {
            char pseudo[BUFF_MAX], cCouleur[BUFF_MAX], texte[BUFF_MAX];
            split(pseudo, params);
            split(cCouleur, params);
            int couleur = atoi(cCouleur);
            split(texte, params);
            afficherMessage(messWin, pseudo, texte, couleur);
            break;
        }
		case 201: // message serveur
        {
            char message[BUFF_MAX]="";
            char temp[BUFF_MAX]="";
            split(temp, params);
            strcat(message, temp);
            while(strcmp(params, "") !=0){
                split(temp, params);
                strcat(message, "\n");
                strcat(message, temp);
            }
            afficherMessageServeur(messWin, message);
            break;
        }
        case 202: //join salon (nom salon)
        {
            char nomSalon[BUFF_MAX];
            split(nomSalon, params);
            entreeSalon(nomSalon, 0);
            break;
        }
        case 203: //creation salon (nom salon)
        {
            char nomSalon2[BUFF_MAX];
            split(nomSalon2, params);
            entreeSalon(nomSalon2, 1);
            break;
        }
        case 204: //creation salon (nom salon)
        {
            sortieSalon(messWin);
            break;
        }
		case 211: // \me
            afficherLigne(messWin, params);
            break;
        case 300: //demande de mp
            demandePrive(params);
            break;
        case 301: //demande de mp accepté
            afficherMessageServeur(messWin, "La demande de conversation privée a été acceptée (\\leave pour quitter)");
            connecterPrive(params);
            break;
        case 302: //demande de mp refusé
            afficherMessageServeur(messWin, "La demande de conversation privée a été refusée");
            break;
        case 400: //Erreur
        {
            char messErreur[BUFF_MAX];
            split(messErreur, params);
            afficherErreur(messWin, "ERREUR 400", messErreur);
            break;
        }
        case 401: //Erreur commande
            afficherErreur(messWin, "ERREUR 401", "Commande inconnue");
            break;
        default: //Code requete inconnu
            afficherErreur(messWin, "REQUETTE INCONNUE", repIDc);
            break;
    }

}

/**
 * @name envoyer_message
 * @brief Fonction permettant l'envoie de message au serveur
 * @param soc
 * @param req
 * @param message
 */
void envoyer_message(int soc, char *req, char *message) {
    sprintf(req,"%i\n%s\n",100,message);
    //TEST affiche la requete sortante
    /*sprintf(message, "Envoie du message: %s", req);
    afficherLigne(messWin, message);*/
    envoyer_requete(soc, req);
}

/**
 * @name envoyer_messagePrive
 * @brief Fonction permettant l'envoie de message lors d'une discussion privée
 * @param req
 * @param message
 */
void envoyer_messagePrive(char *req, char *message) {
    sprintf(req,"%i\n%s\n",212,message);
    envoyer_requete(socPm, req);
    char mess[BUFF_MAX];
    split(mess, message);
    afficherMessage(messWin, "Moi", mess, 2);
}

/**
 * @name envoyer_commande
 * @brief Fonction permettant l'envoie de commande
 * @param soc
 * @param req
 * @param commande
 */
void envoyer_commande(int soc, char *req, char *commande) {
    for(int i=0; i<strlen(commande); i++){
        if(commande[i+1] != ' ' ) {
            commande[i] = commande[i+1];
        } else {
            commande[i] = '\n';
        }
    }
    sprintf(req,"%i\n%s\n",101,commande);
    //TEST affiche la requete sortante
    /*sprintf(commande, "Envoie de la commande: %s", req);
    afficherLigne(messWin, commande);*/
    envoyer_requete(soc, req);
}

/**
 * @name envoyer_requete
 * @brief Fonction permettant l'envoie de requête
 * @param soc
 * @param req
 */
void envoyer_requete(int soc, char *req){
	//On envoie la requete au serveur
    CHECK(write(soc, req, strlen(req)+1), "ERREUR WRITE");
}

/**
 * @name demandePrive
 * @brief Fonction permettant d'afficher la requête de demande de discussion privée
 * @param params
 */
void demandePrive(char *params) {
    split(pseudoPm, params);
    char message[BUFF_MAX] = "L'utilisateur ";
    strcat(message, pseudoPm);
    strcat(message, " veut entamer une discussion privée avec vous. Envoyer \\accept pour accepter la discussion");
    afficherMessageServeur(messWin, message);
}

/**
 * @name startPrive
 * @brief Fonction permettant de démarrer une discussion privée entre deux utilisateurs en créant un socket spécifique entre les deux utilisateurs
 * @param soc
 * @param req
 */
void startPrive(int soc, char *req) {
    sprintf(req,"%i\n%s\n",301, pseudoPm);
    envoyer_requete(soc, req);
    int socServeur;
    //Création des adresses
    struct sockaddr_in addr_serveur;
    struct sockaddr_in addr_client;
    int tailleClient;
    unsigned int serveur_taille;

    socServeur = socket(AF_INET,SOCK_STREAM,0);

    addr_serveur.sin_family = AF_INET;
    addr_serveur.sin_port = htons(PORT_PM);
    addr_serveur.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(addr_serveur.sin_zero,0,8);

    //On bind le socket et on le met en mode écoute
    CHECK(bind(socServeur, (struct sockaddr*)&addr_serveur, sizeof(addr_serveur)), "ERREUR BIND");
    CHECK(listen(socServeur,1),"ERREUR LISTEN")

    //On récupère la taille de l'adresse du client
    tailleClient = sizeof(struct sockaddr);

    //On attend qu'un client se connecte
    CHECK((socPm = accept(socServeur, (struct sockaddr*)&addr_client, (unsigned int*)&tailleClient)), "ERREUR ACCEPT")
    pthread_t thread_pm;
    pthread_create(&thread_pm, NULL, gestion_lecturePm, NULL);

    pthread_detach(thread_pm);
}

/**
 * @name connecterPrive
 * @brief Fonction permettant de se connecter au socket de l'utilisateur pour la discussion privée
 * @param params
 */
void connecterPrive(char *params) {
    split(pseudoPm, params);
    char ip[BUFF_MAX];
    split(ip, params);
    socPm =  socket(AF_INET,SOCK_STREAM,0);
    //Création de l'adresse du serveur
    struct sockaddr_in addr_serveur;

    addr_serveur.sin_family = AF_INET;
    addr_serveur.sin_port = htons(PORT_PM);
    inet_aton(ip,&(addr_serveur.sin_addr));
    memset(addr_serveur.sin_zero,0,8);

    //Connection au serveur
    CHECK(connect(socPm,(struct sockaddr*)&addr_serveur, sizeof(addr_serveur)), "ERREUR CONNECT")
    pthread_t thread_pm;

    CHECK(pthread_create(&thread_pm, NULL, gestion_lecturePm, NULL ), "ERREUR création thread envoie")
    pthread_detach(thread_pm);
}

/**
 * @name main
 * @brief Fonction principale du client
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]){
    char addresseIP[BUFF_MAX];
    if(argc > 2) {
        printf("Erreur : trop d'arguments !\nL'argument attendu est l'adresse IP du serveur\nSans argument l'adresse par défaut est 127.0.0.1\n");
        return 1;
    } else if (argc ==2) {
        strcpy(addresseIP, argv[1]);
        //check si l'adresse IP est valide
        struct sockaddr_in sa;
        if(inet_aton(addresseIP,&(sa.sin_addr)) == 0) {
            printf("Erreur l'argument %s n'est pas une adresse IP valide\n", addresseIP);
            return 1;
        }
    } else { //pas d'argument donc IP par défaut
        strcpy(addresseIP, "127.0.0.1");
    }
    quitter=0;
    socPm=-1;
    strcpy(pseudoPm, "");
    //Handler pour dérouter les signaux
    struct sigaction newact;
    int sts;
    void deroute();
    newact.sa_handler = deroute;
    CHECK(sigemptyset(&newact.sa_mask),"problème sigemptyset");
    newact.sa_flags = 0;
    CHECK(sigaction(SIGINT, &newact, NULL),"problème sigaction SIGINT");
    CHECK(sigaction(SIGWINCH, &newact, NULL),"problème sigaction SIGWINCH");

	//Lancement de l'interface graphique
	initialiserCurses();
	
    //Création du socket client
    int socClient;

    socClient = socket(AF_INET,SOCK_STREAM,0);

    //Création de l'adresse du serveur
    struct sockaddr_in addr_serveur;

    addr_serveur.sin_family = AF_INET;
    addr_serveur.sin_port = htons(PORT);
    inet_aton(addresseIP,&(addr_serveur.sin_addr));
    memset(addr_serveur.sin_zero,0,8);

    //Connection au serveur
    CHECK(connect(socClient,(struct sockaddr*)&addr_serveur, sizeof(addr_serveur)), "ERREUR CONNECT")
    changerTitre("Accueil");
	splashScreen();
    pthread_t threads[2];

    CHECK(pthread_create(&threads[0], NULL, gestion_envoie, (void *) &socClient ), "ERREUR création thread envoie")
    CHECK(pthread_create(&threads[1], NULL, gestion_lecture, (void *) &socClient), "ERREUR création thread lecture")

    //On attend la fin des thread
    pthread_join(threads[0], NULL);
    afficherLigne(messWin, "Join 0\n");
    pthread_join(threads[1], NULL);
    afficherLigne(messWin, "Join 1\n");
    //On ferme la socket
    close(socClient);
    afficherLigne(messWin, "Close socket 0\n");
	
	//fermeture de l'interface graphique
	fermerCurses();
    return 0;
}
