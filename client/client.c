#include "../libs/include.h"

WINDOW *mainWin, *textWin, *messWin, *messWinBox, *inputWin;

/***
 * @name gestion_envoie
 * @brief boucle permettant de s'occuper de du traitement et de l'envoie au serveur des inputs du client
 * @param soc
 */
void* gestion_envoie(void *soc) {
    int sock = *(int*) soc;
    char req[BUFF_MAX];
	char message[BUFF_MAX]="";

    recupererMessage(message);
    while(strcmp(message,"\\exit\n")!=0){
        if(message[0] == '\\') {
            envoyer_commande(sock, req, message);
        } else {
            envoyer_message(sock, req, message);
        }
        strcpy(message, "");
        recupererMessage(message);
    }

	sprintf(req,"%i\n",0);
    afficherLigne(messWin, "Envoie 0\n");
	envoyer_requete(sock, req);
    afficherLigne(messWin, "exit envoie\n");
    pthread_exit(0);
}

/***
 * @name gestion_lecture
 * @brief boucle permettant de s'occuper de la reception et du traitement des requetes du serveur
 * @param soc
 */
void* gestion_lecture(void *soc) {
    int sock = *(int*) soc;
    while(1) {
        lire_reponse(sock);
    }
}

/***
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
}

/***
 * @name lire_reponse
 * @brief fonction permettant de s'occuper de la reception et du traitement des requetes du serveur
 * @param soc
 */
void lire_reponse(int soc){
    char rep[BUFF_MAX];
    int repID=-1;
    //On vide le buffer
    strcpy(rep, "");

    //On lit la réponse
    CHECK(read(soc, rep, BUFF_MAX), "ERREUR READ")

    //TEST affiche les requetes entrente
	/*char temp[BUFF_MAX];
	sprintf(temp ,"Requete recu : '%s'\n", rep);
	afficherLigne(messWin, temp);*/

    char params[BUFF_MAX]="";
    char repIDc[BUFF_MAX];

    //recuperation du code de la requete
    stpcpy(params, rep);
    split(repIDc, params);
    if(strcmp(repIDc,"")==0) {
        afficherLigne(messWin, "Requete vide : exit thread\n");
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
            sleep(0);
            char pseudo[BUFF_MAX], cCouleur[BUFF_MAX], texte[BUFF_MAX];
            split(pseudo, params);
            split(cCouleur, params);
            int couleur = atoi(cCouleur);
            split(texte, params);
            afficherMessage(messWin, pseudo, texte, couleur);
            break;
		case 201: // message serveur
            sleep(0);
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
		case 211: // \me
            afficherLigne(messWin, params);
            break;
        case 300:
            afficherLigne(messWin, "Réponse 300\n");
            break;
        case 400: //Erreur
            sleep(0);
            char messErreur[BUFF_MAX];
            split(messErreur, params);
            afficherErreur(messWin, "ERREUR 400", messErreur);
            break;
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
 * @brief Fonction permettant l'envoie de message
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
 * @brief Fonction permettant l'envoie de requete
 * @param soc
 * @param req
 */
void envoyer_requete(int soc, char *req){
	//On envoie la requete au serveur
    CHECK(write(soc, req, strlen(req)+1), "ERREUR WRITE");
}

int main(){
	//Lancement de l'interface graphique
	initialiserCurses();
	
    //Création du socket client
    int socClient;

    socClient = socket(AF_INET,SOCK_STREAM,0);

    //Création de l'adresse du serveur
    struct sockaddr_in addr_serveur;

    addr_serveur.sin_family = AF_INET;
    addr_serveur.sin_port = htons(PORT);
    inet_aton("127.0.0.1",&(addr_serveur.sin_addr));
    memset(addr_serveur.sin_zero,0,8);

    //Connection au serveur
    CHECK(connect(socClient,(struct sockaddr*)&addr_serveur, sizeof(addr_serveur)), "ERREUR CONNECT")

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
