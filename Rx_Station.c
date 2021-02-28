#include <RF22.h>
#include <RF22Router.h>
#define SOURCE_ADDRESS 22 //transmitter's Tx source address
#define DESTINATION_ADDRESS 8 //receiver's Rx destination address



RF22Router rf22(SOURCE_ADDRESS); //transmitter mode
int ledpin=7;
int btnpin=6;
int buttonOld=1;
int buttonNew;
int LEDstate=0;
int counter=0;
int counter2=0;
boolean successful_packet;
long randNumber;
int max_delay=3000;
char msg[]="measurements time";
void setup() {
  Serial.begin(9600);
  if(!rf22.init())
  Serial.println("RF22 init failed")  ;
  //same frequency and configuration with transmitter
  if (!rf22.setFrequency(431.0))
    Serial.println("setFrequency Fail");
    rf22.setTxPower(RF22_TXPOW_20DBM);
    rf22.setModemConfig(RF22::GFSK_Rb125Fd125);
    rf22.addRouteTo(DESTINATION_ADDRESS,DESTINATION_ADDRESS);

    randomSeed(2000);


    pinMode(ledpin, OUTPUT);
    pinMode(btnpin,INPUT);
  }


  void loop() {

      buttonNew=digitalRead(btnpin);
      if(buttonOld==0 && buttonNew==1){

          if(LEDstate==0){
            digitalWrite(ledpin,HIGH);
            LEDstate=1;
            counter=counter+1;

            Serial.println("Available Station");

      }
        else{
            digitalWrite(ledpin,LOW);
            LEDstate=0;
            counter=counter+1;

            Serial.println("Unavailable Station");
          }
    }
    buttonOld=buttonNew;

    transmit_availability(counter);


    if(counter%2!=0){

    RF22Router rf22(DESTINATION_ADDRESS);
    rf22.addRouteTo(SOURCE_ADDRESS,SOURCE_ADDRESS);

    uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
    char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
    memset(buf,'\0',RF22_ROUTER_MAX_MESSAGE_LEN);
    memset(incoming,'\0',RF22_ROUTER_MAX_MESSAGE_LEN);
    uint8_t len=sizeof(buf);
    uint8_t from;
    boolean flag=false;

    while(flag=false){

        buttonNew=digitalRead(btnpin);
        if(buttonOld==0 && buttonNew==1){
          if(LEDstate==0){
            digitalWrite(ledpin,HIGH);
            LEDstate=1;
            counter=counter+1;
            Serial.println("Available Station");
            }
        else{
            digitalWrite(ledpin,LOW);
            LEDstate=0;
            counter=counter+1;
            Serial.println("Unavailable Station");
            }
      }
       buttonOld=buttonNew;
       /*If the availability changes from available to unavailable become transmitter*/

       if(counter%2==0){
         RF22Router rf22(SOURCE_ADDRESS);
         rf22.addRouteTo(DESTINATION_ADDRESS,DESTINATION_ADDRESS);
        break;
          }
        if(rf22.recvfromAck(buf,&len,&from)){
          buf[RF22_ROUTER_MAX_MESSAGE_LEN-1]='\0';
          memcpy(incoming,buf,RF22_ROUTER_MAX_MESSAGE_LEN);
          Serial.print("got request from:");
          Serial.println(from,DEC);
          if(!strcmp(incoming,msg)){
          environment_receive();
          }
        }
       }
      }
    }



  void transmit_availability(int counter){
    char data_read[RF22_ROUTER_MAX_MESSAGE_LEN];
    uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
    memset(data_read, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    sprintf(data_read, "%d",counter);
    data_read[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(data_send, data_read, RF22_ROUTER_MAX_MESSAGE_LEN);
    successful_packet=false;
    while(!successful_packet) //try to send the packet
    {
      if(rf22.sendtoWait(data_send,sizeof(data_send),DESTINATION_ADDRESS)!=RF22_ROUTER_ERROR_NONE) //if unsuccessful, do the following
       {
        Serial.println("sendtoWait failed");
        randNumber=random(200,max_delay); //wait for random time
        Serial.println(randNumber);
        delay(randNumber);
       }
       else
       {
        successful_packet=true;
        Serial.println("sendtoWait Succesful");

       }
     }
   }

  void environment_receive(){
      uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
      char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
      memset(buf, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
      memset(incoming, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
      uint8_t len = sizeof(buf);
      uint8_t from;
      boolean flag=false;

      // wait to get measurements

      while (flag==false){
       if (rf22.recvfromAck(buf, &len, &from))
    {
      flag=true;
      buf[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
      memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
      Serial.print("got request from : ");
      Serial.println(from, DEC);
      const char d[3]=" ";
      char *token;

      token=strtok(incoming,d);
     float temperature2=atof(token);
      token=strtok(NULL,d);
     float humidity2=atof(token);
      token=strtok(NULL,d);
     float soil2=atof(token);
      token=strtok(NULL,d);
     float flame2=atof(token);
      Serial.print("Region's temperature is: ");
      Serial.println(temperature2);

      Serial.print("Region's humidity is: ");
      Serial.println(humidity2);
      Serial.print("Ground's humidity of the region is: ");
      Serial.println(soil2/100);
      if(flame2==2){
        Serial.println("No fire");

        }else if(flame2==1){
         Serial.println("Fire between 1-3 feet away");
          }else{
              Serial.println("Fire closer than 1.5 feet away");
            }


      delay(3000);

    }
   }

   //As soon as the station gets the measurements, turns into transmitter mode
     RF22Router rf22(SOURCE_ADDRESS);
     rf22.addRouteTo(DESTINATION_ADDRESS,DESTINATION_ADDRESS);
  }
