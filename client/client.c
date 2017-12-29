#include "../libs/include.h"

WINDOW *mainWin;

void* gestion_envoie(void *soc) {
    int sock = *(int*) soc;
    char req[BUFF_MAX];
	char message[BUFF_MAX];
	
	fgets(message, BUFF_MAX, stdin);
	while(strcmp(message,"exit\n")!=0){
		sprintf(req,"%i\n%s\n",100,message);
		printf("Envoie du message: %s", req);
		envoyer_requete(sock, req);
		fgets(message, BUFF_MAX, stdin);
	}

	sprintf(req,"%i\n",0);
    printf("Envoie 0\n");
	envoyer_requete(sock, req);
    printf("exit envoie\n");
    pthread_exit(0);
}

void* gestion_lecture(void *soc) {
    int sock = *(int*) soc;
    while(1) {
        lire_reponse(sock);
    }
}

void lire_reponse(int soc){
    char rep[BUFF_MAX];
    int repID=-1;
    //On vide le buffer
    strcpy(rep, "");

    //On lit la réponse
    CHECK(read(soc, rep, BUFF_MAX), "ERREUR READ")
    printf("Requete recu : '%s'\n", rep);
    char *d = strstr(rep, "\n");
    if(d==NULL) {
        printf("Requete vide : exit thread\n");
        pthread_exit(0);
    }
    int pos = d - rep;
    char repIDc[BUFF_MAX];
    strncpy(repIDc, rep, pos);
    repID = atoi(repIDc);
    char params[BUFF_MAX];
    strcpy(params, d+1);
    switch(repID){
        case 0 :
            printf("Réponse 0\n");
            pthread_exit(0);
            break;
        case 200:
            printf("Réponse 200\n");
            break;
        case 300:
            printf("Réponse 300\n");
            break;
        case 400:
            printf("Réponse 400\n");
            break;
        default:
            printf("Requete inconnue\n");
            break;
    }

}

void envoyer_requete(int soc, char *req){
	//On envoie la requete au serveur
    CHECK(write(soc, req, strlen(req)+1), "ERREUR WRITE");

	//lire_reponse(soc);
}

int main(){
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

	/*sprintf(req,"%i\n",100);
	envoyer_requete(socClient, req);
	sprintf(req,"%i\n",0);
	envoyer_requete(socClient, req);*/
    pthread_join(threads[0], NULL);
    printf("Join 0\n");
    pthread_join(threads[1], NULL);
    printf("Join 1\n");
    //On ferme la socket
    close(socClient);
    printf("Close socket 0\n");
    return 0;
}
