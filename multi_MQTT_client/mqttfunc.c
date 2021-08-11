#include "mqttfunc.h"
#include "dataproc.h"

int disc_finished = 0;
int subscribed = 0;
int finished = 0;

int msgarrvd(void* context, char* topicName, int topicLen, MQTTAsync_message* message)
{
    if (message->payloadlen < 0)
    {
        printf("Message empty, please contact the administrator.\nLast Message didn't send because of some bugs.\n");
        return -1;
    }
    // 1. ÉùÃ÷pubmsgÖ¸Õë, ÉêÇë¿Õ¼ä³ÐÔØpayload
    char* pubmsgpayload = (char*)malloc(JSONLENGTH);
    if (pubmsgpayload == NULL)
    {
        printf("malloc pubmsg memory error, pls restart program...\n");
        return -1;
    }
    MessageSwitch(topicName, message->payload, &pubmsgpayload);


    //PUBLISH
    
    char* returnTopic = (char*)malloc((size_t)topicLen + 5);
    if (returnTopic == NULL)
    {
        printf("malloc returnTopic memory error, pls restart program...\n");
        return -1;
    }

    CatPubtopic(&returnTopic, topicName);

    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;


    printf("Message received: \n%s\n", (char*)message->payload);
    printf("Payload to publish: \n%s\n", pubmsgpayload);

    opts.onSuccess = onSend;
    opts.onFailure = onSendFailure;
    opts.context = client;
    pubmsg.payload = pubmsgpayload;
    pubmsg.payloadlen = (int)strlen(pubmsgpayload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    int rc;
    if ((rc = MQTTAsync_sendMessage(client, returnTopic, &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start sendMessage, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    //FREE MEMORY
    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);

    free(pubmsgpayload);
    free(returnTopic);

    return 1;
}

void onConnect(void* context, MQTTAsync_successData* response)
{
    char* hostport = response->alt.connect.serverURI;
    char* topics[8] = {
        "plat/gateway/460110142949436/pkg/1/up",
        "plat/gateway/460110142949436/pkg/2/up",
        "plat/gateway/460110142949436/pkg/3/up",
        "plat/gateway/460110142949436/pkg/4/up",
        "plat/gateway/460110142949436/pkg/5/up",
        "plat/gateway/460110142949436/pkg/6/up",
        "plat/gateway/460110142949436/pkg/7/up",
        "plat/gateway/460110142949436/pkg/8/up",
    };
    int qoss[8] = { 0,0,0,0,0,0,0,0};

    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    int rc;

    printf("Successful connection to %s\n", hostport);
    printf("Subscribing to topic: %s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", topics[0], topics[1], topics[2], topics[3], topics[4], topics[5], topics[6], topics[7]);
    printf("For client %s using QoS%d\n\n"
        "Press Q or q<Enter> to quit\n\n", CLIENTID, QOS);
    opts.onSuccess = onSubscribe;
    opts.onFailure = onSubscribeFailure;
    opts.context = client;

    // multi topics test 
    if ((rc = MQTTAsync_subscribeMany(client, 8, topics, qoss, &opts) != MQTTASYNC_SUCCESS))
    {
        printf("Failed to start subscribe, return code %d\n", rc);
        finished = 1;
    }    
}

void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
    printf("Connect failed, rc %d\n", response ? response->code : 0);
    finished = 1;
}

void onSubscribe(void* context, MQTTAsync_successData* response)
{
    printf("Subscribe succeeded\n");
    subscribed = 1;
}

void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
    printf("Subscribe failed, rc %d\n", response->code);
    finished = 1;
}

void onDisconnect(void* context, MQTTAsync_successData* response)
{
    printf("Successful disconnection\n");
    disc_finished = 1;
}

void onDisconnectFailure(void* context, MQTTAsync_failureData* response)
{
    printf("Disconnect failed, rc %d\n", response->code);
    disc_finished = 1;
}

void onSend(void* context, MQTTAsync_successData* response)
{


    printf("Message with token value %d delivery confirmed\n", response->token);


}

void onSendFailure(void* context, MQTTAsync_failureData* response)
{
    printf("Message send failed token %d error code %d\n", response->token, response->code);

}


void connlost(void* context, char* cause)
{
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;

    printf("\nConnection lost\n");
    if (cause)
        printf("     cause: %s\n", cause);

    printf("Reconnecting\n");
    conn_opts.keepAliveInterval = 60;
    conn_opts.cleansession = 1;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        finished = 1;
    }
}

void CatPubtopic(char** p, char* maintopic)
{
    int lenp = (int)(strlen(maintopic) + 5);
    char* p1 = strstr(maintopic, "/up");
    if (p1)
    {
        strncpy_s(*p, lenp, maintopic, strlen(maintopic) - strlen(p1));
        strcat_s(*p, lenp, "/retrun");
    }

}