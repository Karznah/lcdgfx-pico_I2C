/*
    MIT License

    Copyright (c) 2018-2019, Alexey Dynda

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

/*
 * @file hal/pico/pico_i2c.h LCDGFX Raspberry Pi Pico Interface communication functions
 */

#pragma once


#if defined(PICO_BOARD)
#include "pico/stdlib.h"
#include "hardware/i2c.h"


#if defined(PICO_USE_I2C1)
#define PICO_I2C i2c1
#else
#define PICO_I2C i2c0
#endif

#ifndef PICO_I2C_DEFAULT_SDA
#define PICO_I2C_DEFAULT_SDA 4
#endif

#ifndef PICO_I2C_DEFAULT_SCL
#define PICO_I2C_DEFAULT_SCL 5
#endif

#ifndef PICO_I2C_FREQUENCY
/** i2c bus frequency in HZ to use, if frequency is not specified by user */
#define PICO_I2C_FREQUENCY 400000
#endif
#ifndef PICO_I2C_BUFFER_SIZE


/**
 * Size of internal transmission buffer. The bytes, passed to the display between
 * start() and stop() calls, are accumulated in that buffer and are sent to the bus
 * as a single i2c transmission. Default size holds the full 128x64 display content
 * (1024 bytes) and the control byte, preceding it.
 */
#define PICO_I2C_BUFFER_SIZE 1025
#endif

/**
 * Class implements i2c support for Raspberry Pi Pico platform
 */
class PicoI2c
{
public:
    /**
     * Creates i2c implementation instance for Raspberry Pi Pico platform.
     * @param scl clock pin to use for i2c, -1 to use PICO_I2C_DEFAULT_SCL pin
     * @param sda data pin to use for i2c, -1 to use PICO_I2C_DEFAULT_SDA pin
     * @param sa i2c address of the device to control over i2c (7 bits)
     * @param frequency frequency in HZ to run i2c bus at, 0 to use PICO_I2C_FREQUENCY
     *
     * @note scl and sda pins must belong to the same hardware block.
     */
    explicit PicoI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00, uint32_t freq = 0);
    ~PicoI2c();

    /**
     * Initializes i2c interface
     */
    void begin();

    /**
     * Closes i2c interface
     */
    void end();

    /**
     * Starts communication with SSD1306 display.
     */
    void start();

    /**
     * Ends communication with SSD1306 display.
     */
    void stop();

    /**
     * Sends byte to SSD1306 device
     * @param data - byte to send
     */
    void send(uint8_t data);

    /**
     * @brief Sends bytes to SSD1306 device
     *
     * Sends bytes to SSD1306 device. This functions gives
     * ~ 30% performance increase than ssd1306_intf.send.
     *
     * @param buffer - bytes to send
     * @param size - number of bytes to send
     */
    void sendBuffer(const uint8_t *buffer, uint16_t size);

    /**
     * Sets i2c address for communication
     * This API is required for some led displays having multiple
     * i2c addresses for different types of data.
     *
     * @param addr i2c address to set (7 bits)
     */
    void setAddr(uint8_t addr)
    {
        m_sa = addr;
    }

private:
    int8_t m_scl;
    int8_t m_sda;
    uint8_t m_sa;
    uint8_t m_mode;
    uint32_t m_frequency;
    i2c_inst_t *m_i2c = PICO_I2C;
    uint16_t m_dataSize = 0;
    uint8_t m_buffer[PICO_I2C_BUFFER_SIZE]{};
};

#endif
