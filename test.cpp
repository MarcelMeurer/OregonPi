/* ===================================================
C code : test.cpp
* ===================================================
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "RCSwitch.h"
#include "RcOok.h"
#include "Sensor.h"

/* inlcude MQTT */
#include <mosquitto.h>

int main(int argc, char *argv[])
{
	int RXPIN = 2;
	int TXPIN = 0;
        int loggingok;   // Global var indicating logging on or off
        FILE *fp;        // Global var file handle
        
    /* default values */
    char  MQTT_HOSTNAME[] = "localhost\0";
    char  MQTT_USERTNAME[] = "admin\0";
    char  MQTT_PASSWORD[] = "password\0";
    char  MQTT_TOPIC[]    = "433MHz\0";
    char* host = MQTT_HOSTNAME; 
    int port   = 1883;
    char* usr  = MQTT_USERTNAME; 
    char* pswd = MQTT_PASSWORD; 
    char* topic= MQTT_TOPIC;
	
    // Initialize the Mosquitto library
    mosquitto_lib_init();

    // Create a new Mosquitto runtime instance with a random client ID,
    //  and no application-specific callback data.  
    mosq = mosquitto_new (NULL, true, NULL);
    if (!mosq)
    {
      fprintf (stderr, "Can't initialize Mosquitto library\n");
      exit (-1);
    }
    
    mosquitto_username_pw_set (mosq, usr, pswd);

    // Establish a connection to the MQTT server. Do not use a keep-alive ping
    int ret = mosquitto_connect (mosq, host, port, 0);
    if (ret)
    {
      fprintf (stderr, "Can't connect to Mosquitto server\n");
      exit (-1);
    }
	
        if(argc==2) {
          fp = fopen(argv[1], "a"); // Log file opened in append mode to avoid destroying data
          loggingok=1;
          if (fp == NULL) {
              perror("Failed to open log file!"); // Exit if file open fails
              exit(EXIT_FAILURE);
          }
        } else {
          loggingok=0;
        }

	if(wiringPiSetup() == -1)
		return 0;

	RCSwitch *rc = new RCSwitch(RXPIN,TXPIN);

	while (1)
	{
		if (rc->OokAvailable())
		{
			char message[100];

			rc->getOokCode(message);
			printf("%s\n",message);

			Sensor *s = Sensor::getRightSensor(message);
			if (s!= NULL)
			{
				printf("Temp : %f\n",s->getTemperature());
				printf("Humidity : %f\n",s->getHumidity());
				printf("Channel : %d\n",s->getChannel());
                                if((loggingok) && (s->getChannel()>0)) {
                                        fprintf(fp,"%d,temp%f,hum%f\n",s->getChannel(),s->getTemperature(),s->getHumidity());
                                        fflush(fp);
                                        fflush(stdout);
                                }
			}
			delete s;
		}
		delay(1000);
	}
}
