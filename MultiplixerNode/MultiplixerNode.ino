#include <Adafruit_GFX.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_MPU6050 mpu;

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int count = 0;
double kecAwal = 0;
double velocity, accX, veloKm;
const long interval = 1000;
unsigned long prevMillis = 0;

//// Select I2C BUS
//void TCA9548A(uint8_t bus){
//  Wire.beginTransmission(0x70);  // TCA9548A address
//  Wire.write(1 << bus);          // send byte to select bus
//  Wire.endTransmission();
//  Serial.print(bus);
//}

double kecepatan(double vo, double acX, int prevTimeX, int currentTimeX){
  int t = ((currentTimeX/1000) - (prevTimeX/1000)); 
  double v = (vo+(acX*t));
//  Serial.println(v);
  return v;
}

void setup() {
  Serial.begin(115200);

  // Start I2C communication with the Multiplexer
//  Wire.begin();

  // Init OLED display on bus number 2
//  TCA9548A(2);

  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1)
      yield();
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  } 

  display.display();
  delay(500);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);
}

void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis - prevMillis >= interval){
    
   //untuk pembacaan accelerometer
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

//    accX = a.acceleration.x;
    accX = 78.7;
    velocity = kecepatan(kecAwal, accX, prevMillis, currentMillis);
    prevMillis = currentMillis;

    //konversi m/s to km/h
    //double vKm = (velocity/1000)/(1/3600);
    double vKm = velocity*3.6;

    if(vKm >= 10){
      veloKm = vKm;
    }else{
      veloKm = 0;
    }
    Serial.println(veloKm);
    
    // Write to OLED on bus number 2
  //  TCA9548A(2);
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("Distance:");
    display.setCursor(0,15);
    display.print("Node1:");
    display.print(count++);
    display.print("m");
    display.setCursor(0,25);
    display.print("Node2:");
    display.print(count++);
    display.print("m");
    
    //kecepatan
    display.setTextSize(1);
    display.setCursor(70,0);
    display.print("Speed:");
    display.setTextSize(2);
    display.setCursor(70, 15);
    display.print(veloKm,0);
    display.setTextSize(1);
    display.print("km");
    display.display(); 
  
    if(count == 120){
      count = 0;
    } 
  }
}
