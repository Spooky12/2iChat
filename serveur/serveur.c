#include "../libs/include.h"

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

void traiter_req0(int soc){
	char message[BUFF_MAX];
	sprintf(message, "200\n");
	envoyer_reponse(soc, message);
}

void traiter_req100(int soc){
	char message[BUFF_MAX];
	sprintf(message, "200\n");
	envoyer_reponse(soc, message);
}

void envoyer_reponse(int soc, char *req){
	//On envoie la requete au serveur
    CHECK(write(soc, req, strlen(req)+1), "ERREUR WRITE");
}

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


	while(1){
		//On récupère la taille de l'adresse du client
		tailleClient = sizeof(struct sockaddr);

		//On attend qu'un client se connecte
		CHECK((socClient = accept(socServeur, (struct sockaddr*)&addr_client, (unsigned int*)&tailleClient)), "ERREUR ACCEPT")
		//On affiche le port et l'adresse du client
		printf("Port : %d\n", ntohs(addr_serveur.sin_port));
		printf("Adresse du client: %s\n", inet_ntoa(addr_client.sin_addr));

		//Booléen de fin
		short fin = 0;
		//On boucle tant que la fin n'est pas demandée
		while(!fin){
			//On initialise une chaine param vide
			char param[BUFF_MAX] = "";
			//On attend une requete du client
			int resultat = lire_requete(socClient, param);
			
			//On gere les différentes requetes possibles
			switch(resultat){
				case 100:
					printf("Requete 100\n");
					traiter_req100(socClient);
				break;
				case 0:
					printf("Requete 0\n");
					traiter_req0(socClient);
					fin = 1;
				break;
				default:
					printf("Code inconnu\n");
				break;
			}
		}
		//On ferme la socket client
		close(socClient);
	}
	//On ferme la socket serveur
	close(socServeur);
	return 0;
}
