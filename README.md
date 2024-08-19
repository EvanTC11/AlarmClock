Alarm Clock built for the ESP32 using Espressif

To Build:
cd into the directory and run "idf.py build"

To Flash to ESP32:
run "idf.py -p PORT flash"

To monitor output:
run "idf.py -p PORT monitor"

The body to a request to the ESP32 HTTP server should look something like this:
{
    "hour" : int,
    "minute": int,
    "second": int,
    "reset": bool - NOTE: only needed if alarm has already been set.
}

This is a post request to "(ESP32 IP)/set" (Note viewable by monitoring the output).

Then, when the alarm goes off, the blue LED (gpio 2) should flash for a minute or so

