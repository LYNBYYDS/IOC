---
title: IOC-TP2
---

# IOC-TP2 Pilote de périphérique

## Auteurs
 - AUTHIER Benjamin
 - LI Yong

## Réponses aux questions du TME

 - __Quelle fonction est exécutée lorsqu'on insère le module du noyau ?__
     - &emsp;La fonction qui est exécutée lorsqu'on insère le module noyeu est mon_module_init
    
 - __Quelle fonction est exécutée lorsqu'on enlève le module du noyau ?__
     - &emsp;La fonction qui est exécutée lorsqu'on enlève le module noyeu est mon_module_cleanup

 - __Comment voir que le paramètre a bien été lu ?__
    
     - &emsp; Pour être sûr que le paramètre a bien été lu, on peut utiliser `printk(KERN_DEBUG "param=%d !\n", param)` pour afficher la valeur des paramètres. Ainsi, lorsqu’on réutilise la commande `dmesg` on peut voir un message `param=<la valeur choisi>`. Dans notre cas, on verra `btn = 18` pour le premier code fourni. et `LED 0 = 4 <saut de ligne> LED 1 = 17` pour le deuxième code donné dans le sujet. Pour être sûr que les paramètres étaient lus correctement, nous avons testé avec différentes valeurs.

 - __Comment savoir que le device a été créé ?__
    
     - &emsp;Pour être sûr que le divice a bien été créer, on peut utiliser la commande `cat /proc/devices | grep led_LA` qui nous affiche la liste de tous les divices. Il nous suffit de chercher notre divice `Led0_LA` pour constater qu’il a bien été créé.

 - __Expliquer comment insdev récupère le numéro major__
    
     - &emsp;Le numéro major est récupéré dans insdev grâce à la ligne `major=$(awk "\$2==\"$module\" {print \$1;exit}" /proc/devices)` qui permet de prendre le numéro major de notre divice directement dans la liste des divice dans le fichier /proc/devices. La commande `awk` est un outil qui est largement utilisé pour le traitement de texte et les tâches de manipulation de données dans les environnements Linux. Ici, il est utilisé pour extraire des champs ou des valeurs spécifiques des données. `$2` prend le deuxième argument et le `$1` est l’argument devant lui, donc ça veut dire qu’il va sortir l’argument juste avant de trouver l’argument qui est le même que le variable `module` et le stocker dans la variable `major`.






## Expériences réalisées

### Pilote de périphérique

 - Création et test d'un module noyau
 - Ajout des paramètres au module
 - Création d'un driver qui ne fait rien, mais qui le fait dans le noyau
 - Accès aux GPIO depuis les fonctions du pilote

#### Créer le premier module du noyau=

Nous avons tout d'abord créé un ficher module.c pour le module noyau.


~~~c
    #include <linux/module.h>
    #include <linux/init.h>

    MODULE_LICENSE("GPL");
    MODULE_AUTHOR("li_authier, 2023");
    MODULE_DESCRIPTION("Module");

    static int __init mon_module_init(void)
    {
        int i;
        printk(KERN_DEBUG "Hello World, this is cart 23 controled by li_authier !\n");
    }

    static void __exit mon_module_cleanup(void)
    {
       printk(KERN_DEBUG "Goodbye World, this is cart 23 controled by li_authier !\n");
    }

    module_init(mon_module_init);
    module_exit(mon_module_cleanup);
~~~   
    

    
#### Compiler le module
    
&emsp;Après avoir créé le fichier `module.c` à partir du code fourni dans la rubrique TP. Nous avons aussi créé le `Makefile` avec le code fourni, en remplacent `LOGIN` par li_authier, et `CARD_NUMB` par 23 le numéro de notre carte.

