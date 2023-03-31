/* A simple server in the internet domain using TCP The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <arpa/inet.h>

void error(const char *msg)
{
        perror(msg);
        exit(1);
}

int main(int argc, char *argv[])
{
        int sockfd, newsockfd, portno;
        socklen_t clilen;
        char buffer[256];
        struct sockaddr_in serv_addr, cli_addr;
        int n;

        if (argc < 2) {
                fprintf(stderr, "ERROR, no port provided\n");
                exit(1);
        }

        // 1) on crée la socket, SOCK_STREAM signifie TCP

        /* La fonction socket ouvre une socket TCP et prend trois arguments:
            - domain: ici AF_INET, qui indique l'utilisation du protocole Internet (IPv4).
            - type: ici SOCK_STREAM, qui indique l'utilisation du protocole de transmission de données TCP.
            - protocol: ici 0, qui indique que le protocole TCP sera utilisé.
        */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);   
        if (sockfd < 0)
                error("ERROR opening socket");

        // 2) on réclame au noyau l'utilisation du port passé en paramètre 
        // INADDR_ANY dit que la socket va être affectée à toutes les interfaces locales

        /*    bzero() initialise les octets de l'adresse du serveur serv_addr à 0.
            atoi() convertit la chaîne de caractères de l'argument en un entier qui représente le numéro de port.
            serv_addr.sin_family est initialisé à AF_INET pour spécifier l'utilisation d'IPv4.
            serv_addr.sin_addr.s_addr est initialisé à INADDR_ANY, ce qui signifie que le serveur acceptera les connexions entrantes sur toutes les interfaces réseau disponibles.
            serv_addr.sin_port est initialisé au numéro de port fourni en argument, converti en format de réseau à l'aide de htons() pour s'assurer que l'ordre des octets est correct.
            Enfin, la fonction bind() lie la socket au port et à l'adresse spécifiés dans la structure serv_addr. Si la liaison échoue, la fonction error() est appelée avec un message d'erreur correspondant.
        */
        
        bzero((char *) &serv_addr, sizeof(serv_addr));
        portno = atoi(argv[1]);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
                error("ERROR on binding");


        // On commence à écouter sur la socket. Le 5 est le nombre max
        // de connexions pendantes

        /*
            listen(sockfd, 5); permet à la socket sockfd d'écouter les connexions entrantes, avec une file d'attente pouvant contenir jusqu'à 5 connexions en attente
            accept() permet au serveur d'accepter une nouvelle connexion entrante
            bzero remplit le tableau buffer de 256 octets avec des zéros.
            read() est utilisée pour lire les données envoyées par le client et les stocker dans le buffer
            le serveur affiche le message reçu avec les informations du client à l'aide de la fonction printf()  
            la connexion est ensuite fermée à l'aide de la fonction close().
        */

        listen(sockfd, 5);      
        while (1) {
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                if (newsockfd < 0)
                    error("ERROR on accept");

                bzero(buffer, 256);
                n = read(newsockfd, buffer, 255);
                if (n < 0)
                    error("ERROR reading from socket");

                printf("Received packet from %s:%d\nData: [%s]\n\n",
                       inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port),
                       buffer);

                close(newsockfd);
        }

        close(sockfd);
        return 0;
}