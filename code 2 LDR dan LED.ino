#include "painlessMesh.h"
#include <ArduinoJson.h>

#define MESH_PREFIX     "MeshIoTNetwork"
#define MESH_PASSWORD   "password123"
#define MESH_PORT       5555

#define LDR_PIN         A0      
#define LED_PIN         12      

Scheduler userScheduler;
painlessMesh mesh;

float latestSuhu = 0.0;
int latestAdc = 512;

void sendMessage();
void evaluateRuleEngine();
Task taskSendMessage(TASK_SECOND * 3, TASK_FOREVER, &sendMessage);

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Perbaikan di sini:
  mesh.setDebugMsgTypes(ERROR | STARTUP);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

void loop() {
  mesh.update();
}

void sendMessage() {
  int nilaiAdc = analogRead(LDR_PIN);
  latestAdc = nilaiAdc;

  JsonDocument doc;
  doc["tipe"] = "cahaya_node";
  doc["adc"] = nilaiAdc;

  String msgString;
  serializeJson(doc, msgString);

  mesh.sendBroadcast(msgString);
  Serial.println("Kirim Data LDR: " + msgString);
  
  evaluateRuleEngine();
}

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Pesan diterima dari %u: %s\n", from, msg.c_str());

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, msg);

  if (error) {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  const char* tipe = doc["tipe"];
  if (strcmp(tipe, "suhu_node") == 0) {
    latestSuhu = doc["suhu"];
    evaluateRuleEngine();
  }
}

void evaluateRuleEngine() {
  Serial.printf("Evaluasi Rule -> Suhu: %.1f C, ADC Cahaya: %d\n", latestSuhu, latestAdc);

  if (latestSuhu > 31.0 || latestAdc < 300) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("STATUS AKTUATOR: LED NYALAKAN (Kondisi Kritis/Gelap)");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("STATUS AKTUATOR: LED MATI (Kondisi Normal)");
  }
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("Node baru terhubung: %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.println("Topologi jaringan mesh berubah!");
}