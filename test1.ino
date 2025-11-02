#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// Define pin connections
#define I2C_ADDR 0x27  // LCD I2C Address
#define SS_PIN PA4     // RFID Slave Select (SDA on RC522)
#define RST_PIN PB0    // RFID Reset pin for RC522
#define RELAY_PIN PA8  // Relay pin for access control
#define SERVO1_PIN D9  // Servo 1 control pin
#define SERVO2_PIN D6  // Servo 2 control pin
#define SDA_PIN PA7    // SPI MOSI (connects to RFID module MOSI)
#define SCK_PIN PA5    // SPI Clock (connects to RFID module SCK)
#define MISO_PIN PA6   // SPI MISO (connects to RFID module MISO)
#define MOSI_PIN PA7   // SPI MOSI (connects to RFID module MOSI)

// Initialize components
LiquidCrystal_PCF8574 lcd(I2C_ADDR);
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo servo1;
Servo servo2;

#define ACCESS_DELAY 2000  // 2 seconds for access granted
#define DENIED_DELAY 1000  // 1 second for access denied

String stm32 = "";

void setup() {
  // Initialize I2C LCD
  Wire.begin();
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.setCursor(0, 0);
  lcd.print("Hello, Nucleo!");
  lcd.setCursor(0, 1);
  lcd.print("I2C LCD Ready");

  // Initialize serial communication
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  // Initialize relay and servo pins
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo1.write(0);
  servo2.write(180);
  delay(ACCESS_DELAY);
  // Serial.println("Place your RFID card on the reader...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  // Serial.print("Card UID: ");?
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  // Serial.println(content);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card Detected!");
  lcd.setCursor(0, 1);
  lcd.print(content);

  if (content.substring(1) == "C3 9B 2C DA" || content.substring(1) == "5A AD 31 02") {

    // Serial.println("Access Granted!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted!");
    digitalWrite(RELAY_PIN, HIGH);

    servo1.write(0);
    servo2.write(180);
    stm32 = "/authorized/" + content + "/";
    Serial.println(stm32);

    delay(ACCESS_DELAY);

    servo1.write(90);
    servo2.write(90);
    delay(ACCESS_DELAY);

    servo1.write(0);
    servo2.write(180);
    delay(ACCESS_DELAY);
    digitalWrite(RELAY_PIN, LOW);
  } else {
    // Serial.println("Access Denied");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied!");
    stm32 = "/UN - authorized/" + content + "/";
    Serial.println(stm32);

    delay(DENIED_DELAY);
  }
}