```
CARD_NUMB       = 23
ROUTER          = peri
LOGIN           = li_authier
LAB             = lab2/module_test

MODULE          = module
MODULE_1para	= module_un_parametre
MODULE_nparas	= module_plus_parametre

CROSSDIR        = /users/enseig/franck/IOC
KERNELDIR       = /dsk/l1/misc/linux-rpi-3.18.y
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-
        
obj-m           += $(MODULE).o
obj-m           += $(MODULE_1para).o
obj-m           += $(MODULE_nparas).o
default:;       make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules
clean:;         make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean
upload:
	scp -P622$(CARD_NUMB) $(MODULE).ko pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) $(MODULE_1para).ko pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) $(MODULE_nparas).ko pi@$(ROUTER):$(LOGIN)/$(LAB)
```
     
Avec les commendes `make` et `make upload` on compile et on transfère le fichier sur la carte Raspeberry Pi.
    
#### Insert le module dans la carte Rasberry Pi

&emsp;La commande `sudo insmod ./module.ko` permet d’insérer le module `module` dans le kernel. Après cela, avec `lsmod`, nous pouvons voir la liste des modules existants et nous avons bien vu la présence de notre module `module`. De plus, avec `dmesg`, qui nous permet d’afficher tous les messages du tampon du noyau, on peut voir que le message `Hello World, this is cart 23 controled by li_authier !` a bien été émis par le module `module`.
    
Enfin, la commande `sudo rmmod module` supprime le module. En répétant les étapes précédentes avec `lsmod` `dmesg` nous pouvons voir que notre module `module` n’est plus présent et que le message `Goodbye World, this is cart 23 controled by li_authier!` a été ecrit dans les messages du noyau.

#### Changer le module pour qu'il puisse prendre des arguments
    
&emsp;Dans cette parite, nous avons modifié le `module.c` pour prendre un paramètre lors de l’insertion du module. Pour cela, nous utilisons les fonctions `module_param` et `MODULE_PARM_DESC` comme indiqué par les codes fournis dans le sujet. Avec cela, lorsque nous insérons le module, nous pouvons prendre un paramètre avec la commande `sudo insmod . /module.ko <param>=<value>`

Les fonctions sont utiliser de la manière suivante dans le code source:

```c 
module_param(param, int, 0); 
MODULE_PARM_DESC(param, "inserer la valeur du parametre");
```
    
#### Création d'un driver qui ne fait rien, mais qui le fait dans le noyau

&emsp;D’abord dans un nouveau dossier `1led_LA`, nous avons créé le fichier `led_LA.c` (LA pour li_authier). Le fichier a été écrit à partir du code fourni dans le sujet et permet d’initialiser un pilote dans le but de changer l’état d’une LED. Le `Makefile` à donc été modifié en changeant la variable `LAB` par `lab2/led_LA` afin que le fichier compilé du pilote se trouve dans le dossier `/lab2` de la carte. la variable `MODULE` a été modifié par `led_LA`, qui est le nouveau nom de notre pilote.

Une fois que nous avons compilé le fichier `led_LA.c`, avec la commande `make all`, nous pouvons transféré le fichier à la carte rasberry Pi grace à `make upload`
    
Lorsque nous avons inséré le pilote, celui-ci exécute la fonction `mon_module_init`, qui contient le code `major = register_chrdev(0, « led_LA », &fops_led);` qui indique au système Linux de trouver un numéro majeur qui n’a pas encore été utilisé, et enregistre le numéro dans la variable `major`.
    
    
Ensuite, nous voulons créer un système de fichiers de nœud de ce lecteur pour le rendre accessible en mode utilisateur, mais pour cela nous devons connaître le numéro `majeur` du pilote. Pour le trouver, nous avons utilisé la commande `cat /proc/devices | grep led_LA`, qui affiche les lignes du fichier `/proc/devices` contenant le mot `led0_LA`. Ici, nous avons le `major` 246.

nous créons le nœud avec le numéro principal obtenu précédemment avec la commande `sudo mknod /dev/led0_LA c <major number> 0` et les commandes `sudo chmod a+rw /dev/led0_LA` créent respectivement un nœud et donnent à tous les utilisateurs le droit d’utiliser ce driver.

