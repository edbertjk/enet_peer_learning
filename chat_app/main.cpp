#include <bits/stdc++.h>
#include "chat_screen.cpp"
#include <pthread.h>
#include <enet/enet.h>
using namespace std;

static ChatScreen chatScreen;

void SendPacket(ENetPeer* peer, const char* data){
    ENetPacket* packet = enet_packet_create(data, strlen(data), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void* MsgLoop(ENetHost* client){
    while(true){
        ENetEvent* event;
        while(enet_host_service(client, event, 0) > 0){
            switch(event->type){
                case ENET_EVENT_TYPE_RECEIVE:
                printf("you got new packet %u with contain data %s in channel %s", event->packet -> dataLength, event->packet -> data, event->channelID);
                enet_packet_destroy(event->packet);
                break;
            }
        }
    }
}

int main(){
    printf("Masukkan Username Anda: ");
    char username[80];
    scanf("%s", username);

    if(enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet!\n");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);

    ENetHost* client;
    client = enet_host_create(NULL, 1, 1, 0, 0);

    if(client == NULL)
    {
        fprintf(stderr, "An error occurred while trying to create an ENet client host!\n");
        return EXIT_FAILURE;
    }

    ENetAddress address;
    ENetEvent event;
    ENetPeer* peer;

    enet_address_set_host(&address, "127.0.0.1");
    address.port = 7777;

    peer = enet_host_connect(client, &address, 1, 0);
    if(peer == NULL)
    {
        fprintf(stderr, "No available peers for initiating an ENet connection!\n");
        return EXIT_FAILURE;
    }
    if(enet_host_service(client, &event, 5000) > 0 &&
	 event.type == ENET_EVENT_TYPE_CONNECT)
    {
        puts("Connection to 127.0.0.1:7777 succeeded.");
    }
    else
    {
        enet_peer_reset(peer);
        puts("Connection to 127.0.0.1:7777 failed.");
        return EXIT_SUCCESS;
    }

    SendPacket(peer, "Hello World");

    chatScreen.Init();

    pthread_t thread;
    pthread_create(&thread, NULL, MsgLoop, client);

    while(true){
        string msg = chatScreen.CheckBoxInput();
        chatScreen.PostMessage(username, msg.c_str());
    }

    enet_peer_disconnect(peer, 0);

    while(enet_host_service(client, &event, 3000) > 0)
    {
        switch(event.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                puts("Disconnection succeeded.");
                break;
        }
    }

    return EXIT_SUCCESS;
    return 0;
}