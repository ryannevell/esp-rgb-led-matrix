/* MIT License
 *
 * Copyright (c) 2019 Andreas Merkle <web@blue-andi.de>
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
@brief  Update manager
@author Andreas Merkle <web@blue-andi.de>

@section desc Description
@see UpdateMgr.h

*******************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "UpdateMgr.h"

#include <SPIFFS.h>
#include <DisplayMgr.h>

#include "LedMatrix.h"

/******************************************************************************
 * Compiler Switches
 *****************************************************************************/

/******************************************************************************
 * Macros
 *****************************************************************************/

/******************************************************************************
 * Types and classes
 *****************************************************************************/

/******************************************************************************
 * Prototypes
 *****************************************************************************/

/******************************************************************************
 * Local Variables
 *****************************************************************************/

/* Set over-the-air update password */
const char*     UpdateMgr::OTA_PASSWORD             = "maytheforcebewithyou";

/* Set standard wait time for showing a system message in ms */
const uint32_t  UpdateMgr::SYS_MSG_WAIT_TIME_STD    = 2000u;

/* Instance of the update manager. */
UpdateMgr       UpdateMgr::m_instance;

/******************************************************************************
 * Public Methods
 *****************************************************************************/

void UpdateMgr::init(void)
{
    /* Prepare over the air update. */
    ArduinoOTA.begin();
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.onStart(onStart);
    ArduinoOTA.onEnd(onEnd);
    ArduinoOTA.onProgress(onProgress);
    ArduinoOTA.onError(onError);

    Serial.print("OTA hostname: ");
    Serial.println(ArduinoOTA.getHostname());
    Serial.print("Sketch size: ");
    Serial.print(ESP.getSketchSize());
    Serial.println(" bytes");
    Serial.print("Free size: ");
    Serial.print(ESP.getFreeSketchSpace());
    Serial.println(" bytes");

    m_isInitialized = true;

    return;
}

void UpdateMgr::process(void)
{
    if (true == m_isInitialized)
    {
        ArduinoOTA.handle();
    }

    return;
}

/******************************************************************************
 * Protected Methods
 *****************************************************************************/

/******************************************************************************
 * Private Methods
 *****************************************************************************/

/**
 * Constructs the update manager.
 */
UpdateMgr::UpdateMgr() :
    m_isInitialized(false),
    m_updateIsRunning(false),
    m_progress(0u)
{
}

/**
 * Destroys the update manager.
 */
UpdateMgr::~UpdateMgr()
{
}

void UpdateMgr::onStart(void)
{
    String  infoStr = "Update ";

    m_instance.m_updateIsRunning = true;

    if (U_FLASH == ArduinoOTA.getCommand())
    {
        infoStr += "sketch.";
    }
    else
    {
        infoStr += "filesystem.";

        /* Close filesystem before continue. 
         * Note, this needs a restart after update is finished.
         */
        SPIFFS.end();
    }

    Serial.println(infoStr);
    DisplayMgr::getInstance().showSysMsg(infoStr);

    /* Give the user a chance to read it. */
    DisplayMgr::getInstance().delay(SYS_MSG_WAIT_TIME_STD);

    /* Prepare to show the progress in the next steps. */
    LedMatrix::getInstance().clear();

    /* Reset progress */
    m_instance.m_progress = 0u;

    return;
}

void UpdateMgr::onEnd(void)
{
    String infoStr = "Update successful finished.";

    m_instance.m_updateIsRunning = false;

    Serial.println(infoStr);
    DisplayMgr::getInstance().showSysMsg(infoStr);

    /* Give the user a chance to read it. */
    DisplayMgr::getInstance().delay(SYS_MSG_WAIT_TIME_STD);

    ESP.restart();

    return;
}

void UpdateMgr::onProgress(unsigned int progress, unsigned int total)
{
    const uint32_t  PROGRESS_PERCENT    = (progress * 100u) / total;
    const uint32_t  PIXELS              = Board::LedMatrix::width * Board::LedMatrix::heigth;
    uint32_t        pixelProgress       = (PIXELS * PROGRESS_PERCENT) / 100u;
    uint32_t        delta               = pixelProgress - m_instance.m_progress;
    int16_t         y                   = m_instance.m_progress / Board::LedMatrix::width;
    int16_t         x                   = m_instance.m_progress % Board::LedMatrix::width;
    const uint16_t  COLOR               = 0xF800;   /* Red */

    Serial.printf("Progress: %u%%\r\n", PROGRESS_PERCENT);

    /* Fill the whole display.
     * The number of pixels equals 100% update progress.
     */
    while(0u < delta)
    {
        LedMatrix::getInstance().writePixel(x, y, COLOR);
        ++x;

        if (Board::LedMatrix::width <= x)
        {
            x = 0;
            ++y;
        }

        --delta;
    }

    LedMatrix::getInstance().show();

    m_instance.m_progress = pixelProgress;

    return;
}

void UpdateMgr::onError(ota_error_t error)
{
    String infoStr;

    m_instance.m_updateIsRunning = false;
    
    switch(error)
    {
    case OTA_AUTH_ERROR:
        infoStr = "OTA - Authentication error.";
        break;

    case OTA_BEGIN_ERROR:
        infoStr = "OTA - Begin error.";
        break;

    case OTA_CONNECT_ERROR:
        infoStr = "OTA - Connect error.";
        break;

    case OTA_RECEIVE_ERROR:
        infoStr = "OTA - Receive error.";
        break;

    case OTA_END_ERROR:
        infoStr = "OTA - End error.";
        break;

    default:
        infoStr = "OTA - Unknown error.";
        break;
    }

    Serial.println(infoStr);
    DisplayMgr::getInstance().showSysMsg(infoStr);

    /* Give the user a chance to read it. */
    DisplayMgr::getInstance().delay(SYS_MSG_WAIT_TIME_STD);

    ESP.restart();

    return;
}

/******************************************************************************
 * External Functions
 *****************************************************************************/

/******************************************************************************
 * Local Functions
 *****************************************************************************/