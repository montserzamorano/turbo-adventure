#include "Particle.h"

SYSTEM_THREAD(ENABLED);

int devicesHandler(String data); // forward declaration
void sendData(void);
void output();

const unsigned long REQUEST_WAIT_MS = 10000;
const unsigned long RETRY_WAIT_MS = 10000;
const unsigned long SEND_WAIT_MS = 20;

uint32_t lastTime;
char inmsg[512];
const char replymsg[60] = "COMPLETED";
String myInStr;
const int value = 5;

enum State { STATE_RECONNECT, STATE_SERVICES, STATE_REQUEST, STATE_REQUEST_WAIT, STATE_CONNECT, STATE_SEND_DATA, STATE_RETRY_WAIT };

State state = STATE_SERVICES;
unsigned long stateTime = 0;
IPAddress serverAddr;
char charac[20];
int serverPort;
char nonce[34];
TCPClient client;

void setup() 
{
	Serial.begin(9600);
	Particle.function("devices", handlerequests);
}

void loop() 
{
    
	switch(state) {
	case STATE_RECONNECT:
	        Serial.println("Looking for a server to connect...");
	        Serial.begin(9600);
	        Particle.function("devices", handlerequests);
	        
	        if(client.connect(serverAddr, serverPort)){
	            Serial.printlnf("Connected to a server.");
	            state = STATE_CONNECT;
	        }
	        else{
	            Serial.printlnf("There is no server available so I'm going to take a nap.");
	            delay(5000);
	            System.sleep(D0,RISING,20,SLEEP_NETWORK_STANDBY);
	            Serial.printlnf("Awake and now I'm going to look for a server to connect.");
	            state = STATE_RECONNECT;
	            delay(10000);
	        }
	        break;
	        
	case STATE_SERVICES:
		if (Particle.connected()) {
			Serial.println("Sending Services Request");
			Particle.publish("devicesSERVICES", WiFi.localIP().toString().c_str(), 10, PRIVATE);
			state = STATE_REQUEST_WAIT;
			stateTime = millis();
		}
		break;
	case STATE_REQUEST:
		if (Particle.connected()) {
			Serial.println("Sending Devices Request");
			Particle.publish("devicesRequest", WiFi.localIP().toString().c_str(), 10, PRIVATE);
			state = STATE_REQUEST_WAIT;
			stateTime = millis();
		}
		break;

	case STATE_REQUEST_WAIT:
		if (millis() - stateTime >= REQUEST_WAIT_MS) {
			state = STATE_RETRY_WAIT;
			stateTime = millis();
		}
		break;

	case STATE_CONNECT:
		if (client.connect(serverAddr, serverPort)) {
		    Serial.println("Connected");
		    fibonacci(); //aqui añadimos fibonacci
		    Serial.println("Sending data to the server and waiting for it to continue the task.");
		    state = STATE_SEND_DATA;
		}
		else {
			Serial.println("Unable to Connect");
			stateTime = millis();
			state = STATE_RECONNECT;
		}
		break;

	case STATE_SEND_DATA:
		if (!client.connected()) {
			Serial.println("server disconnected");
			client.stop();
			state = STATE_RETRY_WAIT;
			stateTime = millis();
			break;
		}

		if (millis() - stateTime >= SEND_WAIT_MS) {
			stateTime = millis();
			sendData();
		}
		client.stop();
		break;

	case STATE_RETRY_WAIT:
		if (millis() - stateTime >= RETRY_WAIT_MS) {
			Serial.printlnf("Entered STATE RETRY WAIT and Next State will be STATE_RECONNECT");
			state = STATE_RECONNECT;
		}
		break;
	}
}

void sendData(void) {

    //int z = 0;
	client.printf("%d\n", value);
	delay(1000); //Required to avoid receiving garbage values
	Serial.printlnf("Sent the parameter to the cloud and now going to sleep for 60 Seconds");
	delay(5000);
	System.sleep(D0,RISING,20,SLEEP_NETWORK_STANDBY); 
	delay(10000);
	Serial.printlnf("Awake and Now going to Receive the data from the Server");
	receive_data(inmsg);
	myInStr = inmsg;
	Serial.printlnf("Printing My instr");
	Serial.printlnf("%s",myInStr.c_str());
	/*if(z == 0)
	 {
	 Serial.printlnf("Entered Output loop");
	  
            if (myInStr.indexOf(replymsg)  >= 0) 
            {
              Serial.printlnf("Trying Output");
              output();
              z = 1; 
            }
	}*/
	output();
	//Serial.printlnf("Out of output loop and now wating to send new data");
	delay(1000);

}

void receive_data(char *ptr)
{
  
       char c;
       int i = 0;
       for (i;i<9;i++)
       {
           c = client.read();
           ptr[i] = c;
       }
       
Serial.printlnf("Out of Receiving Lopp");
}

void output(){
    Serial.println("Entered RGB Control Loop");
    RGB.control(true);
    RGB.color(255, 0, 0);
    for(int j=0; j<4;j++){
        for(int i=0; i<=255; i++)
        {
            RGB.brightness(i);
            delay(1);
        }
        for(int i=255; i>0; i--)
        {
            RGB.brightness(i);
            delay(1);
        }
    }
    RGB.brightness(255);
    RGB.control(false);
}
    

void fibonacci()
{
    Serial.println("Executing Fibonacci Sequence");
    int  a = 0;
	int  b = 1;
	int  c, j ;
	//int value = 5;
		for (j=0;j<value;j++)
		{
			c = a + b;
			a = b;
			b = c;
		}
	Serial.printlnf("The value at the end is %d", c);
	
  delay(1000);
}

void pi(){
    float width;
	float sum;
	int intervals;
	int  i;
	int x;
	intervals = 100;
	width = 1.0 / intervals;

	sum = 0;
	for (i=0; i<intervals; i++)
	{
        x = (i + 0.5) * width;
        sum += 4.0 / (1.0 + x * x);
	}
	sum *= width;
	Serial.printlnf("The Computed value of sum is %d",sum);
	delay(1000);
}
// This is the handler for the Particle.function "devices"
// The server makes this function call after this device publishes a devicesRequest event.
// The server responds with an IP address and port of the server, and a nonce (number used once) for authentication.
int handlerequests(String data) 
{
	Serial.printlnf("devicesHandler data=%s", data.c_str());
	int addr[4];

	if (sscanf(data, "%u.%u.%u.%u,%u,%32s", &addr[0], &addr[1], &addr[2], &addr[3], &serverPort, nonce) == 6) 
	{
		serverAddr = IPAddress(addr[0], addr[1], addr[2], addr[3]);
		Serial.printlnf("serverAddr=%s serverPort=%u nonce=%s", serverAddr.toString().c_str(), serverPort, nonce);
		state = STATE_CONNECT;
	}
	else 
	{
	    sscanf(data, "%s",charac);
	    
	    Serial.printlnf("Received Services=%s", charac);
	    state = STATE_REQUEST;
	}
	return 0;
}
