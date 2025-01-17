#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>

const int buttonPin = 2;
int lastButtonState = LOW;
bool isButtonPressed = false;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 10, 177);

const char* server = "nfc02.id"; // Server address tanpa HTTPS
const char* urlPressed = "/alarmsystems/alarm.php?entity=sender&location=bg&area=oma&status=true";
const char* urlReleased = "/alarmsystems/alarm.php?entity=sender&location=bg&area=oma&status=false";

// Menggunakan EthernetClient, bukan EthernetSSLClient
EthernetClient client;
HttpClient httpClient(client, server, 80); // Port 80 untuk HTTP

void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);

  // Initialize Ethernet
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }

  delay(1000);
  Serial.println("Ethernet initialized");
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH && !isButtonPressed) {
    isButtonPressed = true;
    sendGetRequest(urlPressed);
  } else if (buttonState == LOW && isButtonPressed) {
    isButtonPressed = false;
    sendGetRequest(urlReleased);
  }

  delay(100);
}

void sendGetRequest(const char* url) {
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // Make GET request
  httpClient.get(url);

  // Get status and response
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  // Stop HTTP client
  httpClient.stop();
}
