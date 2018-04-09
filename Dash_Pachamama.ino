#include <DHT.h> //include the DHT library
#define DHTPIN D14     // DHT Sensor output pin connected to Dash GPIO
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE); //DHT Settings 

//Declare RGB LED Pins
int blue_led = D15;
int green_led = D16;
int red_led = D18;


//Declare Other Sensors
int moisture_sensor = D13;
int light_sensor = D19;

//Declare Auto Topic Customization 
String ICCID = "ICCID";
String WateredTopic = "Watered_";
String ThirstyTopic = "Thirsty_";
String PachamamaTopic = "Pachamama_";


//Declare misc. Variables
int delay_to_send = 300000; //delay between sending messages in ms
unsigned long previousMillis = 0; //initialize timekeeping
int moisture = 100; //Set Moisture Value Dummy

//int sensorpower = D12; //optional power for the probe to avoid corrosion uncomment if you want to use the sensorpower pin

void setup() {

  pinMode (blue_led, OUTPUT);
  pinMode (green_led, OUTPUT);
  pinMode (red_led, OUTPUT);
  pinMode (sensorpower, OUTPUT);

    //Boot sequence
  digitalWrite (blue_led, HIGH);
  delay(1000);
  digitalWrite (blue_led, LOW);
  digitalWrite (green_led, HIGH);
  delay(1000);
  digitalWrite (green_led, LOW);
  digitalWrite (red_led, HIGH);
  delay(1000);
  digitalWrite (red_led, LOW);
  digitalWrite (green_led, HIGH);

  //Personalize Topics for Multiple Devices
  ICCID = HologramCloud.getICCID();
  ICCID = ICCID.substring(14);
  WateredTopic.concat(ICCID);
  ThirstyTopic.concat(ICCID);
  PachamamaTopic.concat(ICCID);
  
  dht.begin(); //Initialize DHT object

  //Establish Communication via Serial and the Hologram Cloud
  Serial.begin(9600);
  Serial.println("Start");
  HologramCloud.sendMessage("Starting", PachamamaTopic);
  delay (3000);

  moisture = Moisture();
  Serial.println(JSON(moisture));
  HologramCloud.sendMessage(JSON(moisture), PachamamaTopic);
}

void loop() {
 
    Serial.println(connectionStatusLED()); //Check Connection Status
    delay(10000);
    HologramCloud.offRGB();
    delay(1000);

  //check time to see if its been more than the delay value
  unsigned long currentMillis = millis();
  if ((unsigned long)(currentMillis - previousMillis) >= delay_to_send) {

    moisture = Moisture();

// Branch for watered plants
    if (moisture > 50) {
      HologramCloud.sendMessage(JSON(moisture), WateredTopic);
      digitalWrite (blue_led, LOW);
      digitalWrite (red_led, LOW);
      digitalWrite (green_led, HIGH);
      
    }

//Branch for Thirsty Plants
    else {
      HologramCloud.sendMessage(JSON(moisture), ThirstyTopic);
      digitalWrite (green_led, LOW );
      digitalWrite (red_led, LOW);
      digitalWrite (blue_led, HIGH);
    }
    Serial.println(JSON(moisture));

    previousMillis = currentMillis;
  }

}


//Getting and Mapping Moisture
int Moisture () {
  //digitalWrite (sensorpower, HIGH); //uncomment if you want to use the sensorpower pin
  //delay(10); //uncomment if you want to use the sensorpower pin
  moisture = analogRead(moisture_sensor);
  moisture = map (moisture, 100, 575, 0, 100); //You will want to test your own upper (575) and lower (100) bounds here
  moisture = constrain (moisture, 0, 100);
 // digitalWrite (sensorpower, LOW);  //uncomment if you want to use the sensorpower pin
  return moisture;
}

//Getting, Mapping, and Formatting our Data in JSON
String JSON(int moisture) {
  char buf[8];
   //digitalWrite (sensorpower, HIGH); //uncomment if you want to use the sensorpower pin
  //delay(10); //uncomment if you want to use the sensorpower pin

  int light = analogRead(light_sensor);
  light = map (light, 500, 1023, 0, 100); //You will want to test your own upper (1023) and lower (500) bounds here
  light = constrain (light, 0, 100);
  dtostrf(moisture, 4, 2, buf);
  String ret = "{\"moisture\":";
  ret.concat(buf);
  ret.concat(", ");
  dtostrf(light, 4, 2, buf);
  ret.concat("\"light\":");
  ret.concat(buf);
  ret.concat(", ");
  dtostrf(dht.readTemperature(), 4, 2, buf);
  ret.concat("\"temp\":");
  ret.concat(buf);
  ret.concat(", ");
  dtostrf(dht.readHumidity(), 4, 2, buf);
  ret.concat("\"humidity\":");
  ret.concat(buf);
  ret.concat("}");
  //digitalWrite (sensorpower, LOW);  //uncomment if you want to use the sensorpower pin
  return ret;
}

//Conection Status FN that changes the onboard LED and returns a connection status string
String connectionStatusLED() {
  String ret = "Conection status is: ";
  if (HologramCloud.isConnected()) {
    ret.concat("good");
    HologramCloud.setRGB("GREEN");
  }
  else {
    ret.concat("bad");
    digitalWrite (red_led, HIGH);
    HologramCloud.setRGB("RED");
  }
  return ret;
}
