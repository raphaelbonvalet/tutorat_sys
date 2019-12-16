#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>

#define vendorID 0x2a03
#define productID 0x0043

#define nb_endpoint_max 10

libusb_device_handle * deviceHandle;

void exam(libusb_context* context){ //4.1.Enumeration des peripheriques
    libusb_device **list; //liste des devices connectes
    ssize_t count=libusb_get_device_list(context,&list); //recuperation de la liste
    if(count<0) {perror("libusb_get_device_list"); exit(-1);}
    ssize_t i=0;
    int found=0;
    for(i=0;i<count;i++){
        libusb_device *device=list[i];
        struct libusb_device_descriptor desc;
        int status=libusb_get_device_descriptor(device,&desc);
        if(status!=0) continue;
        if(desc.idVendor==vendorID && desc.idProduct==productID){ //test si le device corrrespond a celui cherche
            libusb_open(list[i], &deviceHandle); //sauvegarde la poignee
            found=1;
            printf("device found\n");
            break;
        }
    }
    if (!found) {perror("Device Not Found"); exit(-1);}
    libusb_free_device_list(list,1);
}


//4.2.configuration du périphérique USB
void config(uint8_t endpoints[nb_endpoint_max]){
    //Recuperer la configuration du device
    struct libusb_config_descriptor * config;
    if(libusb_get_config_descriptor(libusb_get_device(deviceHandle),0,&config)){
        perror("Device descriptor NOK");
        exit(-1);
    }
    printf("config value = %d\n",config->bConfigurationValue);

    //On reprend la main sur le peripherique car le noyau linux est mechant!
    //a enlever qd config ATMega16u2 ok
    for(int i=0;i<config->bNumInterfaces;i++){
        int iface=config->interface->altsetting[i].bInterfaceNumber;
        if(libusb_kernel_driver_active(deviceHandle,iface)){
            int status=libusb_detach_kernel_driver(deviceHandle,iface);
            if(status!=0){ perror("libusb_detach_kernel_driver"); exit(-1);}
        }
    }

    //Installation de la configuration sur le peripherique
    int status = libusb_set_configuration(deviceHandle,config->bConfigurationValue);
    if(status!=0){perror("libusb_set_configuration"); exit(-1);}
    printf("set config OK\n");

    //appropriation des interfaces
    int cpt=0;
    int pt_acces;
    int address_pt_acces;
    for(int i=0;i<config->bNumInterfaces;i++){ //parcour toutes les interfaces
        int iface=config->interface[i].altsetting[0].bInterfaceNumber; //recupere le numero de l'interface
        printf("indice = %d, interface = %d\n",i,iface);
        int status=libusb_claim_interface(deviceHandle,iface);
        if(status!=0){ perror("libusb_claim_interface error"); exit(-1); }

        //recupere les endpoints en mode interruption
        for(int j=0;j<config->interface[i].altsetting[0].bNumEndpoints;j++){ //parcour tout les endpoints
          pt_acces = config->interface[i].altsetting[0].endpoint[j].bmAttributes;
          address_pt_acces = config->interface[i].altsetting[0].endpoint[j].bEndpointAddress;
          printf("Endpoint found : %d \n",address_pt_acces);
          if((pt_acces & 0b00000011) == LIBUSB_TRANSFER_TYPE_INTERRUPT){ //On prend le premier point d'acces en mode interruption
            endpoints[cpt]=address_pt_acces; //sauvegarde du endpoint
    				cpt++;
            printf("Endpoint %d saved\n",address_pt_acces);
    			}
    		}
    }
}

void free_interfaces(){
    struct libusb_config_descriptor *config;
    libusb_device *device = libusb_get_device(deviceHandle);

    // récupération de la config active
    int status = libusb_get_active_config_descriptor(device, &config);
    if(status!=0){ perror("libusb_get_config_descriptor"); exit(-1);}

    // relache des interfaces
    for(int i = 0; i<config->bNumInterfaces; i++){
        status=libusb_release_interface(deviceHandle,config->interface[i].altsetting[0].bInterfaceNumber);
        if(status!=0){ perror("libusb_release_interface"); exit(-1);}
    }
    libusb_close(deviceHandle);
}

int main(){
	//initialisation de la librairie
  libusb_context *context;
  if(libusb_init(&context)){perror("libusb_init error"); exit(-1);}

	exam(context);

  uint8_t endpoints[nb_endpoint_max];
  config(endpoints);

  free_interfaces();

	libusb_exit(context);
	return 0;
}
