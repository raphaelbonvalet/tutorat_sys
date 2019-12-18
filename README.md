# Tutorat Systeme - Bonvalet Raphael - Dubois Vincent


## Programmation PC avec libusb-1.0

### Trouver le bon peripherique (exam()):
Cette fonction examine tous les périphériques USB disponibles sur les bus USB de la machine. Dès qu’un périphérique s’avère être le périphérique USB du type cherché, on sauve la "poignée" vers ce périphérique dans une variable globale.
Pour cela, on cree une liste (list) de **libusb_device** et on utilise la fonction **libusb_get_device_list** pour la remplir (equivalent a faire lsblk dans un terminal). On parcour ensuite cette liste en comparant l'**idVendor** et l'**idProduct** de chaque device avec ceux correspondant au materiel recherche. La poignee de ce device est sauvegardee dans la variable globale **deviceHandle** grace a la fonction **libusb_open**.

### Recuperer les onfigurations/interfaces/points d'acces du peripherique (config())
On recupere dans un premier temps la premiere configuration du peripheriques grace a la fonction **libusb_get_config_descriptor** que l' on sauvegarde dans une variable de type **libusb_config_descriptor**. Cette configuration est ensuite installee comme configuration courante avec la fonction **libusb_set_configuration**.
De base, l'ATMega16u2 qui gere la communication USB de l'arduino est deja bien parametre donc le noyau linux s'est deja approprie les interfaces de la configuration courante. Pour reprendre la main sur le peripherique on utilise la fonction **libusb_detach_kernel_driver** pour chaque interface de la configuration courante. Ce morceau de code ne sera plus necessaire quand la programmation de l'ATMega16u2 sera terminee.
Enfin, on recupere les points d'acces en mode interruption. Pour cela, on s'approprie toutes les interfaces de la configuration en les parcourants a l'aide d'une boucle (de 0 a **bNumInterfaces**). Il faut faire attention ici car l'indice de l'interface dans le tableau de toutes les interfaces ne correcpond pas au numero de l'interface **bInterfaceNumber**. Pour chacune des interfaces reclamees, on parcours la liste des points d'acces et on sauvegarde seulement ceux en mode interruption (**LIBUSB_TRANSFER_TYPE_INTERRUPT**) dans un tableau place en parametre de la fonction config()

### Rececption d'un message (Recieve())

### Envoie d'un message (Send())

### Fermeture du peripherique usb (free_interfaces())
On recpere la configuration active puis on parcour la liste de ses interfaces pour les liberer avec **libusb_release_interface**. Enfin, on ferme la "poignée": **libusb_close(deviceHandle)**.


## Programmation ATMega328p
On utilise le fichier c fournis qui contient deja les fonctions permettant notamment de gerer la liaison série avec le 328p. Dans la fonction main():
* On initialise la liaison série (à 9600 bauds) ainsi que les ports d'entrée (ADC sur PORTD) et de sortie (PORTB) avant de rentrer dans une boucle infinie.
* Pour lire les valeurs des deux axes du joystick on initialise alternativement l'**ADC0** et l'**ADC1** puis on stock la valeur lue dans la variable **newjoystick**. Si cette valeur est différente de l'ancienne valeur (stockée dans le tableau **joystick**) alors on l'envoie sur la liaison série. On a pris soin de ne garder que les 6 premiers bits de la valeur lue sur le joytick pour eviter que de saturer la liaison série au moindre petit mouvement du joystick.
* On lit aussi la valeur des boutons et de la meme maniere on ne l'envoie que si cette derniere à changé depuis le dernier relevé.
* Concernant la reception de messages, on s'assure que a liaison série est disponible avec la fonction **serial_available()** avant de lire dans le registre **UDR0**. Enfin on gere les LED avec une fonction dédiée (LED()): on utilise les lettres de a à f pour éteindre une LED et de A à F pour les allumer. 


## Programmation ATMega16u2

### Descripteur.h
* On a cree deux interfaces avec **USB_Descriptor_Interface_t**: une pour l'entree avec 2 endpoint (joystick et boutons) et une autre pour la sortie avec 1 seul endpoint.
* Chaque interface est dotee d'un identifiant (0 pour l'entrée et 1 pour la sortie)
* Dans les macros on definie les addresses des points d'acces (1,2 et 3) et on définie leur taille à 1 octet.

### Descripteur.c
* On a definie un **idVendor** et **idproduct** pour notre périphérique.
* On parametre les interfaces et les endpoints declarés dans le fichier **Descripteur.h**: identifiants, nombre de point d'acces par interface, adresse des points d'acces, etc...

### PAD.c
* Initialise les endpoints (ligne 91)
* Fonction Main pour envoyer recevoir
38
87
