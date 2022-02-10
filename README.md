# Camera-Shutter-Speed-Tester
An Arduino-based shutter speed tester

This program uses the Timer 1 oomparator input interrupt capture mode of an AT328p-based Arduino to measure shutter speed.
A flicker-free continuous light is directed toward one side of the shutter.
The photodiode is positioned on the other side of the shutter such that the photodiode is
exposed to light only when the shutter is fully open. For leaf shutters, open the f/stop
control fully and position the photodiode near the periphery of the shutter. This avoids inaccuracies
in the readings at higher speeds due to the time needed to open and close the shutter, which is not part of the 
calibrated leaf shutter speed. Focal plane shutters are not subject to this distortion.
The photodiode and detector are quite sensitive. The test should be conducted in dim room light. Some
adjustment of light intensity through distance of the source to the shutter may be needed until reliable
results are obtained.

Best results are obtained using a photodiode as sensor rather than a phototransistor. Be sure to wire the photodiode
with the cathode towards the +5 volt rail (reverse biased) as the photodiode is run in photoconductive mode where the reverse-bias current
is linear to the light falling on the device. Note the schematic shows a phototransistor, rather than a photodiode. Testing 
indicates that a photodiode works better under varying illumination than a phototransistor.

The voltage divider at pin D7 sets the comparator threshold voltage, 152 millivolts in this case, which is quite sensitive. No A to D conversion
is necessary when using comparator mode, making sensing very fast and accurate.

The fastest testable shutter speed is limited in software to just less than 1/2000 second. At this speed, inaccuracy is limited to 1% due to
the 16 microsend tick of Timer 1. Accuracy is better as measured shutter speed increases.

The slowest testable shutter speed is just over 1 second.

The speed of each test is displayed as a conventional fraction as well as in microseconds for additional accuracy.

Error conditions (pulse too short, pulse too long (Timer 1 overflow))are displayed.

Measurements are limited to once every 1.25 seconds to limit bounce and spurious readings.

Accuracy with crystal-controlled Arduinos or Atmega 328p is excellent. Arduinos using resonators may be less accurate
and/or temperature stable. Accuracy of the circuit and code was checked with a calibrated square wave generator.
