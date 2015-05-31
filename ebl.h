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

#include <stdint.h>
#include <_compiler.h>

class EBLDecoder
{
private:
    enum {
        WAIT_H1,
        WAIT_H2,
        ARRAY,
        WAIT_C1,
        WAIT_C2
    }           decode_state = WAIT_H1;

    enum {
        OFS_ENGINE_FLAG         = 0x01,
        OFS_SES_FLAG            = 0x0b,
        OFS_ROAD_SPEED_MPH      = 0x34,
        OFS_ENGINE_SPEED        = 0x1c
    };

    struct Message {
        bool        engine_running = false;
        bool        ses_flag = false;
        unsigned    road_speed_mph = 0U;
        unsigned    engine_speed_rpm = 0U;
    };

    Message     current_data;
    Message     update_data;

    unsigned    running_sum = 0U;
    unsigned    field_index = 0U;

    unsigned    rx_count = 0;
    unsigned    good_packets = 0;
    unsigned    bad_packets = 0;

    bool        updated = false;

public:
    bool        engine_running() const __always_inline { return current_data.engine_running; }
    bool        ses_set() const __always_inline { return current_data.ses_flag; }
    unsigned    road_speed() const __always_inline { return current_data.road_speed_mph; }
    unsigned    engine_speed() const __always_inline { return current_data.engine_speed_rpm; }

    void        decode(uint8_t c);

    bool    was_updated()
    {
        if (updated) {
            updated = false;
            return true;
        }

        return false;
    }
};

