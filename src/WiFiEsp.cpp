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

#include "WiFiEsp.h"

extern "C" {
  #include "utility/wl_definitions.h"
  #include "utility/wl_types.h"
  #include "utility/debug.h"
}

WiFiClassEsp::WiFiClassEsp() : _timeout(50000)
{
}

void WiFiClassEsp::init()
{
    WiFiDrv::wifiDriverInit();
}

const char* WiFiClassEsp::firmwareVersion()
{
	return WiFiDrv::getFwVersion();
}

int WiFiClassEsp::begin(const char* ssid)
{
	uint8_t status = WL_IDLE_STATUS;
	
   if (WiFi.begin(ssid)) != WL_FAILURE)
   {
	   for (unsigned long start = millis(); (millis() - start) < _timeout;)
	   {
		   delay(100);
		   status = WiFi.status();
		   if ((status != WL_IDLE_STATUS) && (status != WL_NO_SSID_AVAIL) && (status != WL_SCAN_COMPLETED)) {
		     break;
		   }
	   }
   }else
   {
	   status = WL_CONNECT_FAILED;
   }
   return status;
}

int WiFiClassEsp::begin(const char* ssid, uint8_t key_idx, const char *key)
{
	uint8_t status = WL_IDLE_STATUS;

	// set encryption key
   if (WiFi.begin(ssid, key) != WL_FAILURE)
   {
	   for (unsigned long start = millis(); (millis() - start) < _timeout;)
	   {
		   delay(100);
		   status = WiFi.status();
		   if ((status != WL_IDLE_STATUS) && (status != WL_NO_SSID_AVAIL) && (status != WL_SCAN_COMPLETED)) {
		     break;
		   }
	   }
   }else{
	   status = WL_CONNECT_FAILED;
   }
   return status;
}

int WiFiClassEsp::begin(const char* ssid, const char *passphrase)
{
	uint8_t status = WL_IDLE_STATUS;

    // set passphrase
    if (WiFi.begin(ssid, passphrase)!= WL_FAILURE)
    {
	   for (unsigned long start = millis(); (millis() - start) < _timeout;)
 	   {
 		   delay(100);
 		   status = WiFi.status();
		   if ((status != WL_IDLE_STATUS) && (status != WL_NO_SSID_AVAIL) && (status != WL_SCAN_COMPLETED)) {
		     break;
		   }
 	   }
    }else{
    	status = WL_CONNECT_FAILED;
    }
    return status;
}

void WiFiClassEsp::config(IPAddress local_ip)
{
	//WiFiDrv::config(1, (uint32_t)local_ip, 0, 0);
	WiFi.config((uint32_t)local_ip, 0, 0);
}

void WiFiClassEsp::config(IPAddress local_ip, IPAddress dns_server)
{
	//WiFiDrv::config(1, (uint32_t)local_ip, 0, 0);
	WiFi.config((uint32_t)local_ip, 0, 0);
	//WiFiDrv::setDNS(1, (uint32_t)dns_server, 0);
	WiFi.setDNS((uint32_t)dns_server, (uint32_t)0);
}

void WiFiClassEsp::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
{
	//WiFiDrv::config(2, (uint32_t)local_ip, (uint32_t)gateway, 0);
	WiFi.config((uint32_t)local_ip, (uint32_t)gateway, 0);
	//WiFiDrv::setDNS(1, (uint32_t)dns_server, 0);
	WiFi.setHostname(uint32_t)dns_server);
}

void WiFiClassEsp::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
{
	//WiFiDrv::config(3, (uint32_t)local_ip, (uint32_t)gateway, (uint32_t)subnet);
	//WiFiDrv::setDNS(1, (uint32_t)dns_server, 0);

	WiFi.config((uint32_t)local_ip, (uint32_t)gateway, (uint32_t)subnet);
	WiFi.setDNS((uint32_t)dns_server, (uint32_t)0);
}

void WiFiClassEsp::setDNS(IPAddress dns_server1)
{
	//WiFiDrv::setDNS(1, (uint32_t)dns_server1, 0);
	WiFi.setDNS((uint32_t)dns_server1, 0);
}

void WiFiClassEsp::setDNS(IPAddress dns_server1, IPAddress dns_server2)
{
	//WiFiDrv::setDNS(2, (uint32_t)dns_server1, (uint32_t)dns_server2);
	WiFi.setDNS((uint32_t)dns_server1, (uint32_t)dns_server2);
}

