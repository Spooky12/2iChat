#include "../libs/include.h"

struct Salon *salons = NULL;

/**
 * @name lire_requete
 * @brief Fonction permettant de lire un socket
 * @param soc
 * @param param
 */
int lire_requete(int soc, char * param){
	char buffer[BUFF_MAX];
	//On lit la socket
	CHECK(read(soc, &buffer, BUFF_MAX), "ERREUR READ")

	//On recupere la première occurence de \n
	char *d = strstr(buffer, "\n");
	if(d==NULL){
		return -1;
	}

	//On calcule la position du premier \n
	int posi = d - buffer;
	//On stocke le code de la requete dans reqs et on la transforme en int
	char reqs[BUFF_MAX];
	strncpy ( reqs, buffer, posi );
	int req = atoi ( reqs );
	//On stocke les paramètres envoyés avec la requete
	strcpy ( param, d + 1 );
	return req;
}

/**
 * @name diffusion
 * @brief Fonction permettant de diffuser un message à tous les membres d'un salon
 * @param salon
 * @param message
 */
void diffusion(struct Salon *salon, char* message){
	struct Client *c;
	for(c=salon->clients; c != NULL; c=c->hh.next) {
		printf("Client: %s\n", c->nom);
		envoyer_reponse(c->socket, message);
    }
}

/**
 * @name traiter_req0
 * @brief Fonction permettant de traiter une requete de fermeture
 * @param soc
 */
void traiter_req0(int soc){
	char message[BUFF_MAX];
	sprintf(message, "0\n");
	envoyer_reponse(soc, message);
}

/**
 * @name traiter_req100
 * @brief Fonction permettant de traiter une requete de message
 * @param salon
 * @param client
 * @param texte
 */
void traiter_req100(struct Salon *salon, struct Client *client,char* texte){
	char message[BUFF_MAX];
	sprintf(message, "200\n%s\n%i\n%s",client->nom, client->couleur, texte);
	printf("Requete: %s\n", message);
	diffusion(salon, message);
}

/**
 * @name traiter_req101
 * @brief Fonction permettant de traiter une requete de commande
 * @param salon
 * @param client
 * @param texte
 */
void traiter_req101(struct Salon **salon, struct Client *client,char* texte){
	char message[BUFF_MAX];//Message à envoyer au client
	char commande[BUFF_MAX];//Commande entrée par le client
	char param[BUFF_MAX];//parametres de la commende

	split(commande, texte);
	strcpy(param, texte);
	
	printf("Commande: %s\n", commande);
	if(strcmp(commande,"ping")==0){
		ping(message,1);
		envoyer_reponse(client->socket, message);
	}else if(strcmp(commande,"pong")==0){
		ping(message,0);
		envoyer_reponse(client->socket, message);
	}else if(strcmp(commande,"list")==0){
		if(strcmp((*salon)->nom, "Accueil")==0){
			listeSalons(message,(*salon));
		}else{
			listeClients(message,(*salon));
		}
		envoyer_reponse(client->socket, message);
	}else if(strcmp(commande,"pseudo")==0){
		if(pseudo(message, client, (*salon), param)){
			diffusion((*salon), message);//Changement de pseudo ok, on informe tous le monde
		}else{
			envoyer_reponse(client->socket, message);//Erreur lors du changement de pseudo, on informe seulement le client
		}
	}else if(strcmp(commande,"couleur")==0){
		couleur(message, client, param);
		envoyer_reponse(client->socket, message);
	}else if(strcmp(commande,"me")==0){
		me(message, client, param);
		diffusion((*salon), message);
	}else if(strcmp(commande,"info")==0){
		info(message, client, (*salon));
		envoyer_reponse(client->socket, message);
	}else if(strcmp(commande,"rand")==0){
		alea(client, message, param);
		diffusion((*salon), message);
	}else if(strcmp(commande,"create")==0){
		creation(message, param, salon, client);
		envoyer_reponse(client->socket, message);
	}else if(strcmp(commande,"connect")==0){
		connection(message, param, salon, client);
		envoyer_reponse(client->socket, message);
	}else if(strcmp(commande,"leave")==0){
		leave(message, salon, client);
		envoyer_reponse(client->socket, message);
	}else{
		printf("Reception d\"une commande inconnue: \"%s\"\n", commande);
		sprintf(message, "401\n");
		envoyer_reponse(client->socket, message);
	}
}

/**
 * @name envoyer_reponse
 * @brief Fonction permettant de transmettre une requete à un socket
 * @param soc
 * @param req
 */
