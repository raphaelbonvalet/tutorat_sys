# Tutorat Systeme - Bonvalet Raphael - Dubois Vincent

## Programmation PC avec libusb-1.0

* Trouver le bon peripherique (exam()):
Cette fonction examine tous les périphériques USB disponibles sur les bus USB de la machine. Dès qu’un périphérique s’avère être le périphérique USB du type cherché, on sauve la "poignée" vers ce périphérique dans une variable globale.
Pour cela, on cree une liste (list) de **libusb_device** et on utilise la fonction **libusb_get_device_list** pour la remplir (equivalent a faire lsblk dans un terminal). On parcour ensuite cette liste en comparant l'**idVendor** et l'**idProduct** de chaque device avec ceux correspondant au materiel recherche. La poignee de ce device est sauvegardee dans la variable globale **deviceHandle** grace a la fonction **libusb_open**.

* Recuperer les onfigurations/interfaces/points d'acces du peripherique (config())
On recupere dans un premier temps la premiere configuration du peripheriques grace a la fonction **libusb_get_config_descriptor** que l' on sauvegarde dans une variable de type **libusb_config_descriptor**. Cette configuration est ensuite installee comme configuration courante avec la fonction **libusb_set_configuration**

De base, l'ATMega16u2 qui gere la communication USB de l'arduino est deja bien parametre donc le noyau linux s'est deja approprie les interfaces de la configuration courante. Pour reprendre la main sur le peripherique on utilise la fonction **libusb_detach_kernel_driver** pour chaque interface de la configuration courante. Ce morceau de code ne sera plus necessaire quand la programmation de l'ATMega16u2 sera terminee.

Enfin, on recupere les points d'acces en mode interruption. Pour cela, on s'approprie toutes les interfaces de la configuration en les parcourants a l'aide d'une boucle (de 0 a **bNumInterfaces**). Il faut faire attention ici car l'indice de l'interface dans le tableau de toutes les interfaces ne correcpond pas au numero de l'interface **bInterfaceNumber**. Pour chacune des interfaces reclamees, on parcours la liste des points d'acces et on sauvegarde seulement ceux en mode interruption (**LIBUSB_TRANSFER_TYPE_INTERRUPT**) dans un tableau place en parametre de la fonction config()

* Fermeture du peripherique usb (free_interfaces())
On recpere la configuration active puis on parcour la liste de ses interfaces pour les liberer avec **libusb_release_interface**. Enfin, on ferme la "poignée": **libusb_close(deviceHandle)**.


## Programmation ATMega328p

## Programmation ATMega16u2
