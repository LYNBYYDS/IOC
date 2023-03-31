#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
        perror(msg);
        exit(0);
}

int main(int argc, char *argv[])
{
        int sockfd, portno, n;
        struct sockaddr_in serv_addr;
        struct hostent *server;

        char buffer[256];

        // Le client doit connaitre l'adresse IP du serveur, et son numero de port
        /*
            Ici on verrifie si le nombre d'argument avec if (argc < 3)
            Si le nombre n'est pas suffisant on renvoie un message d'erreur
            Sinon on recupere le numero de port passe en deuxieme argument dans la variable portno
            atoi converti la chaîne de caractères en entier
        */
        if (argc < 3) {
                fprintf(stderr,"usage %s hostname port\n", argv[0]);
                exit(0);
        }
        portno = atoi(argv[2]);

        // 1) Création de la socket, INTERNET et TCP
        /*
            La fonction socket ouvre une socket TCP et prend trois arguments:
                - domain: ici AF_INET, qui indique l'utilisation du protocole Internet (IPv4).
                - type: ici SOCK_STREAM, qui indique l'utilisation du protocole de transmission de données TCP.
                - protocol: ici 0, qui indique que le protocole TCP sera utilisé.
            gethostbyname(argv[1]); est utilisée pour récupérer l'adresse IP du serveur correspondant au nom d'hôte passé en paramètre argv[1]
        */

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                error("ERROR opening socket");

        server = gethostbyname(argv[1]);
        if (server == NULL) {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
        }

        // On donne toutes les infos sur le serveur
        /*
            La fonction bzero initialise tous les champs de la structure serv_addr à 0.
            serv_addr.sin_family est défini à AF_INET pour indiquer qu'il s'agit d'une adresse IPv4.
            bcopy copie l'adresse IP de l'hôte server->h_addr vers la structure serv_addr.sin_addr.s_addr, server->h_length est le nombre d'octet qu'on souhaite copier
            serv_addr.sin_port est initialisé avec le numéro de port de l'hôte, converti en ordre d'octets réseau à l'aide de la fonction htons.
        */

        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(portno);

        // On se connecte. L'OS local nous trouve un numéro de port, grâce auquel le serveur
        // peut nous renvoyer des réponses, le \n permet de garantir que le message ne reste
        // pas en instance dans un buffer d'emission chez l'emetteur (ici c'est le clent).
        /*
            connect() essaie de se connecter au serveur en utilisant le socket sockfd créé précédemment et en utilisant l'adresse du serveur stockée dans la structure serv_addr.
            connect() prend 3 arguments : 
                -le descripteur de socket
                -une structure sockaddr qui spécifie l'adresse du serveur auquel le client veut se connecter
                -la taille de la structure sockaddr
            strcpy() copie la chaîne de caractères "Coucou Peri\n" dans le tampon de données buffer
            write() est utilisée pour envoyer le contenu de buffer au serveur via le socket sockfd.
            write() prend 3 arguments : 
                - le descripteur de socket où écrire
                - un pointeur vers le début des données à écrire ici le buffer
                - le nombre de bytes à écrire
        */

        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
                error("ERROR connecting");

        int vote;
        printf("Fait ton choix mon grand :\n\t - [1] Ete\n\t - [2] hiver ?\n");
        scanf("%d",&vote);
        sprintf(buffer,"%d",vote); // On met le resultat du vote de le buffer

        n = write(sockfd,buffer,strlen(buffer));
        if (n != strlen(buffer))
                error("ERROR message not fully trasmetted");

        // On ferme la socket

        close(sockfd);
        return 0;
}