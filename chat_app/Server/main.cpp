#include <iostream>
#include <enet/enet.h>


int main(){
     if(enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet!\n");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);

    ENetHost* server;
    ENetEvent event;
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 7777;
    server = enet_host_create(&address, 32, 1, 0, 0);

    if(server == NULL){
        fprintf(stderr, "An Errror occured while trying to create an ENetServer");
        return EXIT_FAILURE;
    }

    //Game Loop Start
    while(true){
        while(enet_host_service(server, &event, 1000) > 0){
            switch(event.type){
                case ENET_EVENT_TYPE_CONNECT:
                    printf("A new Client Connected from %x:%u\n",
                    event.peer -> address.host,
                    event.peer -> address.port);
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    printf("you got new packet %u with contain data %s in channel %s", event.packet -> dataLength, event.packet -> data, event.channelID);
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("%x:%u disconnected\n", event.peer -> address.host, event.peer -> address.port);
                    break;
                
            }
        }
    }
    enet_host_destroy(server);
    return EXIT_SUCCESS;
}