void envoyer_reponse(int soc, char *req){
	//On envoie la requete au serveur
    CHECK(write(soc, req, strlen(req)+1), "ERREUR WRITE");
}

/**
 * @name envoyer_reponse
 * @brief Fonction permettant d'initialiser le serveur et le salon d'accueil
 */
void initServeur(){
	struct Salon *salon = NULL;
	//Création du salon d'accueil
	salon = (struct Salon*)malloc(sizeof(struct Salon));
	strcpy(salon->nom,"Accueil");
	strcpy(salon->mdp,"");
	salon->clients = NULL;
	HASH_ADD_STR( salons, nom, salon);
}

/**
 * @name traiterClient
 * @brief Boucle principale d'un client
 * @param ptr
 */
void* traiterClient(void* ptr){
	int socClient = *(int *) ptr;
	static int i = 0;
	i++;
	//Création de notre client
	struct Client *client = NULL;
	client = (struct Client*)malloc(sizeof(struct Client));
	client->socket = socClient;
	sprintf(client->nom,"%s%d","chaton", i);
	client->couleur=0;
	//Ajout du client au salon de base
	struct Salon *salon;
	HASH_FIND_STR( salons, "Accueil", salon);
	if(salon!= NULL){
		HASH_ADD_STR( salon->clients, nom, client );
	}
	printf("Le client %s est arrivé sur le chan\n", client->nom);
	
	unsigned int num_users;
	num_users = HASH_COUNT(salon->clients);
	printf("Il y a %u utilisateurs sur le chan\n", num_users);
	
	//Booléen de fin
	short fin = 0;
	//Compteur du nombre d'erreurs
	int nbInconnu = 0;
	//On boucle tant que la fin n'est pas demandée
	while(!fin && nbInconnu<10){
		//On initialise une chaine param vide
		char param[BUFF_MAX] = "";
		//On attend une requete du client		
		int resultat = lire_requete(socClient, param);
		//On gere les différentes requetes possibles
		switch(resultat){
			case 100:
				traiter_req100(salon, client, param);
			break;			
			case 101:
				traiter_req101(&salon, client, param);
			break;
			case 0:
				printf("Requete 0\n");
				traiter_req0(socClient);
				fin = 1;
			break;
			default:
			nbInconnu++;
				printf("Code inconnu\n");
			break;
		}
	}
	//on retire le client du salon
	HASH_DEL( salon->clients, client );
	//On ferme la socket client
	close(socClient);
	return NULL;
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
    if(strcmp(dest, "") == 0) {
        split(dest, params);
    }
}

/**
 * @name fermerCurses
 * @brief Fonction permettant de fermer correctement ncurses
 */
void fermerCurses(){
}

/**
 * @name main
 * @brief Fonction principale du serveur, recevant et initialisant chaque nouveau client
 */
int main(){
	//Création des sockets
	int socServeur, socClient;
	//Création des adresses
	struct sockaddr_in addr_serveur;
	struct sockaddr_in addr_client;
	int tailleClient;
	unsigned int serveur_taille;

	socServeur = socket(AF_INET,SOCK_STREAM,0);

	addr_serveur.sin_family = AF_INET;
	addr_serveur.sin_port = htons(PORT);
	addr_serveur.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(addr_serveur.sin_zero,0,8);

	//On bind le socket et on le met en mode écoute
	CHECK(bind(socServeur, (struct sockaddr*)&addr_serveur, sizeof(addr_serveur)), "ERREUR BIND");
	CHECK(listen(socServeur,1),"ERREUR LISTEN")

	//Innitialisation du serveur et de l'aléatoire
	initServeur();
	srand(time(NULL));
	
	while(1){
		//On récupère la taille de l'adresse du client
		tailleClient = sizeof(struct sockaddr);

		//On attend qu'un client se connecte
		CHECK((socClient = accept(socServeur, (struct sockaddr*)&addr_client, (unsigned int*)&tailleClient)), "ERREUR ACCEPT")
		//On affiche le port et l'adresse du client
		printf("Port : %d\n", ntohs(addr_serveur.sin_port));
		printf("Adresse du client: %s\n", inet_ntoa(addr_client.sin_addr));

		pthread_t thread_client;
		pthread_create(&thread_client, NULL, traiterClient, (void *)&socClient);
		pthread_detach(thread_client);
	}
	//On ferme la socket serveur
	close(socServeur);
	return 0;
}
