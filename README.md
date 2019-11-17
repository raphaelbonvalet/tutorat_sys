# tutorat_sys

##Seance 1 (7/11)
Programme C sur l'ordinateur pour pouvoir utiliser la manette.

**fonction exam:**
examine tous les périphériques USB disponibles sur les bus USB de la machine. Dès qu’un périphérique s’avère être le périphérique USB du type cherché, on sauve la "poignée" vers ce périphérique dans une variable globale de type.
Pour cela, on cree une liste (list) de libusb_device et on utilise la fonction libusb_get_device_list pour la remplir (equivalent a faire lsblk dans le terminal).
On parcour ensuite cette liste en comparant l'idVendor et l'idProduct de chaque device avec ceux correspondant au materiel recherche. La poigne de ce devise est sauvegardee dans la variable globale deviceHandle grace a la fonction libusb_open

**Seance 2 (15/11)**
-fonction config
on recupere la premiere configuration du peripheriques grace a la fonction ibusb_get_config_descriptor que l' on sauvegarde dans un variable congig de type libusb_config_descriptor.
Expliquer le truc avec le noyau linux (a enlever qd config ATMega16u2 ok).
On installe cette configuration comme configuration courante: libusb_set_configuration(deviceHandle,config->bConfigurationValue)
