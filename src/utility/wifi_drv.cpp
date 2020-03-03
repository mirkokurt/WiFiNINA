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

// Array of data to cache the information related to the networks discovered
char 	WiFiDrv::_networkSsid[][WL_SSID_MAX_LENGTH] = {{"1"},{"2"},{"3"},{"4"},{"5"}};

// Cached values of retrieved data
char 	WiFiDrv::_ssid[] = {0};
uint8_t	WiFiDrv::_bssid[] = {0};
uint8_t WiFiDrv::_mac[] = {0};
uint8_t WiFiDrv::_localIp[] = {0};
uint8_t WiFiDrv::_subnetMask[] = {0};
uint8_t WiFiDrv::_gatewayIp[] = {0};
// Firmware version
char    WiFiDrv::fwVersion[] = {0};


// Private Methods

void WiFiDrv::getNetworkData(uint8_t *ip, uint8_t *mask, uint8_t *gwip)
{
    tParam params[PARAM_NUMS_3] = { {0, (char*)ip}, {0, (char*)mask}, {0, (char*)gwip}};

    WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_IPADDR_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    SpiDrv::sendParam(&_dummy, sizeof(_dummy), LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    SpiDrv::waitResponseParams(GET_IPADDR_CMD, PARAM_NUMS_3, params);

    SpiDrv::spiSlaveDeselect();
}

void WiFiDrv::getRemoteData(uint8_t sock, uint8_t *ip, uint8_t *port)
{
    tParam params[PARAM_NUMS_2] = { {0, (char*)ip}, {0, (char*)port} };

    WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_REMOTE_DATA_CMD, PARAM_NUMS_1);
    SpiDrv::sendParam(&sock, sizeof(sock), LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    SpiDrv::waitResponseParams(GET_REMOTE_DATA_CMD, PARAM_NUMS_2, params);

    SpiDrv::spiSlaveDeselect();
}


// Public Methods


void WiFiDrv::wifiDriverInit()
{
    SpiDrv::begin();
}

void WiFiDrv::wifiDriverDeinit()
{
    SpiDrv::end();
}

int8_t WiFiDrv::wifiSetNetwork(const char* ssid, uint8_t ssid_len)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SET_NET_CMD, PARAM_NUMS_1);
    SpiDrv::sendParam((uint8_t*)ssid, ssid_len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 5 + ssid_len;
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
    if (!SpiDrv::waitResponseCmd(SET_NET_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();

    return(_data == WIFI_SPI_ACK) ? WL_SUCCESS : WL_FAILURE;
}

int8_t WiFiDrv::wifiSetPassphrase(const char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SET_PASSPHRASE_CMD, PARAM_NUMS_2);
    SpiDrv::sendParam((uint8_t*)ssid, ssid_len, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)passphrase, len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 6 + ssid_len + len;
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
    if (!SpiDrv::waitResponseCmd(SET_PASSPHRASE_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}


int8_t WiFiDrv::wifiSetKey(const char* ssid, uint8_t ssid_len, uint8_t key_idx, const void *key, const uint8_t len)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SET_KEY_CMD, PARAM_NUMS_3);
    SpiDrv::sendParam((uint8_t*)ssid, ssid_len, NO_LAST_PARAM);
    SpiDrv::sendParam(&key_idx, KEY_IDX_LEN, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)key, len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 8 + ssid_len + len;
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
    if (!SpiDrv::waitResponseCmd(SET_KEY_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}

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
        retval=0;
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
    SpiDrv::sendCmd(IOT_WRITE_STRING, PARAM_NUMS_2);
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

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(IOT_READ_BOOL, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();

    bool b;
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

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(IOT_READ_FLOAT, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();

    int i;
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

    // Wait for reply
    float f;
    uint8_t _dataLen = sizeof(f);
    uint8_t _data[sizeof(f)];
    
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

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(IOT_READ_STRING, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();

    String s;
    memcpy(&s, &_data, _dataLen);
    return s;
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

void WiFiDrv::config(uint8_t validParams, uint32_t local_ip, uint32_t gateway, uint32_t subnet)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SET_IP_CONFIG_CMD, PARAM_NUMS_4);
    SpiDrv::sendParam((uint8_t*)&validParams, 1, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)&local_ip, 4, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)&gateway, 4, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)&subnet, 4, LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();
    SpiDrv::readChar();
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(SET_IP_CONFIG_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
}

void WiFiDrv::setDNS(uint8_t validParams, uint32_t dns_server1, uint32_t dns_server2)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SET_DNS_CONFIG_CMD, PARAM_NUMS_3);
    SpiDrv::sendParam((uint8_t*)&validParams, 1, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)&dns_server1, 4, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)&dns_server2, 4, LAST_PARAM);

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(SET_DNS_CONFIG_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
}

