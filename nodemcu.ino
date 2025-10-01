#include <ESP8266WiFi.h>

// Replace these with your network credentials
const char* ssid = "Nithin";
const char* password = "12345678";

WiFiServer server(80);

// Motor pin definitions
#define EnaLeft 5
#define ml1 4
#define ml2 0
#define EnaRight 14
#define mr1 12
#define mr2 13

// Ultrasonic sensor pin definitions
#define TRIG_PIN 15
#define ECHO_PIN 2

unsigned long commandTimestamp = 0;  // To track command timing
bool motorActive = false;           // To manage motor state

void setup() {
  Serial.begin(9600);
  delay(10);

  // Set up motor pins
  pinMode(EnaLeft, OUTPUT);
  pinMode(ml1, OUTPUT);
  pinMode(ml2, OUTPUT);
  pinMode(EnaRight, OUTPUT);
  pinMode(mr1, OUTPUT);
  pinMode(mr2, OUTPUT);

  analogWriteRange(255);  // Set PWM range

  // Stop motors initially
  stopMotors();

  // Set up ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.println("<html><body>");
            client.println("<h1>ESP8266 Motor Control</h1>");
            client.println("<a href='/LED=0'>Forward</a><br>");
            client.println("<a href='/LED=1'>Left</a><br>");
            client.println("<a href='/LED=2'>Right</a><br>");
            client.println("<a href='/LED=4'>Reverse</a><br>");
            client.println("<a href='/LED=3'>Stop</a><br>");
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;

          if (currentLine.endsWith("GET /LED=0")) {
            if (measureDistance() > 20) {
              executeCommand("forward");
            } else {
              Serial.println("Cannot move forward, object too far!");
            }
          } else if (currentLine.endsWith("GET /LED=1")) {
            if (measureDistance() > 20) {
              executeCommand("left");
            } else {
              Serial.println("Cannot turn left, object too far!");
            }
          } else if (currentLine.endsWith("GET /LED=2")) {
            if (measureDistance() > 20) {
              executeCommand("right");
            } else {
              Serial.println("Cannot turn right, object too far!");
            }
          } else if (currentLine.endsWith("GET /LED=4")) {
            if (measureDistance() > 20) {
              executeCommand("reverse");
            } else {
              Serial.println("Cannot move reverse, object too far!");
            }
          } else if (currentLine.endsWith("GET /LED=3")) {
            executeCommand("stop");
          }
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected.");
  }

  // Manage motor delay
  if (motorActive && millis() - commandTimestamp > 1000) {
    stopMotors();
    motorActive = false;
  }
}

void executeCommand(String command) {
  Serial.println("Executing Command: " + command);
  if (command == "forward") {
    forward();
    delay(200);  // Small delay for forward movement
    stopMotors();
  } else if (command == "left") {
    turnLeft();
    delay(200);  // Small delay for left turn
    stopMotors();
  } else if (command == "right") {
    turnRight();
    delay(200);  // Small delay for right turn
    stopMotors();
  } else if (command == "reverse") {
    reverse();
    delay(200);  // Small delay for reverse movement
    stopMotors();
  } else if (command == "stop") {
    stopMotors();
  }
  commandTimestamp = millis();
  motorActive = true;
}

void forward() {
  digitalWrite(ml1, HIGH);
  digitalWrite(ml2, LOW);
  digitalWrite(mr1, HIGH);
  digitalWrite(mr2, LOW);
  analogWrite(EnaLeft, 130);
  analogWrite(EnaRight, 130);
}

void turnRight() {
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, HIGH);
  digitalWrite(mr1, HIGH);
  digitalWrite(mr2, LOW);
  analogWrite(EnaLeft, 190);
  analogWrite(EnaRight, 230);
}

void turnLeft() {
  digitalWrite(ml1, HIGH);
  digitalWrite(ml2, LOW);
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, HIGH);
  analogWrite(EnaLeft, 230);
  analogWrite(EnaRight, 150);
}

void reverse() {
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, HIGH);
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, HIGH);
  analogWrite(EnaLeft, 130);
  analogWrite(EnaRight, 130);
}

void stopMotors() {
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, LOW);
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, LOW);
  analogWrite(EnaLeft, 0);
  analogWrite(EnaRight, 0);
}

// Function to measure distance using ultrasonic sensor
float measureDistance() {
  // Send a 10us pulse to trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the echo pin and calculate distance
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;  // Convert to cm
  return distance;
}