#ifndef MQTTFUNC_H

#include <paho-mqtt/MQTTClient.h>
#include <paho-mqtt/MQTTAsync.h>
#include "zzzjson.h"

#define ADDRESS "tcp://localhost:1883"
#define CLIENTID "Vcwave_3"
#define USERNAME "yishu"
#define PASSWORD "123456"
#define QOS 1
#define TIMEOUT 10000L

#define PARSECONTENT "Vwfc"
#define PUBTOPIC "plat/gateway/460110142949436/pkg/4/return"
#define SUBTOPIC "plat/gateway/460110142949436/pkg/4/up"

extern int disc_finished;
extern int subscribed;
extern int finished;

void connlost(void* context, char* cause);
int msgarrvd(void* context, char* topicName, int topicLen, MQTTClient_message* message);
void onConnect(void* context, MQTTAsync_successData* response);
void onSubscribe(void* context, MQTTAsync_successData* response);
void onSubscribeFailure(void* context, MQTTAsync_failureData* response);
void onConnectFailure(void* context, MQTTAsync_failureData* response);
void onDisconnectFailure(void* context, MQTTAsync_failureData* response);
void onDisconnect(void* context, MQTTAsync_successData* response);
void onSend(void* context, MQTTAsync_successData* response);
void onSendFailure(void* context, MQTTAsync_failureData* response);

#endif // !MQTTFUNC_H