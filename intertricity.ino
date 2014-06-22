/*
 */

#include <GSM.h>
#include <TimerOne.h>
//#include <intertricity.h>


// Printout debug modem AT commands
GSM gsmAccess(true);
GSM_SMS sms;

char remoteNumber[20];  // Holds the emitting number
const int LED_PIN_OFFSET = 4;

struct Status {
    boolean is_updated;
    int port;
};


void init_gsm() {
    Serial.print("GSM");
    boolean notConnected = true;

    while(notConnected) {
        if(gsmAccess.begin()==GSM_READY) {
            notConnected = false;
        }
        else {
            Serial.println("Not connected");
            delay(1000);
        }
    }

    Serial.println(" [done]");
}

void init_screen() {
    Serial.print("Screen");

    for (int port = 0; port <= 1; port++) {
        pinMode(port + LED_PIN_OFFSET, OUTPUT);
        digitalWrite(port + LED_PIN_OFFSET, HIGH);
    }

    Serial.println(" [done]");
}


void setup()
{
    Serial.begin(9600);
    Serial.println("Initializing:");
    init_screen();



    //init_gsm();
    Serial.println("Ready...");
}


void loop() 
{
    struct Status status;

    status = get_serial_status();
    if (status.is_updated) {
        Serial.print("Switching on port: ");
        Serial.println(status.port);
        switch_on(status.port);
    }

    delay(1000);
}

void switch_on(int port) {
    digitalWrite(port + LED_PIN_OFFSET, LOW);
}

struct Status get_serial_status() {
    struct Status s;
    s.is_updated = false;

    if (Serial.available()) {
        Serial.print("Message received from Serial:");

        // Read first char from SMS, this is a number identifying the port that must switch on
        s.port = Serial.parseInt();
        // Read null termination
        Serial.read();
        s.is_updated = true;

        Serial.println(s.port);
    }

    return s;
}

/*
char get_sms_status() {
    char c;

    // Check if SMS available, if there are new SMSes return the first char
    // XXX What if multiple SMS are received during one loop iteration?
    if (sms.available()) {
        Serial.println("Message received from:");

        // Display remote number
        sms.remoteNumber(remoteNumber, 20);
        Serial.println(remoteNumber);

        // Read first char from SMS, this is a number identifying the port that must switch on
        c = sms.read();

        Serial.println(c);
        Serial.println("\nEND OF MESSAGE");

        // delete message from modem memory
        sms.flush();
        Serial.println("MESSAGE DELETED");

    }

    return c;
}
*/

/*
int parse_status(char status) {
    int port;
    int retval = -1;

    port = atoi(status);

    if (port == 1 || port = 2)
        retval = port;

    return retval;
}
*/


