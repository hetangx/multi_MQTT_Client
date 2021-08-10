#include "mqttfunc.h"
#include "dataproc.h"

int disc_finished = 0;
int subscribed = 0;
int finished = 0;


int msgarrvd(void* context, char* topicName, int topicLen, MQTTClient_message* message)
{
    /*  重新设计msg arrvd
    *   1. 声明pubmsg指针, 申请空间承载payload
*                 -> char* pubmsg = (char*)malloc(JSONLENGTH);
            if(pubmsg)
            {
                do something...
            }
            else
            {
                printf("malloc pubmsg memory error, pls restart program...\n");
            }
        2. 得到topicName中的fig, 不同fig对应不同逻辑


            Do something:     
            1.1 fig = 1
            计算三项不平衡
            CalcUnbalance(message->payload, &pubmsg);
            1.2 fig = 2 ~ 7
            CalcWaves(fig, message->payload, &pubmsg);
                -> char waves[6] = {"cwfa", "cwfb", ... ,"vwfc"};
                    parse关键字为waves[atoi(fig)]
            1.3 fig = 8
            待做
        3. 发送至return topic
           char* returnTopic = (char*)malloc(strlen(maintopic) + 5);
           if (returnTopic)
           {
            do something...
           }
           else
           {
            printf("malloc returnTopic memory error, pls restart program...\n");
           }

           Do something:
           PUB(returnTopic, pubmsg);
            -> 复制粘贴
        3. 释放内存
            MQTTAsync_freeMessage(&message);
            MQTTAsync_free(topicName);
            free(pubmsg);
            free(returnTopic);
    */
    printf("%s\t%d\n", topicName, topicLen);
    Allocator* A = NewAllocator();
    Value* v = NewValue(A);
    CutoffBuffer1((char*)message->payload);
    bool ret = Parse(v, (char*)message->payload);
    if (ret != true)
    {
        printf("Parse Error\n");
        return -1;
    }

    Value* waveArray = ObjGet(v, PARSECONTENT);
    if (waveArray == 0)
    {
        printf("parse error\n");
        return 0;
    }

    //FFT func
    double PI = atan(1) * 4;
    for (int i = 0; i < WAVELENGTH; i++)
    {
        double temp = ParseWaveHelper(waveArray, i);
        x[i].real = temp;
    }

    for (int j = 0; j < 64; j++)
    {
        if (j % 5 == 0)
        {
            continue;
        }
        x[j].real = ((x[j + 1].real - x[j].real) / 256) * (56 * ((double)j + 1));
        x[j].img = 0;
    }

    //初始化变换核

    for (int i = 0; i < WAVELENGTH; i++)
    {
        W[i].real = cos(2 * PI / WAVELENGTH * i);   //用欧拉公式计算旋转因子
        W[i].img = -1 * sin(2 * PI / WAVELENGTH * i);
    }

    fft();//调用快速傅里叶变换
    result[32] = output();//调用输出傅里叶变换结果函数

    char* osid = (char*)parse1Helper_str(v, "substationid");
    char* otid = (char*)parse1Helper_str(v, "transformerid");
    char* otm = (char*)parse1Helper_str(v, "time");
    char* orid = (char*)parse1Helper_str(v, "ref_id");



    char front[JSONWAVELENGTH];
    makeJsonWaveFront(front, JSONWAVELENGTH, osid, otid, otm, orid);

    char* cptr = (char*)malloc(sizeof(char*) * JSONWAVELENGTH);
    int cptrlen = 0;
    if (cptr)
    {
        memset(cptr, 0, JSONWAVELENGTH);
        strcpy_s(cptr, JSONWAVELENGTH, front);
        makeJsonWaveBack(result, WAVERESULT, &cptr);
        cptrlen = (int)strlen(cptr);
    }

    //PUBLISH

    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;


    printf("Message received: \n%s\n", (char*)message->payload);
    printf("Payload to publish: \n%s\n", cptr);

    //printf("Successful connection\n");
    opts.onSuccess = onSend;
    opts.onFailure = onSendFailure;
    opts.context = client;
    pubmsg.payload = cptr;
    pubmsg.payloadlen = cptrlen;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    int rc;
    if ((rc = MQTTAsync_sendMessage(client, PUBTOPIC, &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start sendMessage, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    //FREE MEMORY
    ReleaseAllocator(A);
    memset(result, 0, 33);
    memset(x, 0, sizeof(complex) * WAVELENGTH);
    memset(W, 0, sizeof(complex) * WAVELENGTH);
    free(cptr);

    return 1;
}


void onConnect(void* context, MQTTAsync_successData* response)
{
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    int rc;

    printf("Successful connection\n");

    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
        "Press Q<Enter> to quit\n\n", SUBTOPIC, CLIENTID, QOS);
    opts.onSuccess = onSubscribe;
    opts.onFailure = onSubscribeFailure;
    opts.context = client;
    if ((rc = MQTTAsync_subscribe(client, SUBTOPIC, QOS, &opts)) != MQTTASYNC_SUCCESS)
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