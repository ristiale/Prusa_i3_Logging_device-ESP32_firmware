#define DEBUG
#define CMDBUFFER_SIZE 64

#include <HardwareSerial.h>
#include <string.h>

byte data1 = 0;
String data2 = "";
int incomingByte = 0;
bool loggedin = false;
bool can_log_off = true;
String printer_message;

HardwareSerial RFID(2);
HardwareSerial PRINTER(1);

void serialEvent();
char processCharInput(char* cmdBuffer, const char c);

void setup() {
  Serial.begin(115200);
  PRINTER.begin(115200, SERIAL_8N1, 14, 27);
  RFID.begin(9600);
  
  pinMode(4, INPUT); //Stop button - is pressed - LOW
  pinMode(5, OUTPUT); //Green LED
  Serial.printf("The device is ready. Please scan the RFID card.");
}

void loop() {
  
  if (PRINTER.available() > 0) //zkouska komunikace s tiskarnou
    {
      
      serialEvent();
    
    }
  
  // TEST cteni RFID karty

  String full_tag_id = readTags();

  String tag_id1 = "0600BC56BC50";
  String tag_id2 = "06008B79CC38";
  String tag_id3 = "2E0023DDF727";

  if((tag_id1 == full_tag_id) && (loggedin == false)){ //Login prvni kartou
    Serial.printf("Hello PrusaLab card #1\n\n");
    PRINTER.println("M698"); //login
    loggedin = true;
    can_log_off = true;
    //PRINTER.println("M300"); //beep
    PRINTER.println("M117 192.168.0.7"); // IP address  *************** NEEDS CHANGES
    PRINTER.println("M117 v1.0"); // version ***************** Usable?
    PRINTER.println("M117 ID:0600BC56BC50"); // ID of card
    PRINTER.println("M117 M:master"); // mode of user member or master
    PRINTER.println("M117 Aleksandar Ristic"); // name of user
  }
  else if((tag_id3 == full_tag_id) && (loggedin == false))
  {
    Serial.printf("Hello PrusaLab card #2\n\n");
    PRINTER.println("M698"); //login
    loggedin = true;
    can_log_off = true;
    //PRINTER.println("M300"); //beep
    PRINTER.println("M117 192.168.0.7"); // IP address  *************** NEEDS CHANGES
    PRINTER.println("M117 v1.0"); // version ***************** Usable?
    PRINTER.println("M117 ID:2E0023DDF727"); // ID of card
    PRINTER.println("M117 M:member"); // mode of user member or master
    PRINTER.println("M117 Tereza Lukovska"); // name of user
  }
  else if(tag_id2 == full_tag_id)
  {
    Serial.printf("Success!\nHello Aleksandar Ristic\n\n");
  }


  if((digitalRead(4) == LOW)&&(loggedin == true)) // If stop button pressed --> logoff
  {
    if(can_log_off == true) //User can log off
    {
      PRINTER.println("M699"); //logoff M command
      loggedin = false;
      can_log_off = false;
      delay(100);
    }
  }

  
}

void serialEvent()
  {
    static char cmdBuffer[CMDBUFFER_SIZE] = "";
    char c;
    while(PRINTER.available()) 
    {
      c = processCharInput(cmdBuffer, PRINTER.read());
      Serial.print(c);
      if (c == '\n') 
      {
        //Full command received. Do your stuff here!			
        if (strcmp("Serial FM logoff", cmdBuffer) == 0) // Log off message from printer
        {
          Serial.println("Successfully logged off of ESP!"); 
          loggedin = false;
        }

        if(strcmp("File selected", cmdBuffer) == 0)
        {
          can_log_off = false;
        }

        if(strcmp("Done printing file", cmdBuffer) == 0)
        {
          can_log_off = true;
        }




        cmdBuffer[0] = 0;
      }
    }
    delay(1);
  }

char processCharInput(char* cmdBuffer, const char c)
  {
    //Store the character in the input buffer
    if (c >= 32 && c <= 126) //Ignore control characters and special ascii characters
    {
      if (strlen(cmdBuffer) < CMDBUFFER_SIZE) 
      { 
        strncat(cmdBuffer, &c, 1);   //Add it to the buffer
      }
      else  
      {   
        return '\n';
      }
    }
    else if ((c == 8 || c == 127) && cmdBuffer[0] != 0) //Backspace
    {

      cmdBuffer[strlen(cmdBuffer)-1] = 0;
    }

    return c;
  }
