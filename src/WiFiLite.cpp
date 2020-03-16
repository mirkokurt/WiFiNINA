/*
  WiFi.cpp - Library for Arduino Wifi shield.
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

#include "utility/wifi_drv.h"
#include "WiFiLite.h"

extern "C" {
  #include "utility/wl_types.h"
}

WiFiLiteClass::WiFiLiteClass()
{
}

const char* WiFiLiteClass::firmwareVersion()
{
	return WiFiDrv::getFwVersion();
}

int WiFiLiteClass::iotBegin(const char* ssid, const char *passphrase, const char *mqtt_broker)
{
	if (WiFiDrv::iotCloudBegin(ssid, strlen(ssid) + 1, passphrase, strlen(passphrase) +1, mqtt_broker, strlen(mqtt_broker) + 1)== WL_FAILURE)
	{
		return - 1;
	}
	return 1;
}

int WiFiLiteClass::iotUpdate(uint8_t * iotStatus, uint8_t * iotSyncStatus, uint8_t * connStatus)
{
	if (!WiFiDrv::iotCloudUpdate(iotStatus, iotSyncStatus, connStatus))
	{
		return - 1;
	}
	return 1;
}

int WiFiLiteClass::iotAddPropertyBool(const char* name, uint8_t permission_type, long seconds)
{
	if (WiFiDrv::iotCloudAddPropertyBool(name, strlen(name) + 1, permission_type, seconds)== WL_FAILURE)
	{
		return - 1;
	}
	return 1;
}

int WiFiLiteClass::iotAddPropertyInt(const char* name, uint8_t permission_type, long seconds)
{
	if (WiFiDrv::iotCloudAddPropertyInt(name, strlen(name) + 1, permission_type, seconds)== WL_FAILURE)
	{
		return - 1;
	}
	return 1;
}

int WiFiLiteClass::iotAddPropertyFloat(const char* name, uint8_t permission_type, long seconds)
{
	if (WiFiDrv::iotCloudAddPropertyFloat(name, strlen(name) + 1, permission_type, seconds)== WL_FAILURE)
	{
		return - 1;
	}
	return 1;
}

int WiFiLiteClass::iotAddPropertyString(const char* name, uint8_t permission_type, long seconds)
{
	if (WiFiDrv::iotCloudAddPropertyString(name, strlen(name) + 1, permission_type, seconds)== WL_FAILURE)
	{
		return - 1;
	}
	return 1;
}

int WiFiLiteClass::iotWritePropertyBool(const char* name, const bool value)
{
	if (WiFiDrv::iotCloudWritePropertyBool(name, strlen(name) + 1, value, sizeof(value))== WL_FAILURE)
	{
		return - 1;
	}
	return 1;
}

int WiFiLiteClass::iotWritePropertyInt(const char* name, const int value)
{
	uint8_t status = 0;

	if (WiFiDrv::iotCloudWritePropertyInt(name, strlen(name) + 1, value, sizeof(value))== WL_FAILURE)
	{
		return - 1;
	}
	return 1;
}

int WiFiLiteClass::iotWritePropertyFloat(const char* name, const float value)
{
	if (WiFiDrv::iotCloudWritePropertyFloat(name, strlen(name) + 1, value, sizeof(value))== WL_FAILURE)
	{
		return - 1;
	}
	return 1;
}

int WiFiLiteClass::iotWritePropertyString(const char* name, String  value)
{
	if (WiFiDrv::iotCloudWritePropertyString(name, strlen(name) + 1, value, value.length())!= WL_FAILURE)
	{
		return - 1;
	}
	return 1;
}

bool WiFiLiteClass::iotReadPropertyBool(const char* name)
{
	bool property;
	property = WiFiDrv::iotCloudReadPropertyBool(name, strlen(name) + 1);

	return property;
}

int WiFiLiteClass::iotReadPropertyInt(const char* name)
{
	int property;
	property = WiFiDrv::iotCloudReadPropertyInt(name, strlen(name) + 1);

	return property;
}

float WiFiLiteClass::iotReadPropertyFloat(const char* name)
{
	float property;
	property = WiFiDrv::iotCloudReadPropertyFloat(name, strlen(name) + 1);

	return property;
}

String WiFiLiteClass::iotReadPropertyString(const char* name)
{
	String property;
	property = WiFiDrv::iotCloudReadPropertyString(name, strlen(name) + 1);

	return property;
}

int WiFiLiteClass::iotSetThingId(const char* thing_id){
    if (WiFiDrv::iotCloudSetThingId(thing_id, strlen(thing_id) + 1)== WL_FAILURE)
    {
			return - 1;
    }
    return 1;
}

int WiFiLiteClass::iotSetBoardId(const char* board_id){
    if (WiFiDrv::iotCloudSetBoardId(board_id, strlen(board_id) + 1)== WL_FAILURE)
    {
			return - 1;
    }
    return 1;
}

int WiFiLiteClass::iotSetSecretDeviceKey(const char* secret_key){
    if (WiFiDrv::iotCloudSetSecretDeviceKey(secret_key, strlen(secret_key) + 1)== WL_FAILURE)
    {
			return - 1;
    }
    return 1;
}

unsigned long WiFiLiteClass::getTime()
{
	return WiFiDrv::getTime();
}

void WiFiLiteClass::lowPowerMode()
{
	WiFiDrv::setPowerMode(1);
}

void WiFiLiteClass::noLowPowerMode()
{
	WiFiDrv::setPowerMode(0);
}

WiFiLiteClass WiFiLite;
