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


uint8_t WiFiDrv::iotCloudUpdate(uint8_t * iotStatus, uint8_t * iotSyncStatus, uint8_t * connStatus, uint8_t * err_code)
{
    tParam params[PARAM_NUMS_4] = { {0, (char *)iotStatus }, {0, (char*)iotSyncStatus} , {0, (char*)connStatus}, {0, (char*)err_code}};
    WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(IOT_UPDATE, PARAM_NUMS_0);

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t retval = 1;
    if(!SpiDrv::waitResponseParams(IOT_UPDATE, PARAM_NUMS_4, params)){
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

uint8_t WiFiDrv::iotCloudReadPropertyBool(const char* name, uint8_t name_len, bool * value, unsigned long * cloudTimestamp)
{
    uint8_t _data1[sizeof(*value)];
    uint8_t _data2[sizeof(*cloudTimestamp)];

    tParam params[PARAM_NUMS_2] = { {0, (char *)_data1 }, {0, (char *)_data2} };
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

    // Wait for reply
    uint8_t retval = WL_FAILURE;
    if(!SpiDrv::waitResponseParams(IOT_READ_BOOL, PARAM_NUMS_2, params)){
        retval = 0;
    }

    SpiDrv::spiSlaveDeselect();

    memcpy(value, &_data1, sizeof(*value));
    memcpy(cloudTimestamp, &_data2, sizeof(*cloudTimestamp));

    return retval;
}

uint8_t WiFiDrv::iotCloudReadPropertyInt(const char* name, uint8_t name_len, int * value, unsigned long * cloudTimestamp)
{
    uint8_t _data1[sizeof(*value)];
    uint8_t _data2[sizeof(*cloudTimestamp)];

    tParam params[PARAM_NUMS_2] = { {0, (char *)_data1 }, {0, (char *)_data2} };
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

    // Wait for reply
    uint8_t retval = WL_FAILURE;
    if(!SpiDrv::waitResponseParams(IOT_READ_INT, PARAM_NUMS_2, params)){
        retval = 0;
    }

    SpiDrv::spiSlaveDeselect();

    memcpy(value, &_data1, sizeof(*value));
    memcpy(cloudTimestamp, &_data2, sizeof(*cloudTimestamp));

    return retval;
}

uint8_t WiFiDrv::iotCloudReadPropertyFloat(const char* name, uint8_t name_len, float * value, unsigned long * cloudTimestamp)
{
    uint8_t _data1[sizeof(*value)];
    uint8_t _data2[sizeof(*cloudTimestamp)];

    tParam params[PARAM_NUMS_2] = { {0, (char *)_data1 }, {0, (char *)_data2} };
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

    // Wait for reply
    uint8_t retval = WL_FAILURE;
    if(!SpiDrv::waitResponseParams(IOT_READ_FLOAT, PARAM_NUMS_2, params)){
        retval = 0;
    }

    SpiDrv::spiSlaveDeselect();
    memcpy(value, &_data1, sizeof(*value));
    memcpy(cloudTimestamp, &_data2, sizeof(*cloudTimestamp));

    return retval;
}

uint8_t WiFiDrv::iotCloudReadPropertyString(const char* name, uint8_t name_len, String& value, unsigned long * cloudTimestamp)
{
    uint8_t _data1[100];
    uint8_t _data2[sizeof(*cloudTimestamp)];

    tParam params[PARAM_NUMS_2] = { {0, (char *)_data1 }, {0, (char *)_data2} };
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

    // Wait for reply
    uint8_t retval = WL_FAILURE;
    if(!SpiDrv::waitResponseParams(IOT_READ_STRING, PARAM_NUMS_2, params)){
        retval = 0;
    }

    SpiDrv::spiSlaveDeselect();

    value =  (char *)_data1;
    memcpy(cloudTimestamp, &_data2, sizeof(*cloudTimestamp));

    return retval;
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

uint8_t WiFiDrv::iotCloudBeginCSR(int keySlot, bool newPrivateKey){
   WAIT_FOR_SLAVE_SELECT();
    // Send Command

    SpiDrv::sendCmd(IOT_BEGIN_CSR, PARAM_NUMS_2);
    SpiDrv::sendParam((uint8_t*)keySlot, sizeof(keySlot), NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)newPrivateKey, sizeof(newPrivateKey), LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 6 + sizeof(keySlot) + sizeof(newPrivateKey);
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
    if (!SpiDrv::waitResponseCmd(IOT_BEGIN_CSR, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}

String WiFiDrv::iotCloudEndCSR(const char* subjectCommonName, uint8_t subjectCommonName_len)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_END_CSR, PARAM_NUMS_1);
    SpiDrv::sendParam((uint8_t*)subjectCommonName, subjectCommonName_len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 5 + subjectCommonName_len;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    String s;
    unsigned char _data[510];
    uint16_t _dataLen = 0;
    SpiDrv::waitResponseData16(IOT_END_CSR, _data, &_dataLen);
    SpiDrv::spiSlaveDeselect();

    return String((char *)_data);
}

uint8_t WiFiDrv::iotCloudBeginStorage(int compressedCertSlot, int serialNumberAndAuthorityKeyIdentifierSlot){
   WAIT_FOR_SLAVE_SELECT();
    // Send Command

    SpiDrv::sendCmd(IOT_BEGIN_STORAGE, PARAM_NUMS_2);
    SpiDrv::sendParam((uint8_t*)compressedCertSlot, sizeof(compressedCertSlot), NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)serialNumberAndAuthorityKeyIdentifierSlot, sizeof(serialNumberAndAuthorityKeyIdentifierSlot), LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 6 + sizeof(compressedCertSlot) + sizeof(serialNumberAndAuthorityKeyIdentifierSlot);
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
    if (!SpiDrv::waitResponseCmd(IOT_BEGIN_STORAGE, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}

uint8_t WiFiDrv::iotCloudEndStorage(byte signature[], byte authorityKeyIdentifier[], byte serialNumber[], int dates[])
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_END_STORAGE, PARAM_NUMS_8);
    SpiDrv::sendParam((uint8_t*)signature, SIGNATURE_LENGTH, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)authorityKeyIdentifier, AUTHORITY_KEY_IDENTIFIER_LENGTH, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)serialNumber, SERIAL_NUMBER_LENGTH, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)dates[0], sizeof(dates[0]), NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)dates[1], sizeof(dates[1]), NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)dates[2], sizeof(dates[2]), NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)dates[3], sizeof(dates[3]), NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)dates[4], sizeof(dates[4]), LAST_PARAM);


    // pad to multiple of 4
    int commandSize = 12 + SIGNATURE_LENGTH + AUTHORITY_KEY_IDENTIFIER_LENGTH + SERIAL_NUMBER_LENGTH + sizeof(dates[0])*5;
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
    if (!SpiDrv::waitResponseCmd(IOT_BEGIN_STORAGE, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}

uint8_t WiFiDrv::iotCloudBeginReconstruction(int keySlot, int compressedCertSlot, int serialNumberAndAuthorityKeyIdentifierSlot){
   WAIT_FOR_SLAVE_SELECT();
    // Send Command

    SpiDrv::sendCmd(IOT_BEGIN_RECONSTRUCTION, PARAM_NUMS_3);
    SpiDrv::sendParam((uint8_t*)keySlot, sizeof(keySlot), NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)compressedCertSlot, sizeof(compressedCertSlot), NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)serialNumberAndAuthorityKeyIdentifierSlot, sizeof(serialNumberAndAuthorityKeyIdentifierSlot), LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 6 + sizeof(keySlot) + sizeof(compressedCertSlot) + sizeof(serialNumberAndAuthorityKeyIdentifierSlot);
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
    if (!SpiDrv::waitResponseCmd(IOT_BEGIN_RECONSTRUCTION, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}

uint16_t WiFiDrv::iotCloudEndReconstruction(const char* countryName, uint8_t countryName_len, const char* organizationName, uint8_t organizationName_len, const char* organizationalUnitName, uint8_t organizationalUnitName_len, const char* commonName, uint8_t commonName_len)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_END_RECONSTRUCTION, PARAM_NUMS_1);
    SpiDrv::sendParam((uint8_t*)countryName, countryName_len, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)organizationName, organizationName_len, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)organizationalUnitName, organizationalUnitName_len, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)commonName, commonName_len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 5 + countryName_len + organizationName_len + organizationalUnitName_len + commonName_len;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint16_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(IOT_END_RECONSTRUCTION, PARAM_NUMS_1, (uint8_t)&_data, &_dataLen))
    {
        WARN("error certificate reconstruction failed");
        _data = 0;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}

uint8_t WiFiDrv::iotCloudGetCert(byte * cert){

    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(IOT_GET_CERT, PARAM_NUMS_0);

    // pad to multiple of 4
    int commandSize = 4;
    while (commandSize % 4) {
        SpiDrv::readChar();
        commandSize++;
    }

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t retval = WL_FAILURE;
    unsigned char _data[510];
    uint16_t _dataLen = 0;
    if(!SpiDrv::waitResponseData16(IOT_GET_CERT, _data, &_dataLen)){
        retval = 0;
    }
    SpiDrv::spiSlaveDeselect();

    memcpy(cert, &_data, _dataLen);
    return retval;
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
