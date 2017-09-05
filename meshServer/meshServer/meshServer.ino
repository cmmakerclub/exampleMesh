#include <Arduino.h>
#include <SoftwareSerial.h>

extern "C" {
#include "user_interface.h"
}

//************************************************************
// this is a simple example that uses the painlessMesh library to
// setup a single node (this node) as a logging node
// The logClient example shows how to configure the other nodes
// to log to this server
//************************************************************
#include "painlessMesh.h"

#define   MESH_PREFIX     "natnatnat"
#define   MESH_PASSWORD   "tantantan"
#define   MESH_PORT       5555

#define rxPin 14
#define txPin 12
SoftwareSerial swSerial(rxPin, txPin);

painlessMesh  mesh;
bool blinkState= true;

// Send my ID every 10 seconds to inform others
Task logServerTask(5000, TASK_FOREVER, []() {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& msg = jsonBuffer.createObject();
});

void serialWrite(uint8_t b) {
  Serial.write(b);
  swSerial.write(b);
}

void serialWrite(const char *b, size_t len) {
  Serial.write(b, len);
  swSerial.write(b, len);
}

void setup() {
  wifi_status_led_install(2,  PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  swSerial.begin(38400);
  Serial.begin(57600);
  Serial.flush();
  Serial.println();

  serialWrite(0xff);
  serialWrite(0xfa);
  serialWrite(0x01);
  serialWrite(0x00);
  serialWrite(0x0d);
  serialWrite(0x0a);
  // Serial.println("HELLO");

  // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE | DEBUG ); // all types on
  //mesh.setDebugMsgTypes( ERROR | CONNECTION | SYNC | S_TIME );  // set before init() so that you can see startup messages
  // mesh.setDebugMsgTypes( ERROR | CONNECTION | S_TIME );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, STA_AP, AUTH_WPA2_PSK, 9);
  mesh.onReceive(&receivedCallback);

  mesh.onNewConnection([](size_t nodeId) {
    String subConnectionJson = mesh.subConnectionJson();
    // const char* meshSu
    // serialWrite(0xff);
    // serialWrite(0xfa);
    // serialWrite(0x02);
    // serialWrite(subConnectionJson.length());
    // serialWrite(subConnectionJson.c_str(), subConnectionJson.length());
    // serialWrite(0x0d);
    // serialWrite(0x0a);
  });

  mesh.onDroppedConnection([](size_t nodeId) {
    // Serial.printf("Dropped Connection nodeID: %u\n", nodeId);
  });

  // Add the task to the mesh scheduler
  mesh.scheduler.addTask(logServerTask);
  logServerTask.enable();
}

void loop() {
  mesh.update();
}

void receivedCallback( uint32_t from, String &msg ) {
  const char* msg_cstr = msg.c_str();
  serialWrite(0xff);
  serialWrite(0xfa);
  serialWrite(0x03);
  serialWrite(msg.length());
  serialWrite(msg_cstr, strlen(msg_cstr));
  serialWrite(0x0d);
  serialWrite(0x0a);
}
