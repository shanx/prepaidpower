Prepaid Power prototype created for Berlin Startupweekend 2014
==============================================================

This code drives our Arduino prototype that does the following:
 * The Arduino with GSM shield has a pohone number, to this phone number
   one can send an sms message with either the number 1 or 2 as content
 * Upon receiving this sms the total credit balance is increased, this
   balance is shown on a seven segemnt display
 * Based on the number a set amount of time is added to either port 1 or 2
 * When there is still time on either port the respective port will give
   5 volt power to usb socket 1 or 2
 * A timer interrupt routine will decrease the port timers, if there is
   no more time left the power to the usb socket will be switched off.