#### Automatiser le chargement du driver et son effacement à l'aide de deux scripts shell

&emsp;Pour automatiser les étapes décrites précédemment, on crée les scripts `insdev.sh` et `rmdev.sh` qui vont respectivement insérer le driver et crée le nœud pour le premier et retirer le driver et enlever le nœud pour le deuxième.

Pour déplacer `insdev.sh` et `rmdev.sh` dans la Raspberry Pi nous utilisons la commande `scp -P62223 *.sh pi@peri:li_authier/lab2/led0_LA`. Ces fichiers contiennent des lignes de commande qui seront exécutées dans le shell, donc il faut donner le droit de les exécuter en utilisant la commande `chmod +x insdev.sh` et `chmod +x rmdev.sh`.

#### Accès aux GPIO depuis les fonctions du pilote
&emsp;Ici, nous créons un nouveau dossier `3ledbp_module` pour stocker les fichées que nous allons créer pour contrôler les GPIO depuis les fonctions du pilote. 

Le travail que nous allons faire est divisé en 3 parties :

- Création des drivers complets pour le contrôle du bouton et des leds.
- Chargement des exécutables des drivers dans le noyau à l’aide d’un `Makefile`.
- Écriture d’un programme en mode utilisateur qui fait fonctionner les leds et le bouton grâce aux drivers.
    

##### les drivers :
&emsp;Nous avons fait 3 drivers au total:
* Un driver nommé `led0_LA` pour contrôler la LED0, utilisé en mode `WRONLY`, il nous permet de changer l’état de la LED0.
* Un driver nommé `led1_LA` pour contrôler la LED0, utilisé en mode `WRONLY`, il nous permet de changer l’état de la LED1.
* Un driver `bp_LA` pour lire l’état du bouton poussoir, utilisé en mode `RDONLY`.


##### Le `Makefile`

&emsp;Pour compiler les fichiers `.c` des drivers en des fichiers exécutables `.ko`, et également envoyer les exécutables vers la carte Raspberry Pi, nous avons écrit un fichier `Makefile`. Grace au `Makefile`, avec la commande `make` ou `make all` on peut compiler tous les fichiers source des drivers. La commande `make upload` permet d'envoyer les fichiers exécutables vers la carte Raspberry Pi. Enfin, la commande `make uploadsh` permet d'envoyer les scripts vers la carte.
    
##### Le programme utilisateur
    
&emsp;Dans le fichier `main.c` on a écrit un programme qui utilise les API des drivers pour utiliser les LED et le bouton. Pour cela, nous utilisons les fonctions `open`, `read` et `write` pour initialiser les GPIO et lire ou écrire une valeur des GPIO.

##### Tuto utilisation

&emsp;Il ne reste plus qu’à utiliser la entrer la commande `Make` dans le terminal de l’ordinateur de développement pour compiler les fichiers sources des drivers et de la fonction utilisateur. De plus `Make` upload les fichiers exécutables ainsi que les ficher shell sur la Raspeberry PI.

Il faut ensuite se connecter au terminal de la Raspeberry pour insérer les drivers et lancé le programme `main.x`. Le fichier `connect.sh` automatique la connection, il suffit donc de d’entrer la commande `./connect.sh`. Si le fichier n’est pas dans le bon mode, on peut utiliser la commande `chmod +x connect.sh`.

Une fois connecté à la carte, il faut changer le mode des fichiers `insdev.sh` et `rmdev.sh` pour qu’ils puissent être exécutés. La commande `chmod +x *.sh` permet de passer en mode exécutable tous les ficher `.sh`. On peut maintenant insérer les drivers avec `sudo ./insdeb.sh`.

Enfin, dernière étape, nous pouvons lancer l’exécutable `main.x` qui utilise les drivers que nous avons créés. Pour cela, on rentre la commande `./main.x`.

Ne pas oublier de retirer les drivers avec la commande `sudo ./rmdev.sh`, qui exécutera le fichier shell `rmdev.sh` pour retirer le driver.

    
    
    
    
    