void WiFiDrv::setHostname(const char* hostname)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SET_HOSTNAME_CMD, PARAM_NUMS_1);
    SpiDrv::sendParam((uint8_t*)hostname, strlen(hostname), LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 5 + strlen(hostname);
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
    if (!SpiDrv::waitResponseCmd(SET_HOSTNAME_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
}

int8_t WiFiDrv::disconnect()
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(DISCONNECT_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    SpiDrv::sendParam(&_dummy, 1, LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    int8_t result = SpiDrv::waitResponseCmd(DISCONNECT_CMD, PARAM_NUMS_1, &_data, &_dataLen);

    SpiDrv::spiSlaveDeselect();

    return result;
}

uint8_t WiFiDrv::getReasonCode()
{
    WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_REASON_CODE_CMD, PARAM_NUMS_0);

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 1;
    uint8_t _dataLen = 0;
    SpiDrv::waitResponseCmd(GET_REASON_CODE_CMD, PARAM_NUMS_1, &_data, &_dataLen);

    SpiDrv::spiSlaveDeselect();

    return _data;
}

uint8_t WiFiDrv::getConnectionStatus()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_CONN_STATUS_CMD, PARAM_NUMS_0);

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = -1;
    uint8_t _dataLen = 0;
    SpiDrv::waitResponseCmd(GET_CONN_STATUS_CMD, PARAM_NUMS_1, &_data, &_dataLen);

    SpiDrv::spiSlaveDeselect();

    return _data;
}

uint8_t* WiFiDrv::getMacAddress()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_MACADDR_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    SpiDrv::sendParam(&_dummy, 1, LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _dataLen = 0;
    SpiDrv::waitResponseCmd(GET_MACADDR_CMD, PARAM_NUMS_1, _mac, &_dataLen);

    SpiDrv::spiSlaveDeselect();

    return _mac;
}

void WiFiDrv::getIpAddress(IPAddress& ip)
{
	getNetworkData(_localIp, _subnetMask, _gatewayIp);
	ip = _localIp;
}

 void WiFiDrv::getSubnetMask(IPAddress& mask)
 {
	getNetworkData(_localIp, _subnetMask, _gatewayIp);
	mask = _subnetMask;
 }

 void WiFiDrv::getGatewayIP(IPAddress& ip)
 {
	getNetworkData(_localIp, _subnetMask, _gatewayIp);
	ip = _gatewayIp;
 }

const char* WiFiDrv::getCurrentSSID()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_CURR_SSID_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    SpiDrv::sendParam(&_dummy, 1, LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    memset(_ssid, 0x00, sizeof(_ssid));

    // Wait for reply
    uint8_t _dataLen = 0;
    SpiDrv::waitResponseCmd(GET_CURR_SSID_CMD, PARAM_NUMS_1, (uint8_t*)_ssid, &_dataLen);

    SpiDrv::spiSlaveDeselect();

    return _ssid;
}

uint8_t* WiFiDrv::getCurrentBSSID()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_CURR_BSSID_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    SpiDrv::sendParam(&_dummy, 1, LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _dataLen = 0;
    SpiDrv::waitResponseCmd(GET_CURR_BSSID_CMD, PARAM_NUMS_1, _bssid, &_dataLen);

    SpiDrv::spiSlaveDeselect();

    return _bssid;
}

int32_t WiFiDrv::getCurrentRSSI()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_CURR_RSSI_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    SpiDrv::sendParam(&_dummy, 1, LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _dataLen = 0;
    int32_t rssi = 0;
    SpiDrv::waitResponseCmd(GET_CURR_RSSI_CMD, PARAM_NUMS_1, (uint8_t*)&rssi, &_dataLen);

    SpiDrv::spiSlaveDeselect();

    return rssi;
}