void WiFiClassEsp::setHostname(const char* name)
{
	//WiFiDrv::setHostname(name);
	WiFi.setHostname(hostname);
}

int WiFiClassEsp::disconnect()
{
    return WiFi.disconnect();//WiFiDrv::disconnect();
}

void WiFiClassEsp::end(void)
{
	WiFiDrv::wifiDriverDeinit();
}

uint8_t* WiFiClassEsp::macAddress(uint8_t* mac)
{
	uint8_t mac[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	uint8_t* _mac = WiFi.macAddress(mac);
	memcpy(mac, _mac, WL_MAC_ADDR_LENGTH);
  return mac;
}

IPAddress WiFiClassEsp::localIP()
{
	uint32_t ip = WiFi.localIP();
	return (IPAddress)ip;
}

IPAddress WiFiClassEsp::subnetMask()
{
	IPAddress ret;
	WiFiDrv::getSubnetMask(ret);
	return ret;
}

IPAddress WiFiClassEsp::gatewayIP()
{
	IPAddress ret;
	WiFiDrv::getGatewayIP(ret);
	return ret;
}

const char* WiFiClassEsp::SSID()
{
    return WiFiDrv::getCurrentSSID();
}

uint8_t* WiFiClassEsp::BSSID(uint8_t* bssid)
{
	uint8_t* _bssid = WiFiDrv::getCurrentBSSID();
	memcpy(bssid, _bssid, WL_MAC_ADDR_LENGTH);
    return bssid;
}

int32_t WiFiClassEsp::RSSI()
{
    return WiFiDrv::getCurrentRSSI();
}

uint8_t WiFiClassEsp::encryptionType()
{
    return WiFiDrv::getCurrentEncryptionType();
}


int8_t WiFiClassEsp::scanNetworks()
{
	uint8_t attempts = 10;
	uint8_t numOfNetworks = 0;

	if (WiFiDrv::startScanNetworks() == WL_FAILURE)
		return WL_FAILURE;
 	do
 	{
 		delay(2000);
 		numOfNetworks = WiFiDrv::getScanNetworks();
 	}
	while (( numOfNetworks == 0)&&(--attempts>0));
	return numOfNetworks;
}

const char* WiFiClassEsp::SSID(uint8_t networkItem)
{
	return WiFiDrv::getSSIDNetoworks(networkItem);
}

int32_t WiFiClassEsp::RSSI(uint8_t networkItem)
{
	return WiFiDrv::getRSSINetoworks(networkItem);
}

uint8_t WiFiClassEsp::encryptionType(uint8_t networkItem)
{
    return WiFiDrv::getEncTypeNetowrks(networkItem);
}

uint8_t* WiFiClassEsp::BSSID(uint8_t networkItem, uint8_t* bssid)
{
	return WiFiDrv::getBSSIDNetowrks(networkItem, bssid);
}

uint8_t WiFiClassEsp::channel(uint8_t networkItem)
{
	return WiFiDrv::getChannelNetowrks(networkItem);
}

uint8_t WiFiClassEsp::status()
{
    return WiFiDrv::getConnectionStatus();
}

uint8_t WiFiClassEsp::reasonCode()
{
	return WiFiDrv::getReasonCode();
}

int WiFiClassEsp::hostByName(const char* aHostname, IPAddress& aResult)
{
	return WiFiDrv::getHostByName(aHostname, aResult);
}

unsigned long WiFiClassEsp::getTime()
{
	return WiFiDrv::getTime();
}

void WiFiClassEsp::lowPowerMode()
{
	WiFiDrv::setPowerMode(1);
}

void WiFiClassEsp::noLowPowerMode()
{
	WiFiDrv::setPowerMode(0);
}

int WiFiClassEsp::ping(const char* hostname, uint8_t ttl)
{
	IPAddress ip;

	if (!hostByName(hostname, ip)) {
		return WL_PING_UNKNOWN_HOST;
	}

	return ping(ip, ttl);
}

int WiFiClassEsp::ping(const String &hostname, uint8_t ttl)
{
	return ping(hostname.c_str(), ttl);
}

int WiFiClassEsp::ping(IPAddress host, uint8_t ttl)
{
	return WiFiDrv::ping(host, ttl);
}

void WiFiClassEsp::setTimeout(unsigned long timeout)
{
	_timeout = timeout;
}
WiFiClassEsp WiFiEsp;
