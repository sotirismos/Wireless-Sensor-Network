#include <RF22.h>
#include <RF22Router.h>
#include <dht.h>
#define SOURCE_ADDRESS 8
#define DESTINATION_ADDRESS 22


#define DESTINATION_ADDRESS2 8
#define SOURCE_ADDRESS2 22


#define DHT11_PIN 8
dht DHT;
RF22Router rf22(SOURCE_ADDRESS); // receiver mode
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum
int i;
int counter2;
boolean successful_packet;
long randNumber;
int max_delay=1000;
int availableStations=0;
int max_stations=1;
unsigned long current_time;


int present[100]={};
unsigned long start_measurement_time;
int measurement_time=1000; //μέγιστος χρόνος αναμονής
int station; //which station send the signal
void setup() {
  Serial.begin(9600);


  if(!rf22.init())
  Serial.println("RF22 init failed");

  if(!rf22.setFrequency(431.0))
  Serial.println("setFrequency Fail");

  rf22.setTxPower(RF22_TXPOW_20DBM);

  rf22.setModemConfig(RF22::GFSK_Rb125Fd125);

  rf22.addRouteTo(DESTINATION_ADDRESS,DESTINATION_ADDRESS);

  start_measurement_time=millis();
  randomSeed(2000);

}

void loop() {
  uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
  char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(buf, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(incoming, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  uint8_t len = sizeof(buf);
  uint8_t from;
  int received_value = 0;
  boolean flag=false;
 while(flag==false){
  if (rf22.recvfromAck(buf, &len, &from))
  {
    flag=true;
    buf[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
    Serial.print("got request from : ");
    Serial.println(from, DEC);
    received_value = atoi((char*)incoming);

    station=atoi((char*)from); //which station send the signal
    //If received value is odd then the station is available
    if(received_value %2 !=0 && present[station-1]==0)
    {
     availableStations=availableStations+1;
     present[station-1]=received_value;


     //If received value is even and the station was available
    }else if (received_value %2 ==0 && present[station-1]!=0){
      present[station-1]=received_value;
      availableStations=availableStations-1;
    }

  }
 }





  current_time=millis();
  /*
  * If availablestations are greater than zero and the required time has passed then get ready to send measurements
  */
  if (availableStations>0 && current_time - start_measurement_time>=measurement_time){

   //Become Receiver

    RF22Router rf22(SOURCE_ADDRESS);
    rf22.addRouteTo(DESTINATION_ADDRESS,DESTINATION_ADDRESS);
    rf22.addRouteTo(DESTINATION_ADDRESS2,DESTINATION_ADDRESS2);

    //Call the function
    environment(availableStations);
    start_measurement_time=current_time;

    }
}





void environment(int availablestations){
    /*Send the confirmation message first*/
    int  x=availablestations;
    int success=0;


    char msgtosend[]="measurements time";
    char data_read[RF22_ROUTER_MAX_MESSAGE_LEN];
    uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
    memset(data_read,'\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    memset(data_send,'\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    sprintf(data_read, " %s ","measurements time");
    data_read[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(data_send, data_read, RF22_ROUTER_MAX_MESSAGE_LEN);
    boolean successful_packet=false;
    for (counter2=0;counter2<availablestations;counter2++){
    while(!successful_packet){
    if(rf22.sendtoWait(data_send,sizeof(data_send),DESTINATION_ADDRESS)!= RF22_ROUTER_ERROR_NONE){


    Serial.println("sendtoWait failed");
    randNumber=random(200,max_delay);
    delay(randNumber);
    }else{
      successful_packet=true;
      Serial.println("sendtoWait Succesful");
      success=success+1;
     }
   }
   if(success==x){
      success=0;
      break;
   }
 }

int s;
  /*SOIL MOISTURE*/
  int sensorPin = A1;
  int sensorValue2;
  sensorValue2 = analogRead(sensorPin);
  float voltage3 = (sensorValue2/124.0) * 5.0;
  float soil = (voltage3 - 0.05) * 100;
  int soil2 = (int)(soil*100);


  /*TEMPERATURE-HUMIDTIY*/
  int chk = DHT.read11(DHT11_PIN);

  float voltage2=DHT.temperature;
  float temperature=voltage2;

  int temperature2=(int)(temperature);

  float voltage1=DHT.humidity;
  float humidity=voltage1;
  int humidity2=(int)(humidity);

  /*FLAME SENSOR*/
  int sensorReading = analogRead(A0);
  int range = map(sensorReading, sensorMin, sensorMax, 0, 3);
  // range value:
  switch (range) {
  case 0:    // A fire closer than 1.5 feet away.
    s=0;
    break;
    case 1:    // A fire between 1-3 feet away.
    s=1;
    break;
    case 2:    // No fire detected.
    s=2;
     break;
  }
  delay(1000);


 //ready to send measurements

  data_read[RF22_ROUTER_MAX_MESSAGE_LEN];
  data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(data_read,'\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(data_send,'\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  sprintf(data_read, " %d %d %d %d",temperature2 ,humidity2 ,soil2 ,s);
  memcpy(data_send, data_read, RF22_ROUTER_MAX_MESSAGE_LEN);
  data_read[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';



 for (counter2=0;counter2<availableStations;counter2++)
  {
  boolean successful_packet=false;
  while(!successful_packet){
      if(rf22.sendtoWait(data_send,sizeof(data_send),DESTINATION_ADDRESS)!=RF22_ROUTER_ERROR_NONE)
       {
        Serial.println("sendtoWait failed");
        randNumber=random(200,max_delay);

        delay(randNumber);
       }
      else
      {
      successful_packet=true;
      Serial.println("sendtoWait Succesful");
      success=success+1;
        }
      }
     /*When you send the measurements to everyone become Receiver again*/

     if(x=success)
     {
      RF22Router rf22(DESTINATION_ADDRESS2);
      rf22.addRouteTo(SOURCE_ADDRESS2,SOURCE_ADDRESS2);

      break;
      }
     }
    }
