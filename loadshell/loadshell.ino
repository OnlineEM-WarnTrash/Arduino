#include "HX711.h"
#include <SoftwareSerial.h>

SoftwareSerial bt(2, 3);

HX711 scale1;
HX711 scale2;
HX711 scale3;

float calibration_factor = -51500; //로드셀 종류나 상황에 따라 적당한 값으로 시작 + or - 어떤 값이 될지 모름

#define LOAD1DT 5
#define LOAD1SCK 6
#define LOAD2DT 7
#define LOAD2SCK 8
#define LOAD3DT 9
#define LOAD3SCK 10

#define RELAY1  11
#define RELAY2  12
#define RELAY3  13

float Plastic = 0.00;
float Paper = 0.00;
float Metal = 0.00;


void setup() {

  Serial.begin(9600);
  bt.begin(9600);
  scale1.begin(LOAD1DT, LOAD1SCK); //dt 2 sck3
  scale2.begin(LOAD2DT, LOAD2SCK); //dt 2 sck3
  scale3.begin(LOAD3DT, LOAD3SCK); //dt 2 sck3

  Serial.println("HX711 calibration sketch");

  Serial.println("Remove all weight from scale");

  Serial.println("After readings begin, place known weight on scale");

  Serial.println("Press + or a to increase calibration factor");

  Serial.println("Press - or z to decrease calibration factor");

  scale1.set_scale();
  scale2.set_scale();
  scale3.set_scale();

  scale1.tare(); //Reset the scale to 0
  scale2.tare(); //Reset the scale to 0
  scale3.tare(); //Reset the scale to 0​

  long zero_factor1 = scale1.read_average(); //Get a baseline reading
  long zero_factor2 = scale2.read_average(); //Get a baseline reading
  long zero_factor3 = scale3.read_average(); //Get a baseline reading

  Serial.print("Zero factor1: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.

  Serial.println(zero_factor1);

  Serial.print("Zero factor2: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.

  Serial.println(zero_factor2);

  Serial.print("Zero factor3: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.

  Serial.println(zero_factor3);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
}


void loop() {

  scale1.set_scale(calibration_factor); //Adjust to this calibration factor
  scale2.set_scale(calibration_factor); //Adjust to this calibration factor
  scale3.set_scale(calibration_factor); //Adjust to this calibration factor

  Serial.print("Reading1: ");

  Serial.print(scale1.get_units(), 3); // 뒤에 있는 숫자는 몇 자리까지 보여줄지입니다.

  Serial.println(" kg");

  Serial.print("Reading2: ");

  Serial.print(scale2.get_units(), 3); // 뒤에 있는 숫자는 몇 자리까지 보여줄지입니다.

  Serial.println(" kg");

  Serial.print("Reading3: ");

  Serial.print(scale3.get_units(), 3); // 뒤에 있는 숫자는 몇 자리까지 보여줄지입니다.

  Serial.println(" kg");

  if (bt.available()) {
    if (bt.read() == '0') //시작 신호
    {
      Plastic = scale1.get_units();
      Paper = scale2.get_units();
      Metal = scale3.get_units();

      while (bt.read() != '4') {         //종료 신호
        if (bt.read() == '1') {             //플라스틱
          while (scale1.get_units() <= Plastic + 1.00) {
            digitalWrite(RELAY1, LOW);
          }
          digitalWrite(RELAY1, HIGH);
        }
        else if (bt.read() == '2') {        //종이
          while (scale2.get_units() <= Paper + 1.00) {
            digitalWrite(RELAY2, LOW);
          }
          digitalWrite(RELAY2, HIGH);
        }
        else if (bt.read() == '3') {        //캔
          while (scale3.get_units() <= Metal + 1.00) {
            digitalWrite(RELAY3, LOW);
          }
          digitalWrite(RELAY3, HIGH);
        }
      }
      send_message();
    }
    
  }



}




void send_message() {
  String message = "";
  int i = scale1.get_units() * 100;
  float Plastic_Weight = i / 100;
  i = scale2.get_units() * 100;
  float Paper_Weight = i / 100;
  i = scale3.get_units() * 100;
  float Matal_Weight = i / 100;

  message += '#';             //시작 문자
  message += Plastic_Weight;   //플라스틱 무게
  message += ',';             //구분자
  message += Paper_Weight;  //종이 무게
  message += ',';             //구분자
  message += Matal_Weight;  //캔 무게
  message += '@';             //종료 문자

  bt.print(message);
}
