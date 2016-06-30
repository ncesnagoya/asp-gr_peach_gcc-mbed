/* main.cpp */
/* Copyright (C) 2016 Nozomu Fujita, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <Arduino.h>
#include <wiring_private.h>
#include <Tone_private.h>
#include <MsTimer2_private.h>

#include "arduino_main.h"

static void msecInterrupt(void);

void arduino_main_task(intptr_t exinf)
{
    start1msecInterrupt(msecInterrupt);
    setup();
    for (;;) {
        loop();
    }
    return;
}

static void msecInterrupt(void)
{
    if (updateMsTimer2 != NULL) {
        updateMsTimer2();
    }
    if (update1msecCounter != NULL) {
        update1msecCounter();
    }
    if (updateToneDuration != NULL) {
        updateToneDuration();
    }
}
