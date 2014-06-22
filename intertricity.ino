/*
 */

#include <GSM.h>
#include <TimerOne.h>


// Printout debug modem AT commands
GSM gsmAccess(true);
GSM_SMS sms;

const int DEFAULT_EXTRA_PORT_TIME = 10;  // By default customer buys 10 second increments
const int PORT1_PIN = 4;
const int PORT2_PIN = 5;

struct Status {
    boolean is_updated;
    int port;
};

char remoteNumber[20];  // Holds the emitting number

int port1_timer = 0;
int port2_timer = 0;

int balance = 0;


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

    Serial.println(" [done]");
}

void init_ports() {
    Serial.print("Ports");

    pinMode(PORT1_PIN, OUTPUT);
    digitalWrite(PORT1_PIN, HIGH);

    pinMode(PORT2_PIN, OUTPUT);
    digitalWrite(PORT2_PIN, HIGH);

    // Initialize interrupt that updates ports every second
    Timer1.initialize();
    Timer1.attachInterrupt(update_ports); 

    Serial.println(" [done]");
}

// Interrupt service routine for ports update timer and
// switch individual ports on or off accordingly
void update_ports() {
    if (port1_timer > 0) {
        port1_timer--;
        Serial.print("Port 1 seconds remaining: ");
        Serial.println(port1_timer);
    }
    if (port2_timer > 0) {
        port2_timer--;
        Serial.print("Port 2 seconds remaining: ");
        Serial.println(port2_timer);
    }

    // If port still has time then leave it switched on otherwise switch off (HIGH)
    digitalWrite(PORT1_PIN, port1_timer > 0 ? LOW : HIGH);
    digitalWrite(PORT2_PIN, port2_timer > 0 ? LOW : HIGH);
}



void setup()
{
    Serial.begin(9600);
    Serial.println("Initializing:");

    //init_screen();
    init_ports();
    //init_gsm();

    Serial.println("Ready...");
}


void loop() 
{
    struct Status status;

    status = get_serial_status();
    if (status.is_updated) {
        switch (status.port) {
            case PORT1:
                port1_timer += DEFAULT_EXTRA_PORT_TIME; 
                balance++;
                break;
            case PORT2:
                port2_timer += DEFAULT_EXTRA_PORT_TIME;
                balance++;
                break;
        }

        Serial.print("Account balance: ");
        Serial.println(balance);
        Serial.print("Adding time to port: ");
        Serial.println(status.port);
    }

    delay(1000);
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


