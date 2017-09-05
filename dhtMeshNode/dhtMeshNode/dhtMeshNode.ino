#include <Arduino.h>
extern "C" {
#include "user_interface.h"
}
//************************************************************
// this is a simple example that uses the painlessMesh library to
// setup a node that logs to a central logging node
// The logServer example shows how to configure the central logging nodes
//************************************************************
#include "painlessMesh.h"
#include "DHT.h"

#define   MESH_PREFIX     "natnatnat"
#define   MESH_PASSWORD   "tantantan"
#define   MESH_PORT       5555

painlessMesh  mesh;

size_t logServerId = 0;

#define DHTPIN 12     // what digital pin we're connected to

// #define DHTTYPE DHT11
#define DHTTYPE DHT22
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);
#include "user_tasks.hpp"

// Send message to the logServer every 10 seconds
Task myLoggingTask(10000, TASK_FOREVER, []() {
  userTaskReadSensor();
});

void setup() {
  Serial.begin(38400);
  // pinMode(16, OUTPUT);
  wifi_status_led_install(2,  PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
  dht.begin();

  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, STA_AP, AUTH_WPA2_PSK, 9,
    PHY_MODE_11G, 82, 1, 4);
  mesh.onReceive(&receivedCallback);

  // Add the task to the mesh scheduler
  mesh.scheduler.addTask(myLoggingTask);
  myLoggingTask.enable();
}

void loop() {
  mesh.update();
}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("logClient: Received from %u msg=%s\n", from, msg.c_str());

  // Saving logServer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(msg);
  if (root.containsKey("topic")) {
      if (String("logServer").equals(root["topic"].as<String>())) {
          // check for on: true or false
          logServerId = root["nodeId"];
          Serial.printf("logServer detected!!!\n");
      }
      Serial.printf("Handled from %u msg=%s\n", from, msg.c_str());
  }
}
