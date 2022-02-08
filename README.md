# Camera-Shutter-Speed-Tester
An Arduino-based shutter speed tester

This program uses the Timer 1 oomparator input interrupt capture mode of an AT328p-based Arduino to measure shutter speed.
A flicker-free continuous light is directed toward one side of the shutter.
The photodiode is positioned on the other side of the shutter such that the photodiode is
exposed to light only when the shitter is fully open. For leaf shutters, open the f/stop
control fully and position the photodiode near the periphery of the shutter. This avoids inaccuracies
in the readings at higher speeds due to the time needed to open and close the shutter, which is not part of the 
calibrated leaf shutter speed. Focal plane shutters are not subject to this distortion.
The photodiode and detector is quite sensitive. The test should be conducted in dim room light. Some
adjustment of light intensity through distance of the source to the shutter may be needed until reliable
results are obtained.

Best results are obtained using a photodiode as sensor rather than a phototransistor. Be sure to wire the photodiode
with polarity as indicated in the schematic.

Maximum testable shutter speed is limited to just over 1/2000 second. At this speed, inaccuracy is limited to 1% due to
the 16 microsend tick of Timer 1. Accuracy is better as measured shutter speed increases.

Maximum testable shutter speed is just over 1 second.

Speed of each test is displayed as a fraction as well as in microseconds for additional accuracy.

Error conditions (pulse too short, pulse too long (Timer 1 overflow)})are displayed.

Measurements are limited to once every 1.25 seconds to limit bounce and spurious readings.

Accuracy with crystal-controlled Arduinos or Atmega 328p is excellent. Arduinos using resonators may be less accurate
and/or temperature stable.
