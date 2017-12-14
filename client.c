#include "include.h"

/*int req100(int soc){
	//On crée le buffer avec 100 comme requete
	char buffer[BUFF_MAX] = "100\n";
	//On envoie la requete au serveur
	TEST(write(soc, buffer, strlen(buffer)+1), "ERREUR WRITE")
	//on vide le buffer
	strcpy(buffer, "");

	//On lit la réponse
	TEST(read(soc, buffer, BUFF_MAX), "ERREUR READ")

	//On récupère la première occurence de \n
	char* d = strstr(buffer, "\n");
	if (d==NULL){
		return -1;
	}

	//On renvoi le nombre d'éléments
	return atoi(d+1);
}

void req200(int soc, int index){
	//On crée le buffer
	char buffer[BUFF_MAX];
	//On ajoute la requete et l'index au buffer
	sprintf(buffer, "200\n%d", index);
	//On envoie la requete au serveur
	TEST(write(soc, buffer, strlen(buffer)+1), "ERREUR WRITE");
	//On vide le buffer
	strcpy(buffer, "");

	//On lit la réponse
	TEST(read(soc, buffer, BUFF_MAX), "ERREUR READ")
	//On récupère la première occurence de \n
	char* d = strstr(buffer, "\n");

	if(d==NULL){
		return;
	}

	//On récupère la position du \n
	int posi = d - buffer;
	char reps[BUFF_MAX];
	//On met les posi premiers elements de buffer dans reps (Le code requète)
	strncpy ( reps, buffer, posi );
	//On récupère le code de la requete de réponse
	int rep = atoi ( reps );

	if(rep == 200){
		printf ("Indice %d : %s\n", index, d + 1 );
	}else if(rep == 250){
		printf("L'indice %d n'existe pas\n", index);
	}else{
		printf("Erreur\n");
	}
}

void req0(int soc){
	//On crée le buffer avec 100 comme requete
	char buffer[BUFF_MAX] = "0\n";
	//On envoie la requete au serveur
	TEST ( write ( soc, buffer, strlen ( buffer ) + 1 ), "ERREUR WRITE" );
}*/

void traiter_requete(int soc, char *req, int *taille){
    char rep[BUFF_MAX];
    int repID;
    //On envoie la requete au serveur
    TEST(write(soc, req, strlen(req)+1), "ERREUR WRITE");
    //On vide le buffer
    strcpy(rep, "");

    //On lit la réponse
    TEST(read(soc, rep, BUFF_MAX), "ERREUR READ")
    sscanf(rep,"%i",&repID);
    switch(repID){
        case 0:
            printf("Serveur bien fermé\n");
            break;
        case 100:
            sscanf(rep,"%i\n%i",&repID,taille);
            printf("Nombre d'elements: %d\n", *taille);
            break;
        case 200:
            //%[^\t\n] est utilisé car %s s'arrète au premier espace blanc (espace, tab, entrée)
            sscanf(rep,"%i\n%[^\t\n]",&repID, rep);
            printf("Phrase renvoyée: %s\n", rep);
            break;
        case 250:
            printf("Indice out of bound\n");
            break;
        default:
            printf("Requette inconnue\n");
            break;
    }
}

int main(){
    //Création du socket client
    int socClient;
    char req[BUFF_MAX];

    socClient = socket(AF_INET,SOCK_STREAM,0);

    //Création de l'adresse du serveur
    struct sockaddr_in addr_serveur;

    addr_serveur.sin_family = AF_INET;
    addr_serveur.sin_port = htons(PORT);
    inet_aton("127.0.0.1",&(addr_serveur.sin_addr));
    memset(addr_serveur.sin_zero,0,8);

    //Connection au serveur
    TEST(connect(socClient,(struct sockaddr*)&addr_serveur, sizeof(addr_serveur)), "ERREUR CONNECT")

    //Récupération du nombre d'éléments dans le serveur
    //int nb = req100(socClient);
    int nb;
    memset(req, BUFF_MAX,0);
    sprintf(req,"%i\n",100);
    traiter_requete(socClient, req, &nb);

    if(nb>0){
        //On affiche le nombre d'elements dans le serveur et on les affiche tous plus un elements pour tester le cas ou on est en dehors des bornes du tableau
        for(int i=0; i<nb+1; i++){
            sprintf(req,"%i\n%i",200,i);
            traiter_requete(socClient, req, &nb);
            //req200(socClient, i);
        }
    }else{
        printf("Le serveur est vide\n");
    }

    //On ferme la liaison avec le serveur
    sprintf(req,"%i\n",0);
    traiter_requete(socClient, req, &nb);
    //req0(socClient);
    //On ferme la socket
    close(socClient);
    return 0;
}
