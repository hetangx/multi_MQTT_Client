#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <process.h>
#include "mqttfunc.h"
#include "dataproc.h"

//int slim1(MQTTAsync* client, MQTTAsync_connectOptions* conn_opts, MQTTAsync_disconnectOptions* disc_opts, int* rc)
//{
//
//    if ((*rc = MQTTAsync_create(client, ADDRESS, CLIENTID,
//        MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTASYNC_SUCCESS)
//    {
//        printf("Failed to create client, return code %d\n", *rc);
//
//        exit(EXIT_FAILURE);
//    }
//
//    if ((*rc = MQTTAsync_setCallbacks(*client, *client, connlost, msgarrvd, NULL)) != MQTTASYNC_SUCCESS)
//    {
//        printf("Failed to set callbacks, return code %d\n", *rc);
//        MQTTClient_destroy(client);
//        exit(EXIT_FAILURE);
//    }
//
//    conn_opts->keepAliveInterval = 60;
//    conn_opts->cleansession = 1;
//    conn_opts->username = USERNAME;
//    conn_opts->password = PASSWORD;
//    conn_opts->onSuccess = onConnect;
//    conn_opts->onFailure = onConnectFailure;
//    conn_opts->context = *client;
//    conn_opts->automaticReconnect = true;
//    conn_opts->minRetryInterval = 2;
//    conn_opts->maxRetryInterval = 6;
//
//
//    disc_opts->onSuccess = onDisconnect;
//    disc_opts->onFailure = onDisconnectFailure;
//
//    if ((*rc = MQTTAsync_connect(*client, conn_opts)) != MQTTASYNC_SUCCESS)
//    {
//        printf("Failed to connect, return code %d\n", *rc);
//        MQTTClient_destroy(client);
//        exit(EXIT_FAILURE);
//    }
//
//
//    while (!subscribed && !finished)
//        Sleep(100);
//
//    if (finished)
//        return *rc;
//}

int main()
{
    MQTTAsync client;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
    int rc;

    //slim1(&client,&conn_opts, &disc_opts, &rc);
    if ((rc = MQTTAsync_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);

        exit(EXIT_FAILURE);
    }

    if ((rc = MQTTAsync_setCallbacks(client, client, connlost, msgarrvd, NULL)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to set callbacks, return code %d\n", rc);
        MQTTClient_destroy(client);
        exit(EXIT_FAILURE);
    }

    conn_opts.keepAliveInterval = 60;
    conn_opts.cleansession = 1;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    conn_opts.automaticReconnect = true;
    conn_opts.minRetryInterval = 2;
    conn_opts.maxRetryInterval = 6;


    disc_opts.onSuccess = onDisconnect;
    disc_opts.onFailure = onDisconnectFailure;

    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        MQTTClient_destroy(client);
        exit(EXIT_FAILURE);
    }


    while (!subscribed && !finished)
        Sleep(100);

    if (finished)
        return rc;

    printf("Enter Q to Quit this program...\n");
    int ch;
    do
    {
        ch = getchar();
    } while (ch != 'Q' && ch != 'q');


    if ((rc = MQTTAsync_disconnect(client, &disc_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start disconnect, return code %d\n", rc);
        MQTTAsync_destroy(&client);
        exit(EXIT_FAILURE);
    }

    while (!disc_finished)
    {
        Sleep(100);
    }

    return rc;
}