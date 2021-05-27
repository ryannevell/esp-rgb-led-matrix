/* MIT License
 *
 * Copyright (c) 2019 - 2021 Andreas Merkle <web@blue-andi.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*******************************************************************************
    DESCRIPTION
*******************************************************************************/
/**
 * @brief  DHTx sensor
 * @author Andreas Merkle <web@blue-andi.de>
 * 
 * @addtogroup hal
 *
 * @{
 */

#ifndef __SENSOR_DHTX_H__
#define __SENSOR_DHTX_H__

/******************************************************************************
 * Compile Switches
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <stdint.h>
#include <ISensor.hpp>
#include <SensorChannelType.hpp>
#include <DHTesp.h>

/******************************************************************************
 * Macros
 *****************************************************************************/

/******************************************************************************
 * Types and Classes
 *****************************************************************************/

/**
 * Temperature channel of the DHTx sensor.
 */
class DhtXTemperatureChannel : public SensorChannelFloat32
{
public:

    /**
     * Constructs the temperature channel of the DHTx sensor.
     * 
     * @param[in] driver    The DHTx driver.
     */
    DhtXTemperatureChannel(DHTesp& driver) :
        m_driver(driver)
    {
    }

    /**
     * Destroys the temperature channel of the DHTx sensor.
     */
    ~DhtXTemperatureChannel()
    {
    }

    /**
     * Get the kind of data and its unit.
     * 
     * @return Sensor data unit
     */
    DataWithUnit getDataWithUnit() const final
    {
        return ISensorChannel::DATA_TEMPERATURE_DEGREE_CELSIUS;
    }

    /**
     * Get data value.
     * 
     * @return Sensor data value
     */
    float getValue()
    {
        return m_driver.getTemperature();
    }

private:

    DHTesp& m_driver;   /**< DHTx sensor driver. */

    DhtXTemperatureChannel();
    DhtXTemperatureChannel(const DhtXTemperatureChannel& channel);
    DhtXTemperatureChannel& operator=(const DhtXTemperatureChannel& channel);
};

/**
 * Humidity channel of the DHTx sensor.
 */
class DhtXHumidityChannel : public SensorChannelFloat32
{
public:

    /**
     * Constructs the humidity channel of the DHTx sensor.
     * 
     * @param[in] driver    The DHTx driver.
     */
    DhtXHumidityChannel(DHTesp& driver) :
        m_driver(driver)
    {
    }

    /**
     * Destroys the humidity channel of the DHTx sensor.
     */
    ~DhtXHumidityChannel()
    {
    }

    /**
     * Get the kind of data and its unit.
     * 
     * @return Sensor data unit
     */
    DataWithUnit getDataWithUnit() const final
    {
        return ISensorChannel::DATA_HUMIDITY_PERCENT;
    }

    /**
     * Get data value.
     * 
     * @return Sensor data value
     */
    float getValue()
    {
        return m_driver.getHumidity();
    }

private:

    DHTesp& m_driver;   /**< DHTx sensor driver. */

    DhtXHumidityChannel();
    DhtXHumidityChannel(const DhtXHumidityChannel& channel);
    DhtXHumidityChannel& operator=(const DhtXHumidityChannel& channel);
};

/**
 * DHTx sensor adapter
 */
class SensorDhtX : public ISensor
{
public:

    /**
     * Constructs the DHTx sensor.
     * 
     * @param[in] model DHTx sensor model
     */
    SensorDhtX(DHTesp::DHT_MODEL_t model) :
        m_driver(),
        m_model(model),
        m_isAvailable(false),
        m_temperatureChannel(m_driver),
        m_humidityChannel(m_driver)
    {
    }

    /**
     * Destroys the DHTx sensor.
     */
    ~SensorDhtX()
    {
    }

    /**
     * Configures the sensor, so it is able to provide sensor data.
     */
    void begin() final;

    /**
     * Get sensor name.
     * 
     * @return Sensor name
     */
    const char* getName() const final;
    
    /**
     * Is sensor available?
     * If a sensor is physically not available or the initialization failed (see begin()),
     * this can be checked with this method.
     *
     * @return If sensor is available, it will return true otherwise false.
     */
    bool isAvailable() const final
    {
        return m_isAvailable;
    }

    /**
     * Get number of data channels.
     * 
     * @return Number of data channels.
     */
    uint8_t getNumChannels() const final
    {
        return CHANNEL_ID_COUNT;
    }

    /**
     * Get data channel by index.
     * 
     * @return Data channel
     */
    ISensorChannel* getChannel(uint8_t index) final;

private:

    /**
     * Channel id
     */
    enum ChannelId
    {
        CHANNEL_ID_TEMPERATURE = 0, /**< Temperature channel id */
        CHANNEL_ID_HUMIDITY,        /**< Humidity channel id */
        CHANNEL_ID_COUNT            /**< Number of channels */
    };

    DHTesp                  m_driver;               /**< DHTx sensor driver. */
    DHTesp::DHT_MODEL_t     m_model;                /**< DHTx sensor model */
    bool                    m_isAvailable;          /**< Is a DHTx sensor available or not? */
    DhtXTemperatureChannel  m_temperatureChannel;   /**< Temperature channel */
    DhtXHumidityChannel     m_humidityChannel;      /**< Humidity channel */
    
    SensorDhtX();
    SensorDhtX(const SensorDhtX& sensor);
    SensorDhtX& operator=(const SensorDhtX& sensor);
};

/******************************************************************************
 * Functions
 *****************************************************************************/

#endif  /* __SENSOR_DHTX_H__ */

/** @} */