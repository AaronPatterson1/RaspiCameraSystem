## BTS Module 2
----

`Task: Which motion sensor to acquire and explain its API`

I decided to acquire the [Adafruit PIR motion sensor](https://www.adafruit.com/product/189) as it's actually made by the creators
of the Raspberry Pi. Intefacing with it would be easier than other motion sensors.


One thing I've noticed is that the Raspberry Pi developers don't seem to document
their code very well. They show their code modules via examples for some reason as opposed to
laying out all of the commands for ease of use.

The way we would interface with the PIR is using Python. There's a Python module called
RPi.GPIO that deals with general-purpose input/output pins specifically for the Pi.

We would import RPi.GPIO and use some of these commands (import RPi.GPIO as GPIO simplifies the writing):

`GPIO.setmode(GPIO.BOARD)` //sets the numbering system used by the PI

`GPIO.setup(channel, GPIO.IN)` //Where channel is the pin number, can also set an initial value as 3rd arg
`GPIO.setup(channel, GPIO.OUT)` //can also specify output channel

`GPIO.input(channel)` //reads value of pin - either 0/False/LOW or 1/True/HIGH

`GPIO.output(channel, state)` //Sets output state of pin, state is same values as .input can have

`GPIO.cleanup()` // Cleans up all channels at the end of program, can specify which channels as well

`GPIO.add_event_detect(channel, GPIO.edge_change)` //You can add a type of event detected using GPIO.RISING, GPIO.FALLING or GPIO.BOTH (replace .edge_change arg with these) and check for them.
We will most likely use this command the most to detect changes in the motion sensor's value. Everything else can
be written using Python code and packages.

This is the resource I used that gives examples of these commands. It's the Pi's
extent of documenting this Python module.

https://sourceforge.net/p/raspberry-gpio-python/wiki/Home/
