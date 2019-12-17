# Tutorat Systeme - Bonvalet Raphael - Dubois Vincent

## Programmation PC avec libusb-1.0

### Trouver le bon peripherique (exam()):
Cette fonction examine tous les périphériques USB disponibles sur les bus USB de la machine. Dès qu’un périphérique s’avère être le périphérique USB du type cherché, on sauve la "poignée" vers ce périphérique dans une variable globale.
Pour cela, on cree une liste (list) de **libusb_device** et on utilise la fonction **libusb_get_device_list** pour la remplir (equivalent a faire lsblk dans un terminal). On parcour ensuite cette liste en comparant l'**idVendor** et l'**idProduct** de chaque device avec ceux correspondant au materiel recherche. La poignee de ce device est sauvegardee dans la variable globale **deviceHandle** grace a la fonction **libusb_open**.

### Recuperer les onfigurations/interfaces/points d'acces du peripherique (config())
On recupere dans un premier temps la premiere configuration du peripheriques grace a la fonction **libusb_get_config_descriptor** que l' on sauvegarde dans une variable de type **libusb_config_descriptor**. Cette configuration est ensuite installee comme configuration courante avec la fonction **libusb_set_configuration**

De base, l'ATMega16u2 qui gere la communication USB de l'arduino est deja bien parametre donc le noyau linux s'est deja approprie les interfaces de la configuration courante. Pour reprendre la main sur le peripherique on utilise la fonction **libusb_detach_kernel_driver** pour chaque interface de la configuration courante. Ce morceau de code ne sera plus necessaire quand la programmation de l'ATMega16u2 sera terminee.

Enfin, on recupere les points d'acces en mode interruption. Pour cela, on s'approprie toutes les interfaces de la configuration en les parcourants a l'aide d'une boucle (de 0 a **bNumInterfaces**). Il faut faire attention ici car l'indice de l'interface dans le tableau de toutes les interfaces ne correcpond pas au numero de l'interface **bInterfaceNumber**. Pour chacune des interfaces reclamees, on parcours la liste des points d'acces et on sauvegarde seulement ceux en mode interruption (**LIBUSB_TRANSFER_TYPE_INTERRUPT**) dans un tableau place en parametre de la fonction config()

### Fermeture du peripherique usb (free_interfaces())
On recpere la configuration active puis on parcour la liste de ses interfaces pour les liberer avec **libusb_release_interface**. Enfin, on ferme la "poignée": **libusb_close(deviceHandle)**.


## Programmation ATMega328p

## Programmation ATMega16u2
Sur le site http://www.fourwalledcubicle.com/LUFA.php téléchargez la dernière version de la bibliothèque USB LUFA. Créez un répertoire PolytechLille au même niveau que les répertoires Demos et Projects. Dans ce répertoire copiez le projet RelayBoard sous le nom, par exemple, PAD. Renommez les fichiers dans le répertoire et modifiez le makefile. Au niveau du makefile indiquez atmega16u2 comme micro-contrôleur, UNO comme platine et passez à 16Mhz. Dans le fichier de configuration du répertoire Config, commentez l’option CONTROL_ONLY_DEVICE.  
 
Vous pouvez ensuite modifier le projet pour obtenir un périphérique USB avec deux interfaces et jusqu’à deux points d’accès de type interruption par interface. Un point d’accès dans le sens hôte vers périphérique et deux autres en sens inverse. Vous pouvez vous aider de la démonstration Keyboard dans le répertoire Device/LowLevel. La déclaration des interfaces et des point d’accès est clairement montrée dans les fichiers Descriptors.c et Descriptors.h de cette démonstration. La gestion de points d’interruption est montrée dans l’autre source C de la démonstration, cherchez les fonctions Endpoint_ConfigureEndpoint, Endpoint_SelectEndpoint, etc. N’hésitez pas à consulter la documentation de la bibliothèque sur Internet.

### Descripteur.h
*On a cree deux interfaces avec USB_Descriptor_interface_t
une pour l'entree avec 2 endpoint
une pour sortie avec 1 endpoint
*Chaque interface est dotee d'un identifiant(ligne 67,68)
*Dans les macros on definie les addresses des endpoints et de taille 1 octet


### Descripteur.c
* On a definie un idVendor et idproduct.\
* On parametre les interfaces et les endpoints declares dans le .h (identifiant, nb d'endpoint par interface, adresse des endpoints)

### PAD.h
* rien a part les prototypes des fonctions

### PAD.c
*(ligne 91) initialise les endpoints
* Fonction Main pour envoyer recevoir.
