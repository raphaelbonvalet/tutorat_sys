#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>

#define vendorID 0x2341
#define productID 0x0001

libusb_device_handle * deviceHandle;

void exam(libusb_context* context){ //4.1.Enumeration des peripheriques
    libusb_device **list;
    ssize_t count=libusb_get_device_list(context,&list);
    if(count<0) {perror("libusb_get_device_list"); exit(-1);}
    ssize_t i=0;
    for(i=0;i<count;i++){
        libusb_device *device=list[i];
        struct libusb_device_descriptor desc;
        int status=libusb_get_device_descriptor(device,&desc);
        if(status!=0) continue;
        //uint8_t bus=libusb_get_bus_number(device);
        //uint8_t address=libusb_get_device_address(device);
        //printf("Device Found @ (Bus:Address) %d:%d\n",bus,address);
        //printf("Vendor ID 0x0%x\n",desc.idVendor);
        //printf("Product ID 0x0%x\n",desc.idProduct);

        if(desc.idVendor==vendorID && desc.idProduct==productID){
            libusb_open(list[i], &deviceHandle);
            //printf("ok\n");
        }
    }
    libusb_free_device_list(list,1);
}


//4.2.configuration du périphérique USB
uint8_t * config(){
    //Obtenir la configuration de la poignée
    struct libusb_config_descriptor * config;
    if(!libusb_get_config_descriptor(libusb_get_device(deviceHandle),0,&config)){
        //printf("config ok\n");
    }
    printf("valeur config = %d\n",config->bConfigurationValue);

    //car le noyau linux est mechant! a enlever qd config ATMega16u2 ok
    for(int i=0;i<config->bNumInterfaces;i++){
        int iface=config->interface->altsetting[i].bInterfaceNumber;
        if(libusb_kernel_driver_active(deviceHandle,iface)){
            int status=libusb_detach_kernel_driver(deviceHandle,iface);
            if(status!=0){ perror("libusb_detach_kernel_driver"); exit(-1);}
        }
    }

    //Installer la configuration
    int status = libusb_set_configuration(deviceHandle,config->bConfigurationValue);
    if(status!=0){ perror("libusb_set_configuration"); exit(-1); }

    //appropriation des interfaces
    uint8_t endPoint[config->bNumInterfaces][2]; //tableau de 2 endPoints pour chaque interface

    for(int i=0;i<config->bNumInterfaces;i++){
        int iface=config->interface->altsetting[i].bInterfaceNumber;
        int status=libusb_claim_interface(deviceHandle,iface);
        if(status!=0){ perror("libusb_claim_interface"); exit(-1); }

        //recupere les endpoints en mode interruption
        int k=0;
        for(int j=0;j<config->interface->altsetting[i].bNumEndpoints;j++){
            if((config->interface->altsetting[i].endpoint->bmAttributes & 0x03) == LIBUSB_TRANSFER_TYPE_INTERRUPT){
                endPoint[i][k]=config->interface->altsetting[i].endpoint->bEndpointAddress;
                k++;
            }
            if(k>=2) break;
        }
    }
    return endPoint;
}



int main(){
	//initialisation de la librairie
    libusb_context *context;
    int status=libusb_init(&context);
    if(status!=0) {perror("libusb_init"); exit(-1);}

	exam(context);
    config();


    libusb_close(deviceHandle);
	libusb_exit(context);
	return 0;
}