uint8_t WiFiDrv::getCurrentEncryptionType()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_CURR_ENCT_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    SpiDrv::sendParam(&_dummy, 1, LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t dataLen = 0;
    uint8_t encType = 0;
    SpiDrv::waitResponseCmd(GET_CURR_ENCT_CMD, PARAM_NUMS_1, (uint8_t*)&encType, &dataLen);

    SpiDrv::spiSlaveDeselect();

    return encType;
}

int8_t WiFiDrv::startScanNetworks()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(START_SCAN_NETWORKS, PARAM_NUMS_0);

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;

    if (!SpiDrv::waitResponseCmd(START_SCAN_NETWORKS, PARAM_NUMS_1, &_data, &_dataLen))
     {
         WARN("error waitResponse");
         _data = WL_FAILURE;
     }

    SpiDrv::spiSlaveDeselect();

    return ((int8_t)_data == WL_FAILURE)? _data : (int8_t)WL_SUCCESS;
}


uint8_t WiFiDrv::getScanNetworks()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(SCAN_NETWORKS, PARAM_NUMS_0);

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t ssidListNum = 0;
    SpiDrv::waitResponse(SCAN_NETWORKS, &ssidListNum, (uint8_t**)_networkSsid, WL_NETWORKS_LIST_MAXNUM);

    SpiDrv::spiSlaveDeselect();

    return ssidListNum;
}

const char* WiFiDrv::getSSIDNetoworks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
		return (char*)NULL;

	return _networkSsid[networkItem];
}

uint8_t WiFiDrv::getEncTypeNetowrks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
		return ENC_TYPE_UNKNOWN;

	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_IDX_ENCT_CMD, PARAM_NUMS_1);

    SpiDrv::sendParam(&networkItem, 1, LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t dataLen = 0;
    uint8_t encType = 0;
    SpiDrv::waitResponseCmd(GET_IDX_ENCT_CMD, PARAM_NUMS_1, (uint8_t*)&encType, &dataLen);

    SpiDrv::spiSlaveDeselect();

    return encType;
}

uint8_t* WiFiDrv::getBSSIDNetowrks(uint8_t networkItem, uint8_t* bssid)
{
    if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
        return NULL;

    WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_IDX_BSSID, PARAM_NUMS_1);

    SpiDrv::sendParam(&networkItem, 1, LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t dataLen = 0;
    SpiDrv::waitResponseCmd(GET_IDX_BSSID, PARAM_NUMS_1, (uint8_t*)bssid, &dataLen);

    SpiDrv::spiSlaveDeselect();

    return bssid;
}

uint8_t WiFiDrv::getChannelNetowrks(uint8_t networkItem)
{
    if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
        return 0;

    WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_IDX_CHANNEL_CMD, PARAM_NUMS_1);

    SpiDrv::sendParam(&networkItem, 1, LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t dataLen = 0;
    uint8_t channel = 0;
    SpiDrv::waitResponseCmd(GET_IDX_CHANNEL_CMD, PARAM_NUMS_1, (uint8_t*)&channel, &dataLen);

    SpiDrv::spiSlaveDeselect();

    return channel;
}

int32_t WiFiDrv::getRSSINetoworks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
		return 0;
	int32_t	networkRssi = 0;

	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_IDX_RSSI_CMD, PARAM_NUMS_1);

    SpiDrv::sendParam(&networkItem, 1, LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t dataLen = 0;
    SpiDrv::waitResponseCmd(GET_IDX_RSSI_CMD, PARAM_NUMS_1, (uint8_t*)&networkRssi, &dataLen);

    SpiDrv::spiSlaveDeselect();

	return networkRssi;
}

uint8_t WiFiDrv::reqHostByName(const char* aHostname)
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(REQ_HOST_BY_NAME_CMD, PARAM_NUMS_1);
    SpiDrv::sendParam((uint8_t*)aHostname, strlen(aHostname), LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 5 + strlen(aHostname);
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
    uint8_t result = SpiDrv::waitResponseCmd(REQ_HOST_BY_NAME_CMD, PARAM_NUMS_1, &_data, &_dataLen);

    SpiDrv::spiSlaveDeselect();

    if (result) {
        result = (_data == 1);
    }

    return result;
}

