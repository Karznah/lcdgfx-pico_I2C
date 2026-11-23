/*
    MIT License

    Copyright (c) 2016-2020, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "../io.h"

#if defined(PICO_BOARD)
//////////////////////////////////////////////////////////////////////////////////
//                        PI PICO I2C IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////

PicoI2c::PicoI2c(int8_t scl, int8_t sda, uint8_t sa, uint32_t frequency)
    : m_scl(scl)
    , m_sda(sda)
    , m_sa(sa)
    , m_mode(0)
    , m_frequency(frequency)
{
}

PicoI2c::~PicoI2c()
{
}

void PicoI2c::begin()
{
    if(m_sda < 0)
        m_sda = PICO_I2C_DEFAULT_SDA;
    if(m_scl < 0)
        m_scl = PICO_I2C_DEFAULT_SCL;
    if(m_frequency == 0)
        m_frequency = PICO_I2C_FREQUENCY;
    m_i2c = PICO_I2C;

    i2c_init(m_i2c, m_frequency);
    gpio_set_function(m_sda, GPIO_FUNC_I2C);
    gpio_set_function(m_scl, GPIO_FUNC_I2C);
    // most display modules have external pull-ups, internal ones do not harm
    gpio_pull_up(m_sda);
    gpio_pull_up(m_scl);
}

void PicoI2c::end()
{
    m_dataSize = 0;
    i2c_deinit(m_i2c);
}

void PicoI2c::start()
{
    m_dataSize = 0;
}

void PicoI2c::stop()
{
    if(m_dataSize)
    {
        /* Error code is ignored on purpose: there is nothing to recover here, and *
         * the hardware generates STOP condition itself, when transfer is aborted. */
        i2c_write_blocking(m_i2c, m_sa, m_buffer, m_dataSize, false);
    }
    m_dataSize = 0;
}

void PicoI2c::send(uint8_t data)
{
    if(m_dataSize >= sizeof(m_buffer))
    {
        /* Send function puts all data to internal buffer. Restart transmission, *
         * if internal buffer is full, repeating control byte, so the display    *
         * continues to receive the data of the same type.                       */
        stop();
        start();
        send(m_mode);
    }
    if(m_dataSize == 0)
        m_mode = data;
    m_buffer[m_dataSize++] = data;
}

void PicoI2c::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    while(size)
    {
        if(m_dataSize >= sizeof(m_buffer))
        {
            stop();
            start();
            send(m_mode);
        }
        if(m_dataSize == 0)
            m_mode = *buffer;
        uint16_t chunk = static_cast<uint16_t>(sizeof(m_buffer) - m_dataSize);
        if(chunk > size)
            chunk = size;
        memcpy(&m_buffer[m_dataSize], buffer, chunk);
        m_dataSize += chunk;
        buffer += chunk;
        size -= chunk;
    }
}

#endif // PICO_BOARD
