/*
  wifi_drv.h - Library for Arduino Wifi shield.
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

#ifndef WiFi_Drv_h
#define WiFi_Drv_h

#include <inttypes.h>
#include "utility/wifi_spi.h"
#include "IPAddress.h"


// Key index length
#define KEY_IDX_LEN     1
// 100 msecs of delay to have the connection established
#define WL_DELAY_START_CONNECTION 100
// firmware version string length
#define WL_FW_VER_LENGTH 6

class WiFiDrv
{
private:

	// firmware version string in the format a.b.c
	static char 	fwVersion[WL_FW_VER_LENGTH];

public:

    /* Initialize the ArduinoIoTCloud.
     * Configure ssid and key into the device.
     *
     * param ssid: Pointer to the SSID string.
     * param ssid_len: Lenght of ssid string.
     * param key: Key input buffer.
     * param len: Lenght of key string.
     * param mqtt_broker: broker address.
     * param mqtt_broker_len: Lenght of broker address string.
     * return: WL_SUCCESS or WL_FAILURE
     */
    static int8_t iotCloudBegin(const char* ssid, uint8_t ssid_len, const void *key, const uint8_t len, const char* mqtt_broker, uint8_t mqtt_broker_len);

    /* Update the ArduinoIoTCloud.
     * Run the state machine of the iot cloud connection
     *
    */
    static uint8_t iotCloudUpdate(uint8_t * iotStatus, uint8_t * iotSyncStatus, uint8_t * connStatus);

    /* Add a property in ArduinoIoTCloud.
     *
     * param name: name of the property.
     * param name_len: Lenght of the name of the property.
     * param permission: 1 READ, 2 WRITE, 3 READWRITE.
     * param seconds: how many seconds to wait between two update of the cloud value.
     */
    static uint8_t iotCloudAddPropertyBool(const char* name, uint8_t name_len, uint8_t permission_type, long seconds);
    static uint8_t iotCloudAddPropertyInt(const char* name, uint8_t name_len, uint8_t permission_type, long seconds);
    static uint8_t iotCloudAddPropertyFloat(const char* name, uint8_t name_len, uint8_t permission_type, long seconds);
    static uint8_t iotCloudAddPropertyString(const char* name, uint8_t name_len, uint8_t permission_type, long seconds);
    /* Add a property in ArduinoIoTCloud.
     *
     * param property_type: identifier of the property type 1:bool, 2:int, 3:float, 4:string
     */
    static uint8_t iotCloudAddProperty(uint8_t property_type, const char* name, uint8_t name_len, uint8_t permission_type, long seconds);

    /* Update the value of a property in ArduinoIoTCloud.
     *
     * param name: name of the property.
     * param name_len: Lenght of the name of the property.
     * param value: value of the property
     * param value_length: length of the value of the property
     */
    static uint8_t iotCloudWritePropertyBool(const char* name, uint8_t name_len, const bool value, uint8_t value_len);
    static uint8_t iotCloudWritePropertyInt(const char* name, uint8_t name_len, const int value, uint8_t value_len);
    static uint8_t iotCloudWritePropertyFloat(const char* name, uint8_t name_len, const float value, uint8_t value_len);
    static uint8_t iotCloudWritePropertyString(const char* name, uint8_t name_len, const String value, uint8_t value_len);

    /* Read the value of a property in ArduinoIoTCloud.
     *
     * param name: name of the property.
     * param name_len: Lenght of the name of the property.
     */
    static bool iotCloudReadPropertyBool(const char* name, uint8_t name_len);
    static int iotCloudReadPropertyInt(const char* name, uint8_t name_len);
    static float iotCloudReadPropertyFloat(const char* name, uint8_t name_len);
    static String iotCloudReadPropertyString(const char* name, uint8_t name_len);

    static uint8_t iotCloudSetThingId(const char* thing_id, uint8_t thing_id_len);
    static uint8_t iotCloudSetBoardId(const char* board_id, uint8_t board_id_len);
    static uint8_t iotCloudSetSecretDeviceKey(const char* secret_key, uint8_t secret_key_len);

    /*
     * Get the firmware version
     * result: version as string with this format a.b.c
     */
    static const char* getFwVersion();

    static uint32_t getTime();

    static void setPowerMode(uint8_t mode);

    static void pinMode(uint8_t pin, uint8_t mode);
    static void digitalWrite(uint8_t pin, uint8_t value);
    static void analogWrite(uint8_t pin, uint8_t value);

};

extern WiFiDrv wiFiDrv;

#endif
