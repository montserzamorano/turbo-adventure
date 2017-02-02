#include "Particle.h"

SYSTEM_THREAD(ENABLED);

int devicesHandler(String data); // forward declaration
void sendData(void);

const unsigned long REQUEST_WAIT_MS = 10000;
const unsigned long RETRY_WAIT_MS = 10000;
const unsigned long SEND_WAIT_MS = 20;

uint32_t lastTime;
char inmsg[512];
const char replymsg[60] = "COMPLETED";
const char ServerFibo[60] = "FIBONACCI";
const char ServerPi[60] = "PI";

String myInStr;
const int value = 5;

enum State { STATE_SELF_EXECUTION, STATE_SERVICES, STATE_REQUEST, STATE_REQUEST_WAIT, STATE_CONNECT, STATE_SEND_DATA, STATE_RETRY_WAIT };

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
	case STATE_SELF_EXECUTION:
	        fibonacci_pi();
	        state = STATE_SERVICES;
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
//	Serial.printlnf("serverAddr=%s serverPort=%u nonce=%s", serverAddr.toString().c_str(), serverPort, nonce);
		if (client.connect(serverAddr, serverPort)) {
		    Serial.println("Connected");
		    state = STATE_SEND_DATA;
		}
		else {
			//state = STATE_RETRY_WAIT;
			Serial.println("Unable to Connect");
			Serial.println(WiFi.localIP());
			stateTime = millis();
			state = STATE_SELF_EXECUTION;
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
		break;

	case STATE_RETRY_WAIT:
		if (millis() - stateTime >= RETRY_WAIT_MS) {
			//state = STATE_REQUEST;
			Serial.printlnf("Entered STATE RETRY WAIT and Next State will be STATE_SELF_EXECUTION");
			state = STATE_SELF_EXECUTION;
		}
		break;
	}
}

void sendData(void) {

    int z = 0;
	client.printf("%d\n", value);
	delay(1000); //Required to avoid receiving garbage values
	Serial.printlnf("Sent the parameter and now going to sleep for 60 Seconds");
	delay(5000);
	System.sleep(D0,RISING,20,SLEEP_NETWORK_STANDBY);
	delay(10000);
	Serial.printlnf("Awake and Now going to Receive the data from the Server");
	receive_data(inmsg);
	Serial.printlnf("inmsg");
	myInStr = inmsg;
	Serial.printlnf("Printing My instr");
	Serial.printlnf(myInStr);
	 if(z == 0)
	 {
	 Serial.printlnf("Entered Output loop");

	 					//Here we see if what has been sent is the word COMPLETED
            if (myInStr.indexOf(replymsg)  >= 0)
            {
							Serial.printlnf("Server has sent to us the last value and the task has been COMPLETED");
              Serial.printlnf("Trying Breathing Leds now");
              output ();
              z = 1;
            }
            /*else if ((inmsg[0] = 'C')|| (inmsg[1] = 'O') || (inmsg[2] = 'M')|| (inmsg[3] = 'P') || (inmsg[4] = 'L')|| (inmsg[5] = 'E') || (inmsg[6] = 'T')|| (inmsg[7] = 'E'))
            {
               Serial.printlnf("Trying Output with Alternative Method");
               output ();
               z = 1;
            }*/

	}
	Serial.printlnf("Out of output loop and now wating to send new data");
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

Serial.printlnf("Out of Receiving Loop");
}

void output ()
{
    Serial.println("Entered RGB Control Loop");
    RGB.control(true);
    RGB.color(0, 0, 255);
    for (int k=200;k>3;k -= 35)
    {
        RGB.brightness(k);
        delay(500);
    }
    RGB.brightness(255);
    RGB.control(false);
}


void fibonacci_pi()
{

	//Here we evaluate which function can the server give to us, in this case is FIBONACCI
	receive_data(inmsg);
	Serial.printlnf("inmsg");
	myInStr = inmsg;
	if (myInStr.indexOf(ServerFibo)  >= 0)
	{
    Serial.println("Executing Fibonacci Sequence");
    int  a = 0;
	int  b = 1;
	int  c, j ;
	for (j=0;j<value;j++)
	{
			c = a + b;
			a = b;
			b = c;
			Serial.printlnf("The value in the %d (from %d) loop is %d",j,value,c);
	}
	client.printf("%d\n", value);
	Serial.printlnf("The value at the end is %d and Now Calculating the Value of pi", c);

  /*double width, sum, x;
  int intervals, i;
  // get the number of intervals
  intervals = 100;
  width = 1.0 / intervals;

  //do the computation
  sum = 0;
  for (i=0; i<intervals; ++i)
  {
     x = (i + 0.5) * width;
    sum += 4.0 / (1.0 + x * x);
  }
  sum *= width; //multiply assign operator, x *= y  equals to x = x*y

  Serial.printlnf("Estimation of pi is %f\n", sum);*/
  delay(1000);
  Serial.println("Application Done, Now look for the Server");
 }
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
