// Copyright (c) 2015 Michael Smith, All Rights Reserved
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  o Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  o Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include <sysctl.h>
#include <pin.h>
#include <timer.h>
#include <uart.h>
#include <sct.h>

#include "ebl.h"

#define RXD     P0_0
#define TXD     P0_4
#define TACH    P0_2
#define SPEEDO  P0_3
#define SES     P0_5

#define DEBUG(str)  do { UART0 << str << "\r\n"; } while(0)

EBLDecoder EBL;

extern "C" int main();
static void output_rpm(unsigned rpm);
static void output_mph(unsigned mph);


int
main()
{
    Sysctl::init_24MHz();

    // SCT frequency output setup
    SCT::configure_fout();
    CTOUT_0.claim_pin(TACH);
    CTOUT_1.claim_pin(SPEEDO);
    SCT::set_fout_period(0, 1000);
    SCT::set_fout_period(1, 2000);

    // UART setup
    UART0_RXD.claim_pin(RXD);
    UART0_TXD.claim_pin(TXD);
    UART0.configure(57600);

    // SES LED is active-low
    SES.configure(Pin::Output, Pin::PushPull);
    SES << true;

    // spin forever
    for (;;) {
        unsigned c = '?';

        if (UART0.recv(c)) {

            EBL.decode(c);

            if (EBL.was_updated()) {
                output_rpm(EBL.engine_speed());
                output_mph(EBL.road_speed());
                SES << !EBL.ses_set();
                //SES.toggle();
            }
        }
    }
}

//
// Tachometer output, assuming v8 (4 pulses per revolution):
// ---------------------------------------------------------
//
// interval = 1 / (rpm * 4 / 60)
// interval * (rpm * 4 / 60) = 1
// rpm * 4 / 60 = 1 / interval
// rpm * 4 = 60 / interval
// rpm = 15 / interval
//
// assuming 16-bit timer counting in microseconds, minimum interval is
// 2µs (7500000rpm), maximum is 128ms (117rpm).
//

static void
output_rpm(unsigned rpm)
{
    auto tach_us = 15000000UL / rpm;
    SCT::set_fout_period(0, tach_us);
    //SCT::set_fout_period(0, rpm);
}

//
// Speedometer output, assuming 8 pulses per wheel revolution:
// -----------------------------------------------------------
//
// OE 930 rear wheel = 840 revs per mile
// 100 mph = 23.3 revs per second @ 8 pulses per rev = 186Hz.
//
// interval = 1 / (speed * 840 * 8 / 3600)
// interval * (speed * 840 * 8 / 3600) = 1
// speed * 840 * 8 / 3600 = 1 / interval
// speed * 6720 = 3600 / interval
// speed = 3600 / (interval * 6720)
//
// assuming 16-bit timer counting in microseconds, minimum interval is
// 2µs (267000mph), maximum is 128ms (4mph).
//

static void
output_mph(unsigned mph)
{
    auto speedo_us = 535714UL / mph;
    SCT::set_fout_period(1, speedo_us);
    //SCT::set_fout_period(1, mph);
}
