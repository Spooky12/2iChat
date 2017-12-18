#include "../libs/include.h"

WINDOW *mainWin;

void lire_reponse(int soc){
    char rep[BUFF_MAX];
    int repID;
    //On vide le buffer
    strcpy(rep, "");

    //On lit la réponse
    CHECK(read(soc, rep, BUFF_MAX), "ERREUR READ")
    sscanf(rep,"%i",&repID);
    switch(repID){
        case 200:
            break;
        case 300:
            break;        
		case 400:
            break;
        default:
            printf("Requete inconnue\n");
            break;
    }
}

void envoyer_requete(int soc, char *req){
	//On envoie la requete au serveur
    CHECK(write(soc, req, strlen(req)+1), "ERREUR WRITE");
	
	lire_reponse(soc);
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
    CHECK(connect(socClient,(struct sockaddr*)&addr_serveur, sizeof(addr_serveur)), "ERREUR CONNECT")

    //On ferme la socket
    close(socClient);
    return 0;
}