int WiFiDrv::getHostByName(IPAddress& aResult)
{
	uint8_t  _ipAddr[WL_IPV4_LENGTH];
	IPAddress dummy(0xFF,0xFF,0xFF,0xFF);
	int result = 0;

	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(GET_HOST_BY_NAME_CMD, PARAM_NUMS_0);

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(GET_HOST_BY_NAME_CMD, PARAM_NUMS_1, _ipAddr, &_dataLen))
    {
        WARN("error waitResponse");
    }else{
    	aResult = _ipAddr;
    	result = (aResult != dummy);
    }
    SpiDrv::spiSlaveDeselect();
    return result;
}

int WiFiDrv::getHostByName(const char* aHostname, IPAddress& aResult)
{
	if (reqHostByName(aHostname))
	{
		return getHostByName(aResult);
	}else{
		return 0;
	}
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

int8_t WiFiDrv::wifiSetApNetwork(const char* ssid, uint8_t ssid_len, uint8_t channel)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SET_AP_NET_CMD, PARAM_NUMS_2);
    SpiDrv::sendParam((uint8_t*)ssid, ssid_len);
    SpiDrv::sendParam(&channel, 1, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 3 + ssid_len;
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
    if (!SpiDrv::waitResponseCmd(SET_AP_NET_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();

    return(_data == WIFI_SPI_ACK) ? WL_SUCCESS : WL_FAILURE;
}

int8_t WiFiDrv::wifiSetApPassphrase(const char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len, uint8_t channel)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SET_AP_PASSPHRASE_CMD, PARAM_NUMS_3);
    SpiDrv::sendParam((uint8_t*)ssid, ssid_len, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)passphrase, len, NO_LAST_PARAM);
    SpiDrv::sendParam(&channel, 1, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 4 + ssid_len + len;
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
    if (!SpiDrv::waitResponseCmd(SET_AP_PASSPHRASE_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}

int8_t WiFiDrv::wifiSetEnterprise(uint8_t eapType, const char* ssid, uint8_t ssid_len, const char *username, const uint8_t username_len, const char *password, const uint8_t password_len, const char *identity, const uint8_t identity_len, const char* ca_cert, uint16_t ca_cert_len)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SET_ENT_CMD, PARAM_NUMS_6);
    SpiDrv::sendBuffer(&eapType, sizeof(eapType));
    SpiDrv::sendBuffer((uint8_t*)ssid, ssid_len);
    SpiDrv::sendBuffer((uint8_t*)username, username_len);
    SpiDrv::sendBuffer((uint8_t*)password, password_len);
    SpiDrv::sendBuffer((uint8_t*)identity, identity_len);
    SpiDrv::sendBuffer((uint8_t*)ca_cert, ca_cert_len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 15 + sizeof(eapType) + ssid_len + username_len + password_len + identity_len + ca_cert_len;
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
    if (!SpiDrv::waitResponseCmd(SET_ENT_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}

int16_t WiFiDrv::ping(uint32_t ipAddress, uint8_t ttl)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(PING_CMD, PARAM_NUMS_2);
    SpiDrv::sendParam((uint8_t*)&ipAddress, sizeof(ipAddress), NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)&ttl, sizeof(ttl), LAST_PARAM);

    // pad to multiple of 4
    SpiDrv::readChar();

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint16_t _data;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(PING_CMD, PARAM_NUMS_1, (uint8_t*)&_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_PING_ERROR;
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}

void WiFiDrv::debug(uint8_t on)
{
    WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(SET_DEBUG_CMD, PARAM_NUMS_1);

    SpiDrv::sendParam(&on, 1, LAST_PARAM);

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
    SpiDrv::waitResponseCmd(SET_DEBUG_CMD, PARAM_NUMS_1, &data, &dataLen);

    SpiDrv::spiSlaveDeselect();
}

float WiFiDrv::getTemperature()
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(GET_TEMPERATURE_CMD, PARAM_NUMS_0);

    SpiDrv::spiSlaveDeselect();
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();
    SpiDrv::spiSlaveSelect();

    // Wait for reply
    uint8_t _dataLen = 0;
    float _data = 0;
    if (!SpiDrv::waitResponseCmd(GET_TEMPERATURE_CMD, PARAM_NUMS_1, (uint8_t*)&_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
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
