#include <Arduino.h>
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

painlessMesh  mesh;
bool blinkState= true;

// Send my ID every 10 seconds to inform others
Task logServerTask(5000, TASK_FOREVER, []() {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& msg = jsonBuffer.createObject();
});

void setup() {
  wifi_status_led_install(2,  PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);

  Serial.begin(57600);
  Serial.flush();
  Serial.println();

  uint8_t headerByte[2] = {0xff, 0xfa};
  Serial.write(headerByte, 2);
  Serial.write(0x01);
  Serial.write(0x00);
  Serial.write(0x0d);
  Serial.write(0x0a);
  // Serial.println("HELLO");

  // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE | DEBUG ); // all types on
  //mesh.setDebugMsgTypes( ERROR | CONNECTION | SYNC | S_TIME );  // set before init() so that you can see startup messages
  // mesh.setDebugMsgTypes( ERROR | CONNECTION | S_TIME );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, STA_AP, AUTH_WPA2_PSK, 9);
  mesh.onReceive(&receivedCallback);

  mesh.onNewConnection([](size_t nodeId) {
    String subConnectionJson = mesh.subConnectionJson();
    // const char* meshSu
    Serial.write(0xff);
    Serial.write(0xfa);
    Serial.write(0x02);
    Serial.write(subConnectionJson.length());
    Serial.write(subConnectionJson.c_str(), subConnectionJson.length());
    Serial.write(0x0d);
    Serial.write(0x0a);
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
  Serial.write(0xff);
  Serial.write(0xfa);
  Serial.write(0x03);
  Serial.write(msg.length());
  Serial.write(msg_cstr, strlen(msg_cstr));
  Serial.write(0x0d);
  Serial.write(0x0a);
}
