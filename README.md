# Tutorat Systeme - Bonvalet Raphael - Dubois Vincent


## Programmation PC avec libusb-1.0

### Trouver le bon peripherique (fonction exam()):
Cette fonction examine tous les périphériques USB disponibles sur les bus USB de la machine. Dès qu’un périphérique s’avère être le périphérique USB du type cherché, on sauve la "poignée" vers ce périphérique dans une variable globale.
Pour cela, on cree une liste (list) de **libusb_device** et on utilise la fonction **libusb_get_device_list** pour la remplir (equivalent a faire lsblk dans un terminal). On parcoure ensuite cette liste en comparant l'**idVendor** et l'**idProduct** de chaque device avec ceux correspondant au materiel recherché. La poignée de ce device est sauvegardée dans la variable globale **deviceHandle** grâce à la fonction **libusb_open**.

### Récuperer les configurations/interfaces/points d'accès du périphérique (fonction config())
On recupere dans un premier temps la premiere configuration du périphériques grâce à la fonction **libusb_get_config_descriptor** que l'on sauvegarde dans une variable de type **libusb_config_descriptor**. Cette configuration est ensuite installée comme configuration courante avec la fonction **libusb_set_configuration**.
De base, l'ATMega16u2 qui gere la communication USB de l'arduino est deja bien paramètré donc le noyau linux s'est déjà approprie les interfaces de la configuration courante. Pour reprendre la main sur le périphérique on utilise la fonction **libusb_detach_kernel_driver** pour chaque interface de la configuration courante. Ce morceau de code ne sera plus nécessaire quand la programmation de l'ATMega16u2 sera terminée.
Enfin, on récupère les points d'accès en mode interruption. Pour cela, on s'approprie toutes les interfaces de la configuration en les parcourants a l'aide d'une boucle (de 0 a **bNumInterfaces**). Il faut faire attention ici car l'indice de l'interface dans le tableau de toutes les interfaces ne correspond pas au numéro de l'interface **bInterfaceNumber**. Pour chacune des interfaces reclamées, on parcours la liste des points d'accès et on sauvegarde seulement ceux en mode interruption (**LIBUSB_TRANSFER_TYPE_INTERRUPT**) dans un tableau placé en parametre de la fonction config()

### Envoie et reception d'un message (fonctions Send() et Recieve())
On utilise la fonction **libusb_interrupt_transfer** qui permet d'envoyer et recevoir un message de maniere asynchrone. Cette derniere prend en parametre la poignée ainsi que le point d'acces concerné.

### Fermeture du peripherique usb (free_interfaces())
On recupère la configuration active puis on parcoure la liste de ses interfaces pour les libérer une à une avec **libusb_release_interface**. Enfin, on ferme la "poignée": **libusb_close(deviceHandle)**.


## Programmation ATMega328p
On utilise le fichier c fourni qui contient déjà les fonctions permettant notamment de gérer la liaison série avec le 328p. Dans la fonction main():
* On initialise la liaison série (à 9600 bauds) ainsi que les ports d'entrée (ADC sur PORTD) et de sortie (PORTB) avant de rentrer dans une boucle infinie.
* Pour lire les valeurs des deux axes du joystick on initialise alternativement l'**ADC0** et l'**ADC1** puis on stock la valeur lue dans la variable **newjoystick**. Si cette valeur est différente de l'ancienne valeur (stockée dans le tableau **joystick**) alors on l'envoie sur la liaison série. On a pris soin de ne garder que les 6 premiers bits de la valeur lue sur le joytick pour eviter que de saturer la liaison série au moindre petit mouvement du joystick.
* On lit aussi la valeur des boutons et de la même manière on ne l'envoie que si cette derniere a changé depuis le dernier relevé.
* Concernant la reception de messages, on s'assure que a liaison série est disponible avec la fonction **serial_available()** avant de lire dans le registre **UDR0**. Enfin on gere les LED avec une fonction dédiée (LED()): on utilise les lettres de a à f pour éteindre une LED et de A à F pour les allumer.


## Programmation ATMega16u2

### Descripteur.h
* On a cree deux interfaces avec **USB_Descriptor_Interface_t**: une pour l'entrée avec 2 endpoint (joystick et boutons) et une autre pour la sortie avec 1 seul endpoint.
* Chaque interface est dotée d'un identifiant (0 pour l'entrée et 1 pour la sortie)
* Dans les macros on définie les addresses des points d'accès (1,2 et 3) et on définit leur taille à 1 octet.

### Descripteur.c
* On a defini un **idVendor** et **idproduct** pour notre périphérique. Ce sont ces deux meme numeros qui seront recherches dans le programme sur l'ordinateur.
* On paramètre les interfaces et les endpoints declarés dans le fichier **Descripteur.h**: identifiants, nombre de point d'accès par interface, adresse des points d'accès, etc...

### PAD.c
* On inclue le fichier **LUFA/Drivers/Peripheral/Serial.h** pour pouvoir communiquer entre l'ATMega16u2 et l'ATMega328p.
* Dans la fonction **main**, pour envoyer un message de l'ordinateur a l'ATMega328p, on selectionne le point d'accès **PAD_OUT_EPADDR** puis on récupère le message **Endpoint_Read_8()** avant de l'envoyer avec **Serial_SendByte** sur la liaison serie.
* Initialise les endpoints dans la fonction **EVENT_USB_Device_ConfigurationChanged**
