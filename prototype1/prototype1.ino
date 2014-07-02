/*
 * Prepaid Power prototype created for Berlin Startupweekend 2014
 *
 * This code drives our Arduino prototype that does the following:
 *  * The Arduino with GSM shield has a pohone number, to this phone number
 *    one can send an sms message with either the number 1 or 2 as content
 *  * Upon receiving this sms the total credit balance is increased, this
 *    balance is shown on a seven segemnt display
 *  * Based on the number a set amount of time is added to either port 1 or 2
 *  * When there is still time on either port the respective port will give
 *    5 volt power to usb socket 1 or 2
 *  * A timer interrupt routine will decrease the port timers, if there is
 *    no more time left the power to the usb socket will be switched off.
 *
 * Author: Remco Wendt <remco.wendt@gmail.com>
 */

#include <GSM.h>
#include <TimerOne.h>


// Printout debug modem AT commands in the serial console
GSM cellular(true);
GSM_SMS sms;

const int DEFAULT_EXTRA_PORT_TIME = 10;  // By default customer buys 10 second increments
const int PORT1_PIN = 4;
const int PORT2_PIN = 5;


#define SCREEN_A A1
#define SCREEN_B A0
#define SCREEN_C 10
#define SCREEN_D 9
#define SCREEN_E 8
#define SCREEN_F A2
#define SCREEN_G A3

// Mapping for seven segment display
byte digit_map[10][7]={
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW},        // 0
  {LOW, HIGH, HIGH, LOW, LOW, LOW, LOW},            // 1
  {HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH},         // 2
  {HIGH, HIGH, HIGH, HIGH, LOW, LOW, HIGH},         // 3
  {LOW, HIGH, HIGH, LOW, LOW, HIGH, HIGH},          // 4
  {HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH},         // 5
  {HIGH, LOW, HIGH, HIGH, HIGH, HIGH, HIGH},        // 6
  {HIGH, HIGH, HIGH, LOW, LOW, LOW, LOW},           // 7
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH},       // 8
  {HIGH, HIGH, HIGH, HIGH, LOW, HIGH, HIGH},        // 9
};


void screen_display_digit(int digit) {
    // XXX Don't go past 9 :)
    digitalWrite(SCREEN_A, digit_map[digit][0]);
    digitalWrite(SCREEN_B, digit_map[digit][1]);
    digitalWrite(SCREEN_C, digit_map[digit][2]);
    digitalWrite(SCREEN_D, digit_map[digit][3]);
    digitalWrite(SCREEN_E, digit_map[digit][4]);
    digitalWrite(SCREEN_F, digit_map[digit][5]);
    digitalWrite(SCREEN_G, digit_map[digit][6]);
}

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
        if(cellular.begin("6802")==GSM_READY) {
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

    pinMode(SCREEN_A, OUTPUT);
    pinMode(SCREEN_B, OUTPUT);
    pinMode(SCREEN_C, OUTPUT);
    pinMode(SCREEN_D, OUTPUT);
    pinMode(SCREEN_E, OUTPUT);
    pinMode(SCREEN_F, OUTPUT);
    pinMode(SCREEN_G, OUTPUT);

    screen_display_digit(0);

    Serial.println(" [done]");
}

void init_ports() {
    Serial.print("Ports");

    pinMode(PORT1_PIN, OUTPUT);
    digitalWrite(PORT1_PIN, LOW);

    pinMode(PORT2_PIN, OUTPUT);
    digitalWrite(PORT2_PIN, LOW);

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
    digitalWrite(PORT1_PIN, port1_timer > 0 ? HIGH : LOW);
    digitalWrite(PORT2_PIN, port2_timer > 0 ? HIGH : LOW);
}



void setup()
{
    Serial.begin(9600);
    Serial.println("Initializing:");

    init_screen();
    init_ports();
    init_gsm();

    Serial.println("Ready...");
}


void loop() 
{
    struct Status status;

    status = get_sms_status();
    if (status.is_updated) {
        add_credit_to_port(status.port);
    }

    delay(1000);
}

void add_credit_to_port(int port) {
    switch (port) {
        case PORT1:
            port1_timer += DEFAULT_EXTRA_PORT_TIME; 
            inc_balance();
            break;
        case PORT2:
            port2_timer += DEFAULT_EXTRA_PORT_TIME;
            inc_balance();
            break;
    }

    Serial.print("Account balance: ");
    Serial.println(balance);
    Serial.print("Adding time to port: ");
    Serial.println(port);
}

void inc_balance() {
    balance++;
    update_screen();
}


void update_screen() {
    screen_display_digit(balance);
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

struct Status get_sms_status() {
    char c;
    struct Status s;
    s.is_updated = false;

    // Check if SMS available, if there are new SMSes return the first char
    // XXX What if multiple SMS are received during one loop iteration?
    if (sms.available()) {
        Serial.println("Message received from:");

        // Display remote number
        sms.remoteNumber(remoteNumber, 20);
        Serial.println(remoteNumber);

        // Read first char from SMS, this is a number identifying the port that must switch on
        s.port = sms.parseInt();
        sms.read();

        Serial.println(s.port);

        // delete message from modem memory
        sms.flush();
        Serial.println("MESSAGE DELETED");

        s.is_updated = true;
    }

    return s;
}


