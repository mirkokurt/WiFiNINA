/*
  wifi_drv.cpp - Library for Arduino Wifi shield.
  Copyright (c) 2018 Arduino SA. All rights reserved.
  Copyright (c) 2011-2014 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "Arduino.h"
#include "utility/spi_drv.h"
#include "utility/wifi_drv.h"

#define _DEBUG_

extern "C" {
#include "utility/wifi_spi.h"
#include "utility/wl_types.h"
#include "utility/debug.h"
}


// Firmware version
char    WiFiDrv::fwVersion[] = {0};

int8_t WiFiDrv::iotCloudBegin(const char* ssid, uint8_t ssid_len, const void *key, const uint8_t len, const char* mqtt_broker, uint8_t mqtt_broker_len)
{

	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_BEGIN, PARAM_NUMS_3);
    SpiDrv::sendParam((uint8_t*)ssid, ssid_len, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)key, len, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)mqtt_broker, mqtt_broker_len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 7 + ssid_len + len + mqtt_broker_len;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(IOT_BEGIN, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}


uint8_t WiFiDrv::iotCloudUpdate(uint8_t * iotStatus, uint8_t * iotSyncStatus, uint8_t * connStatus)
{
    tParam params[PARAM_NUMS_3] = { {0, (char *)iotStatus }, {0, (char*)iotSyncStatus} , {0, (char*)connStatus}};
    WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(IOT_UPDATE, PARAM_NUMS_0);

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t retval = 1;
    if(!SpiDrv::waitResponseParams(IOT_UPDATE, PARAM_NUMS_3, params)){
        retval = 0;
    }

    SpiDrv::spiSlaveDeselect();

    return retval;
}

uint8_t WiFiDrv::iotCloudAddPropertyBool(const char* name, uint8_t name_len, uint8_t permission_type, long seconds)
{
    return iotCloudAddProperty(1, name, name_len, permission_type, seconds);
}

uint8_t WiFiDrv::iotCloudAddPropertyInt(const char* name, uint8_t name_len, uint8_t permission_type, long seconds)
{
    return iotCloudAddProperty(2, name, name_len, permission_type, seconds);
}

uint8_t WiFiDrv::iotCloudAddPropertyFloat(const char* name, uint8_t name_len, uint8_t permission_type, long seconds)
{
    return iotCloudAddProperty(3, name, name_len, permission_type, seconds);
}

uint8_t WiFiDrv::iotCloudAddPropertyString(const char* name, uint8_t name_len, uint8_t permission_type, long seconds)
{
    return iotCloudAddProperty(4, name, name_len, permission_type, seconds);
}

uint8_t WiFiDrv::iotCloudAddProperty(uint8_t property_type, const char* name, uint8_t name_len, uint8_t permission_type, long seconds)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_ADD_PROPERTY, PARAM_NUMS_4);
    SpiDrv::sendParam(&property_type, 1, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)name, name_len, NO_LAST_PARAM);
    SpiDrv::sendParam(&permission_type, 1, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)&seconds, sizeof(seconds), LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 9 + name_len;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(IOT_ADD_PROPERTY, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}

uint8_t WiFiDrv::iotCloudWritePropertyBool(const char* name, uint8_t name_len, const bool value, uint8_t value_len)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_WRITE_BOOL, PARAM_NUMS_2);
    SpiDrv::sendParam((uint8_t*)name, name_len, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)&value, value_len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 6 + name_len + value_len;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(IOT_WRITE_BOOL, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}

uint8_t WiFiDrv::iotCloudWritePropertyInt(const char* name, uint8_t name_len, const int value, uint8_t value_len)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_WRITE_INT, PARAM_NUMS_2);
    SpiDrv::sendParam((uint8_t*)name, name_len, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)&value, value_len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 6 + name_len + value_len;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(IOT_WRITE_INT, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}

uint8_t WiFiDrv::iotCloudWritePropertyFloat(const char* name, uint8_t name_len, const float value, uint8_t value_len)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_WRITE_FLOAT, PARAM_NUMS_2);
    SpiDrv::sendParam((uint8_t*)name, name_len, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)&value, value_len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 6 + name_len + value_len;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(IOT_WRITE_FLOAT, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}

uint8_t WiFiDrv::iotCloudWritePropertyString(const char* name, uint8_t name_len, String value, uint8_t value_len)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command

    const char * array;
    array = value.c_str();

    SpiDrv::sendCmd(IOT_WRITE_STRING, PARAM_NUMS_2);
    SpiDrv::sendParam((uint8_t*)name, name_len, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)array, value_len + 1, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 6 + name_len + value_len;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(IOT_WRITE_STRING, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}

bool WiFiDrv::iotCloudReadPropertyBool(const char* name, uint8_t name_len)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_READ_BOOL, PARAM_NUMS_1);
    SpiDrv::sendParam((uint8_t*)name, name_len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 5 + name_len;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    bool b;

    uint8_t _data[sizeof(b)];
    uint8_t _dataLen = 0;
    SpiDrv::waitResponseCmd(IOT_READ_BOOL, PARAM_NUMS_1, _data, &_dataLen);
    SpiDrv::spiSlaveDeselect();

    memcpy(&b, &_data, sizeof(b));
    return b;
}

int WiFiDrv::iotCloudReadPropertyInt(const char* name, uint8_t name_len)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_READ_INT, PARAM_NUMS_1);
    SpiDrv::sendParam((uint8_t*)name, name_len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 5 + name_len;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    int i;

    uint8_t _data[sizeof(i)];
    uint8_t _dataLen = 0;

    SpiDrv::waitResponseCmd(IOT_READ_FLOAT, PARAM_NUMS_1, _data, &_dataLen);
    SpiDrv::spiSlaveDeselect();

    memcpy(&i, &_data, sizeof(i));
    return i;
}

float WiFiDrv::iotCloudReadPropertyFloat(const char* name, uint8_t name_len)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_READ_FLOAT, PARAM_NUMS_1);
    SpiDrv::sendParam((uint8_t*)name, name_len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 5 + name_len;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    float f;

    uint8_t _data[sizeof(f)];
    uint8_t _dataLen = 0;

    SpiDrv::waitResponseCmd(IOT_READ_FLOAT, PARAM_NUMS_1, _data, &_dataLen);
    SpiDrv::spiSlaveDeselect();

    memcpy(&f, _data, sizeof(f));
    return f;
}

String WiFiDrv::iotCloudReadPropertyString(const char* name, uint8_t name_len)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_READ_STRING, PARAM_NUMS_1);
    SpiDrv::sendParam((uint8_t*)name, name_len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 5 + name_len;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    String s;
    unsigned char _data[255];
    uint8_t _dataLen = 0;
    SpiDrv::waitResponseCmd(IOT_READ_STRING, PARAM_NUMS_1, _data, &_dataLen);
    SpiDrv::spiSlaveDeselect();

    //memcpy(&s, &_data, _dataLen);
    return String((char *)_data);
}

uint8_t WiFiDrv::iotCloudSetThingId(const char* thing_id, uint8_t thing_id_len)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_SET_THING_ID, PARAM_NUMS_1);
    SpiDrv::sendParam((uint8_t*)thing_id, thing_id_len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 5 + thing_id_len;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(IOT_SET_THING_ID, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();

    return _data;
}

uint8_t WiFiDrv::iotCloudSetBoardId(const char* board_id, uint8_t board_id_len)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_SET_BOARD_ID, PARAM_NUMS_1);
    SpiDrv::sendParam((uint8_t*)board_id, board_id_len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 5 + board_id_len;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(IOT_SET_BOARD_ID, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();

    return _data;
}

uint8_t WiFiDrv::iotCloudSetSecretDeviceKey(const char* secret_key, uint8_t secret_key_len)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_SET_SECRET_KEY, PARAM_NUMS_1);
    SpiDrv::sendParam((uint8_t*)secret_key, secret_key_len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 5 + secret_key_len;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(IOT_SET_SECRET_KEY, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();

    return _data;
}

const char*  WiFiDrv::getFwVersion()
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(GET_FW_VERSION_CMD, PARAM_NUMS_0);

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(GET_FW_VERSION_CMD, PARAM_NUMS_1, (uint8_t*)fwVersion, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
    return fwVersion;
}

uint32_t WiFiDrv::getTime()
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(GET_TIME_CMD, PARAM_NUMS_0);

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _dataLen = 0;
    uint32_t _data = 0;
    if (!SpiDrv::waitResponseCmd(GET_TIME_CMD, PARAM_NUMS_1, (uint8_t*)&_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}

void WiFiDrv::setPowerMode(uint8_t mode)
{
    WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(SET_POWER_MODE_CMD, PARAM_NUMS_1);

    SpiDrv::sendParam(&mode, 1, LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t dataLen = 0;
    uint8_t data = 0;
    SpiDrv::waitResponseCmd(SET_POWER_MODE_CMD, PARAM_NUMS_1, &data, &dataLen);

    SpiDrv::spiSlaveDeselect();
}

void WiFiDrv::pinMode(uint8_t pin, uint8_t mode)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SET_PIN_MODE, PARAM_NUMS_2);
    SpiDrv::sendParam((uint8_t*)&pin, 1, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)&mode, 1, LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(SET_PIN_MODE, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
}

void WiFiDrv::digitalWrite(uint8_t pin, uint8_t value)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SET_DIGITAL_WRITE, PARAM_NUMS_2);
    SpiDrv::sendParam((uint8_t*)&pin, 1, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)&value, 1, LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(SET_DIGITAL_WRITE, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
}

void WiFiDrv::analogWrite(uint8_t pin, uint8_t value)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SET_ANALOG_WRITE, PARAM_NUMS_2);
    SpiDrv::sendParam((uint8_t*)&pin, 1, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)&value, 1, LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(SET_ANALOG_WRITE, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
}

WiFiDrv wiFiDrv;
