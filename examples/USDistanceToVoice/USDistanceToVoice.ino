/*
 *  USDistanceToVoice.cpp
 *
 *  Tells the distance measured by the ultrasonic distance sensor HCSR04 in meter, since we have no "centi" in the vocabulary.
 *
 *  Copyright (C) 2020  Armin Joachimsmeyer
 *  armin.joachimsmeyer@gmail.com
 *
 *  This file is part of Talkie_new https://github.com/ArminJo/Talkie_new.
 *
 *  Talkie_new is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#include <Arduino.h>

#include "TalkieUtils.h"
#include "Vocab_US_Large.h" // for timeout

#include "HCSR04.h"

/*
 * Voice PWM output pins for different ATmegas:
 *  ATmega328 (Uno and Nano): non inverted at pin 3, inverted at pin 11.
 *  ATmega2560: non inverted at pin 6, inverted at pin 7.
 *  ATmega32U4 (Leonardo): non inverted at pin 10, inverted at pin 9.
 *  ATmega32U4 (CircuitPlaygound): only non inverted at pin 5.
 *
 *  As default both inverted and not inverted outputs are enabled to increase volume if speaker is attached between them.
 *  Use Talkie Voice(true, false); if you only need not inverted pin or if you want to use SPI on ATmega328 which needs pin 11.
 *
 *  The outputs can drive headphones directly, or add a simple audio amplifier to drive a loudspeaker.
 */
Talkie Voice;
//Talkie Voice(true, false);

const uint8_t ECHO_IN_PIN = 4;
const uint8_t TRIGGER_OUT_PIN = 5;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    while (!Serial)
        ; //delay for Leonardo
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_TALKIE));

    initUSDistancePins(TRIGGER_OUT_PIN, ECHO_IN_PIN);

#if defined(TEENSYDUINO)
    pinMode(5, OUTPUT);
    digitalWrite(5, HIGH); //Enable Amplified PROP shield
#endif
    Serial.print("Voice queue size is: ");
    Serial.println(Voice.sayQ(spPAUSE1)); // this initializes the queue and the hardware

}

void loop() {

    int tCentimeter = getUSDistanceAsCentiMeterWithCentimeterTimeout(300);
    // print distance
    if (tCentimeter >= 300) {
        Serial.println("timeout");

        sayQTimeout(&Voice);
        Voice.sayQ(sp2_OUT);
    } else {
        Serial.print("cm=");
        Serial.println(tCentimeter);

        float tDistanceMeter = tCentimeter / 100.0;
        sayQFloat(&Voice, tDistanceMeter, 2, true, true);
        Voice.sayQ(sp2_METER);
    }

// Using .say() here is another way to block the sketch here and wait for end of speech as you can easily see in the source code of say().
    Voice.sayQ(spPAUSE1);
    while (Voice.isTalking()) {
        ;
    }

}
