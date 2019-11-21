#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>

//lsusb
//gcc usb.c -o usb -l usb-1.0
//locate libusb-1.0.so

#define Idvendor 0x1234
#define Idproduct 0x4321

libusb_context *context;
libusb_device *our_device;

void enumeration_device(libusb_device_handle **handle)
{
	libusb_device **list;  //Structure du device USB détecté

	ssize_t count=libusb_get_device_list(context,&list);       //Nombre d'appareils connectés en USB
	if(count<0) {perror("libusb_get_device_list"); exit(-1);}
	int found=0;

	for(ssize_t i=0;i<count;i++){
		libusb_device *device=list[i]; //On prend un appareil connecté

		struct libusb_device_descriptor struct_desc_device;
        int status=libusb_get_device_descriptor(device,&struct_desc_device);    //Récupère les infos de l'USB et renvoie 0 en cas de succès

	    if(status!=0) continue; //on test l'USB suivant
		if((Idvendor == struct_desc_device.idVendor) && (Idproduct == struct_desc_device.idProduct))
		{
			uint8_t bus=libusb_get_bus_number(device);
			uint8_t address=libusb_get_device_address(device);
			printf("Device Found @ (Bus:Address) %03d:%03d\n",bus,address);
			printf("Vendor ID %x\n",struct_desc_device.idVendor);
			printf("Product ID %x\n",struct_desc_device.idProduct);
			status=libusb_open(device,handle);
            our_device = device;
			if(status!=0){perror("libusb_open error"); exit(-1);}
			//printf("Vendor : %x Product : %x \n",struct_desc_device.idVendor,struct_desc_device.idProduct);
			found = 1;
			break;
		}
	}
	if (!found)
    {
        perror("Device Not Found"); exit(-1);
    }
	libusb_free_device_list(list,1);
}



void configuration_device(libusb_device_handle *handle,uint8_t tab_endpoint[12])
{
	struct libusb_config_descriptor* config_descri;
	libusb_get_config_descriptor(libusb_get_device(handle),0,&config_descri);      //On récup dans config_descri la config d'indice 0
	printf("Valeur ID config=%hhu\n",config_descri->bConfigurationValue);   //Affichage de l'ID de la config

	ssize_t i=0;
	for(i=0;i<config_descri->bNumInterfaces;i++)
	{
		if(libusb_kernel_driver_active(handle,config_descri->interface[i].altsetting[0].bInterfaceNumber))
            //On reprend la main sur le périphérique
		{     //Voir "Si le méchant noyau est passé avant vous"
  			int status=libusb_detach_kernel_driver(handle,config_descri->interface[i].altsetting[0].bInterfaceNumber);
  			if(status!=0){ perror("libusb_detach_kernel_driver"); exit(-1); }
  		}
	}

	//Voir "Utilisation d'une configuration du périph"
	int configuration=config_descri->bConfigurationValue;  //On récupère l'id de la config
	int status=libusb_set_configuration(handle,configuration); //On installe la config sur le pérophérique en config courante
	if(status!=0){perror("libusb_set_configuration");exit(-1);}



	ssize_t cpt=0;
    int pt_acces;
    int address_pt_acces;
	for(i=0;i<config_descri->bNumInterfaces;i++) //on parcours toutes les interfaces
	{
		int interface=config_descri->interface[i].altsetting[0].bInterfaceNumber; //on récupère le numéro d'interface
		int status=libusb_claim_interface(handle,interface); //on récup une interface
		if(status!=0){perror("libusb_claim_interface");exit(-1);} //on verifie que l'interface est bien recup
		printf("Interface d'indice %ld et de numero %d detectee et reclamee\n",i,interface);

		ssize_t j=0;
		for(j=0;j<config_descri->interface[i].altsetting[0].bNumEndpoints;j++) //parcourt les sous-structure d'interface
		{
			pt_acces = config_descri->interface[i].altsetting[0].endpoint[j].bmAttributes;
            address_pt_acces = config_descri->interface[i].altsetting[0].endpoint[j].bEndpointAddress;
            printf("Point d'accès %d trouvé\n",address_pt_acces);
            if((pt_acces & 0b00000011) == LIBUSB_TRANSFER_TYPE_INTERRUPT) //On prend le premier point d'acces de type interruption
			{
				tab_endpoint[cpt]=address_pt_acces; //STOCKAGE DANS un tableau en mémoire
				cpt++;
                printf("Point d'accès %d sauvegarde\n",address_pt_acces);
			}
		}
	}

}


void ferme_peripherique(libusb_device_handle **handle)
{
    struct libusb_config_descriptor* config_descri;
    libusb_get_active_config_descriptor(our_device,&config_descri);      //On récup dans config_descri la config d'indice 0


    for(int i=0;i<config_descri->bNumInterfaces;i++) //on parcours toutes les interfaces
	{
		int interface=config_descri->interface[i].altsetting[0].bInterfaceNumber; //on récupère le numéro d'interface
		int status=libusb_release_interface(*handle,interface); //on libere une interface
		if(status!=0){perror("libusb_release_interface");exit(-1);} //on verifie que l'interface est bien liberee
        printf("Interface d'indice %d et de numero %d liberee\n",i,interface);
	}


    libusb_free_config_descriptor(config_descri);
    libusb_close(*handle);
    libusb_exit(context);
}



void Send(libusb_device_handle* handle, char c, int endpoint_out)
{
    unsigned char data[8]={c};            /* data to send or to receive */
    int size=sizeof(data);           /* size to send or maximum size to receive */
    int timeout=0;        /* timeout in ms */

    /* OUT interrupt, from host to device */
    int bytes_out;
    int status=libusb_interrupt_transfer(handle,endpoint_out,data,size,&bytes_out,timeout);
    if(status!=0){ perror("libusb_interrupt_transfer_S"); exit(-1); }
}



//Recieve ne fonctionne pas
void Recieve(libusb_device_handle* handle, int endpoint_in)
{
    unsigned char data[8];           /* data to send or to receive */
    int size=sizeof(data);;           /* size to send or maximum size to receive */
    int timeout=500;        /* timeout in ms */

    /* IN interrupt, host polling device */
    int bytes_in;
    int status=libusb_interrupt_transfer(handle,endpoint_in,data,size,&bytes_in,timeout);
    if(status!=0){ perror("libusb_interrupt_transfer_R"); exit(-1); }

    printf("%c\n",data[0]);

    //return data[0];
}



void Traitement(libusb_device_handle* handle,uint8_t tab_endpoint[12])
{
    unsigned char joystick;
    unsigned char buttons;
    char carac;

    while (carac != 's')
    {
        carac = getchar();
        Send(handle,carac, tab_endpoint[2]);
        //Recieve(handle, tab_endpoint[1]);
    }


}

int main()
{
	libusb_device_handle *handle=NULL;


	//Initialisation
	int status=libusb_init(&context);      //initialise libusb, retourne 0 en cas de succès
	if(status!=0) {perror("libusb_init"); exit(-1);}

	//Enumération des devices USB
	enumeration_device(&handle);


	//Configuration USB
	uint8_t tab_endpoint[12];
	configuration_device(handle,tab_endpoint);


	//Traitement des données
	Traitement(handle,tab_endpoint);

    	//Libérer les Endpoints
    	ferme_peripherique(&handle);


	return 0;
}
