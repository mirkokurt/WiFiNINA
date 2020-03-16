/*
  WiFi.h - Library for Arduino Wifi shield.
  Copyright (c) 2018 Arduino SA. All rights reserved.
  Copyright (c) 2011-2014 Arduino LLC.  All right reserved.

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

#ifndef WiFiLite_h
#define WiFiLite_h

#define WIFI_FIRMWARE_LATEST_VERSION "1.2.1"

#include <inttypes.h>

extern "C" {
	#include "utility/wl_definitions.h"
	#include "utility/wl_types.h"
}

class WiFiLiteClass
{
public:
    WiFiLiteClass();

    /*
     * Get firmware version
     */
    static const char* firmwareVersion();

    int iotBegin(const char* ssid, const char *passphrase, const char *mqtt_broker);
    int iotUpdate(uint8_t * iotStatus, uint8_t * iotSyncStatus, uint8_t * connStatus);
    int iotAddPropertyBool(const char* name, uint8_t permission_type, long seconds);
    int iotAddPropertyInt(const char* name, uint8_t permission_type, long seconds);
    int iotAddPropertyFloat(const char* name, uint8_t permission_type, long seconds);
    int iotAddPropertyString(const char* name, uint8_t permission_type, long seconds);
    int iotWritePropertyBool(const char* name, const bool value);
    int iotWritePropertyInt(const char* name, const int value);
    int iotWritePropertyFloat(const char* name, const float value);
    int iotWritePropertyString(const char* name, String value);
    int iotReadPropertyBool(const char* name, bool * value, unsigned long * cloudTimestamp);
    int iotReadPropertyInt(const char* name, int * value, unsigned long * cloudTimestamp);
    int iotReadPropertyFloat(const char* name, float * value, unsigned long * cloudTimestamp);
    int iotReadPropertyString(const char* name, String * value, unsigned long * cloudTimestamp);

    int iotSetThingId(const char* thing_id);
    int iotSetBoardId(const char* board_id);
    int iotSetSecretDeviceKey(const char* secret_key);

    unsigned long getTime();

    void lowPowerMode();
    void noLowPowerMode();
};

extern WiFiLiteClass WiFiLite;

#endif
