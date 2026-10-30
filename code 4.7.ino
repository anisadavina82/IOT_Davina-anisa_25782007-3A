#include <ESP8266WiFi.h> 
#include <ESPAsyncTCP.h> 
#include <ESPAsyncWebServer.h> 
#include <DHT.h> 
 
const char* ssid = "NAMA_WIFI_ANDA"; const char* password = "PASSWORD_WIFI_ANDA"; 
 

DHT dht(dhtPin, DHT11); 
 

AsyncWebServer server(80); 
AsyncWebSocket ws("/ws"); 
 
const char index_html[] PROGMEM = R"rawliteral( 
<!DOCTYPE html> 
<html> 
<head> 
  <meta name="viewport" content="width=device-width, initial-scale=1"> 
  <title>Real-Time IoT Web</title> 
  <style> 
    body { font-family: Arial; text-align: center; } 
    .card { background: #f0f0f0; margin: 20px auto; padding: 20px; max-width: 300px; border-radius: 10px; }     button { padding: 15px 30px; font-size: 20px; border-radius: 5px; cursor: pointer; color: white;}   
    .btn-on { background-color: #4CAF50; }   
    .btn-off { background-color: #f44336; }   
  </style> 
</head> 
<body> 
  <h1>Smart Room</h1> 
  <div class="card"> 
    <h2>Suhu: <span id="tempValue">--</span> Celcius</h2> 
  </div> 
  <div class="card"> 
    <h2>LED: <span id="ledStatus">OFF</span></h2> 
    <button id="toggleBtn" class="btn-off" onclick="toggleLed()">Turn ON</button>   </div> 
 
  <script> 
    // Membuka terowongan koneksi WebSocket ke alamat IP ESP8266     var gateway = `ws://${window.location.hostname}/ws`;     var websocket; 
 
    // Menjalankan inisiasi saat halaman pertama kali dimuat     window.addEventListener('load', onLoad);     function onLoad(event) { initWebSocket(); } 
 
    function initWebSocket() {       websocket = new WebSocket(gateway);       websocket.onopen    = onOpen;       websocket.onclose   = onClose; 
      websocket.onmessage = onMessage; 
    } 
 
    function onOpen(event) { console.log('WebSocket Terkoneksi'); }     function onClose(event) { setTimeout(initWebSocket, 2000); } 
 
    // Dipanggil saat tombol di layar web ditekan     function toggleLed(){   
      websocket.send('toggle'); 
    } 
 
    // Menangkap paket data JSON yang "didorong" (PUSH) oleh C++ NodeMCU     function onMessage(event) { 
      var dataObj = JSON.parse(event.data);   
         
      // Menyuntikkan teks angka Suhu ke dalam id "tempValue" HTML       if(dataObj.suhu !== undefined) {            document.getElementById('tempValue').innerHTML = dataObj.suhu;   
      }   
         
      // Merombak UI Tombol & Teks sesuai status hardware         if(dataObj.led !== undefined) {            var btn = document.getElementById('toggleBtn');            var status = document.getElementById('ledStatus');   
         if(dataObj.led == "1"){              status.innerHTML = "ON";              btn.innerHTML = "Turn OFF";   
           btn.className = "btn-on";   
         } else {   
           status.innerHTML = "OFF";              btn.innerHTML = "Turn ON";   
           btn.className = "btn-off";   
         }   
      }   
    }   
  </script>   
</body>   
</html>   
)rawliteral"; 
 

  String jsonString = "{\"led\":\"" + String(ledState ? 1 : 0) + "\", "; 
  jsonString += "\"suhu\":\"" + currentTemp + "\"}";   ws.textAll(jsonString); 
} 
 
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {   AwsFrameInfo *info = (AwsFrameInfo*)arg; 
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {     data[len] = 0;     if (strcmp((char*)data, "toggle") == 0) { 
      ledState = !ledState;       notifyClients(); 
    } 
  } 
} 
 
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,              void *arg, uint8_t *data, size_t len) { 
  switch (type) {     case WS_EVT_CONNECT: 
      Serial.printf("Client WebSocket #%u terhubung\n", client->id());       notifyClients(); 
      Serial.printf("Client WebSocket #%u terputus\n", client->id()); 
      break;     case WS_EVT_DATA: 
      handleWebSocketMessage(arg, data, len);       break; 
  } 
} 
 
void setup() { 
  Serial.begin(115200); 
 
 
  pinMode(ledPin, OUTPUT);   digitalWrite(ledPin, LOW); 
  dht.begin(); 
 
  WiFi.mode(WIFI_STA);   WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); } 
  Serial.println("\nIP Address: " + WiFi.localIP().toString()); 
 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ 
    request->send_P(200, "text/html", index_html); 
  }); 
 
 
void loop() { 
  ws.cleanupClients(); 
 
  int reading = digitalRead(buttonPin);   if (reading != lastButtonState) { 
    lastDebounceTime = millis(); 
  } 
  if ((millis() - lastDebounceTime) > debounceDelay) { 
    if (reading != buttonState) { 
      buttonState = reading; 
       
        ledState = !ledState; 
        notifyClients();
      } 
    } 
  } 
  lastButtonState = reading; 
 
    if(!isnan(t)) {       currentTemp = String(t);       notifyClients();
    } 
    lastTime = millis(); 
  } 
} 
