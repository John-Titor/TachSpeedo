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

#include "ebl.h"

void
EBLDecoder::decode(uint8_t c)
{
    running_sum += c;
    rx_count++;

    switch (decode_state) {
    case WAIT_H1:
        if (c == 0x55) {
            decode_state = WAIT_H2;
            running_sum = c;
        }

        break;

    case WAIT_H2:
        if (c == 0xaa) {
            decode_state = ARRAY;
            field_index = 0;

        } else {
            decode_state = WAIT_H1;
        }

        break;

    case ARRAY:
        switch (field_index) {
        case OFS_ENGINE_FLAG:
            update_data.engine_running = c & 0x80;
            break;

        case OFS_SES_FLAG:
            update_data.ses_flag = c & 0x01;
            break;

        case OFS_ROAD_SPEED_MPH:
            update_data.road_speed_mph = c;
            break;

        case OFS_ENGINE_SPEED:
            update_data.engine_speed_rpm = c * 25;
            break;

        default:
            break;
        }

        if (++field_index == 273) {
            decode_state = WAIT_C1;
        }

        break;

    case WAIT_C1:
        // subtract this byte from the running sum, since it shouldn't be included
        running_sum -= c;

        // this is the high byte of the running sum, so subtract it out
        running_sum -= (c << 8);
        decode_state = WAIT_C2;
        break;

    case WAIT_C2:
        // subtract this byte from the running sum, since it shouldn't be included
        running_sum -= c;

        // ths is the low byte of the running sum, so should be equal
        if (running_sum == c) {
            updated = true;
            current_data = update_data;
            good_packets++;

        } else {
            bad_packets++;
        }

        decode_state = WAIT_H1;
        break;

    default:
        decode_state = WAIT_H1;
        break;
    }
}
