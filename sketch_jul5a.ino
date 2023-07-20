// C++ code
//
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <DHT.h>
#define DHTTYPE DHT11
#define SPEED_OF_SOUND 0.034

LiquidCrystal_I2C LCD(0x27, 16, 2);
static const int DHT_PIN = 4;
static const int LED_RED = 12;
static const int LED_YELLOW = 15;
static int SERVO_PIN = 27;
static int PhotoCell = 26;
static int WATER_PIN = 33;
static int RELAY = 5;
const int TRIGGER_PIN = 13;
const int ECHO_PIN = 32;

int light = 0; // store the current light value

Servo myServo;
//Create and bind dht object to type and pin
DHT dht(DHT_PIN, DHTTYPE);

// Poll for humidity and temperature values from DHT sensor
static bool measure_env(float *temp, float *humidity){
  float h = dht.readHumidity();
  //read temperature as Celcius (the default)
  float t = dht.readTemperature();

  if(isnan(h) || isnan(t)) {
    return (false);
  }
  else {
    *temp = t;
    *humidity = h;

    return (true);
  }
}

void setup()
{
  //init Serial console
  Serial.begin(115200);

  //init DHT (humidity and temp sensor)
  dht.begin();

  //Red LED
  pinMode(LED_RED, OUTPUT);

  //Blue LED
  pinMode(LED_YELLOW, OUTPUT);

  //LCD
  LCD.init();
  delay(250);
  LCD.backlight();
  delay(250);
  LCD.print("HELLO");
  delay(1000);
  LCD.clear();
  delay(1000);

  //water level
  pinMode(WATER_PIN, INPUT);

  //Servo motor
  myServo.attach(SERVO_PIN);
  myServo.write(0);

  //Relay
  pinMode(RELAY, OUTPUT);

  //init pins
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

float measureDistance(){
  //reset trigger pin
 digitalWrite(TRIGGER_PIN, LOW);
 delayMicroseconds(2);
  
  //send pulse by setting trigger pin on HIGH state for 10us
 digitalWrite(TRIGGER_PIN, HIGH);
 delayMicroseconds(10);
 digitalWrite(TRIGGER_PIN, LOW); 
  
  //read echo pin to get soundwave travel time, also in us
  long duration = pulseIn(ECHO_PIN, HIGH);
  
  //Calculate and return the distance in cm
  return duration * SPEED_OF_SOUND / 2;
}

void loop()
{
  float temperature;
  float humidity;

  light = analogRead(PhotoCell);
  int level = analogRead(WATER_PIN);

  if(measure_env(&temperature, &humidity)){
    Serial.print("temperature = ");
    Serial.println(temperature);
    Serial.print("humidity = ");
    Serial.println(humidity);

    LCD.setCursor(1, 0);
    LCD.print("temp: ");
    LCD.setCursor(8, 0);
    LCD.print(temperature);
    LCD.setCursor(1, 1);
    LCD.print("humi: ");
    LCD.setCursor(8, 1);
    LCD.print(humidity); 
  }
  else{
    Serial.println("Nothing to report, sir!");
  }

  if(temperature > 25 && humidity > 40){
    digitalWrite(LED_RED, HIGH);
    Serial.println("Warning");
  }
  else{
    digitalWrite(LED_RED, LOW);
    Serial.println("Safe");
  }

  Serial.println(light); // print current  light value

  if(light > 2000) { // If it is bright...
        Serial.println("Bright");
        myServo.write(90);
        delay(1000);
    }
    else if(light > 600 && light < 2001) { // If  it is average light...
        Serial.println("Average");
        myServo.write(0);
    }
    else { // If it's dark...
        Serial.println("Dark");
        myServo.write(0);
        delay(1000);
    }

  Serial.print("Water Level: ");
  Serial.println(level);

  if(level>500){
    digitalWrite(RELAY, HIGH);
  }
  else{
    digitalWrite(RELAY, LOW);
  }

  float distance = measureDistance();

  Serial.print("Distance: ");
  Serial.println(distance);

  if(distance < 10.0){
    digitalWrite(LED_YELLOW, HIGH);
  }
  else{
    digitalWrite(LED_YELLOW, LOW);
  }

  delay(3000);
}