// Definisi PIN
const int buttonPin = 4;  // GPIO 4
const int ledPin = 5;     // GPIO 5

// Variabel Penyimpanan Status
bool ledState = false;     // Simpan kondisi LED 
int lastButtonState = LOW; // Simpan status tombol pada pembacaan sebelumnya

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Memastikan kondisi LED OFF sebelum digunakan
  digitalWrite(ledPin, LOW);
  Serial.println("KONDISI AWAL LED OFF. SILAHKAN GUNAKAN SISTEM");
}

void loop() {
  // Baca kondisi tombol saat ini
  int currentButtonState = digitalRead(buttonPin);
  // Deteksi perubahan status tombol
  if (currentButtonState == HIGH && lastButtonState == LOW) {
    // Debouncing (Delay singkat untuk abaikan getaran mekanis saklar)
    delay(200);
    // Membalikkan status LED
    ledState = !ledState;
    // Terapkan status baru ke LED
    digitalWrite(ledPin, ledState ? HIGH : LOW);

    // Tampilkan status ke Serial Monitor
    if (ledState) {
      Serial.println("Tombol ditekan -> LED ON");
    } else {
      Serial.println("Tombol ditekan -> LED OFF");
    }
  }

  // Simpan kondisi tombol saat ini untuk dibandingkan
  lastButtonState = currentButtonState;
}