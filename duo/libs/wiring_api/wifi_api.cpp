#include "spark_wiring_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace spark;

WLanConfig* wifi_wifiConfig(void)
{
    return WiFi.wifi_config();
}

void wifi_macAddress(uint8_t *mac)
{
	WiFi.macAddress(mac);
}

void wifi_localIP(uint8_t *local_ip)
{
    IPAddress IP;
    IP = WiFi.localIP();
    local_ip[0] = IP[0];
    local_ip[1] = IP[1];
    local_ip[2] = IP[2];
    local_ip[3] = IP[3];
}

void wifi_subnetMask(uint8_t *net_mask)
{
    IPAddress IP;
    IP = WiFi.subnetMask();
    net_mask[0] = IP[0];
    net_mask[1] = IP[1];
    net_mask[2] = IP[2];
    net_mask[3] = IP[3];
}

void wifi_gatewayIP(uint8_t *gateway_ip)
{
    IPAddress IP;
    IP = WiFi.gatewayIP();
    gateway_ip[0] = IP[0];
    gateway_ip[1] = IP[1];
    gateway_ip[2] = IP[2];
    gateway_ip[3] = IP[3];
}

void wifi_dnsServerIP(uint8_t *dns_ip)
{
    IPAddress IP;
    IP = WiFi.dnsServerIP();
    dns_ip[0] = IP[0];
    dns_ip[1] = IP[1];
    dns_ip[2] = IP[2];
    dns_ip[3] = IP[3];
}

void wifi_dhcpServerIP(uint8_t *dhcp_ip)
{
    IPAddress IP;
    IP = WiFi.dhcpServerIP();
    dhcp_ip[0] = IP[0];
    dhcp_ip[1] = IP[1];
    dhcp_ip[2] = IP[2];
    dhcp_ip[3] = IP[3];
}

void wifi_BSSID(uint8_t* bssid)
{
	WiFi.BSSID(bssid);
}

const char *wifi_SSID(void)
{
    return WiFi.SSID();
}

int8_t wifi_RSSI(void)
{
    return WiFi.RSSI();
}

uint32_t wifi_ping(uint8_t *ip, uint8_t nTries)
{
	IPAddress remoteIP(ip[0], ip[1], ip[2], ip[3]);
    return WiFi.ping(remoteIP, nTries);
}

void wifi_connect(void)
{
	WiFi.connect();
}

void wifi_disconnect(void)
{
	WiFi.disconnect();
}

bool wifi_isConnecting(void)
{
    return WiFi.connecting();
}

bool wifi_isReady(void)
{
    return WiFi.ready();
}

void wifi_on(void)
{
	WiFi.on();
}

void wifi_off(void)
{
	WiFi.off();
}

void wifi_startListen(void)
{
	WiFi.listen();
}

void wifi_stopListen(void)
{
	WiFi.listen(false);
}

bool wifi_isListening(void)
{
    return WiFi.listening();
}

void wifi_setCredentials(const char *ssid, const char *password, unsigned long security, unsigned long cipher)
{
    WiFi.setCredentials(ssid, password, security, cipher);
}

bool wifi_hasCredentials(void)
{
    return WiFi.hasCredentials();
}

bool wifi_clearCredentials(void)
{
    return WiFi.clearCredentials();
}

void wifi_resolve(const char* name, uint8_t *ip)
{
    IPAddress IP;
    IP = WiFi.resolve(name);
    ip[0] = IP[0];
    ip[1] = IP[1];
    ip[2] = IP[2];
    ip[3] = IP[3];
}

void wifi_setStaticIP(uint8_t *host_ip, uint8_t *netmask_ip, uint8_t *gateway_ip, uint8_t *dns_ip)
{
    IPAddress host(host_ip[0],host_ip[1],host_ip[2],host_ip[3]);
    IPAddress netmask(netmask_ip[0],netmask_ip[1],netmask_ip[2],netmask_ip[3]);
    IPAddress gateway(gateway_ip[0],gateway_ip[1],gateway_ip[2],gateway_ip[3]);
    IPAddress dns(dns_ip[0],dns_ip[1],dns_ip[2],dns_ip[3]);

    IPAddress &_host=host;
    IPAddress &_netmask=netmask;
    IPAddress &_gateway=gateway;
    IPAddress &_dns=dns;

    WiFi.setStaticIP(_host, _netmask, _gateway, _dns);
}

void wifi_useStaticIP(void)
{
	WiFi.useStaticIP();
}

void wifi_useDynamicIP(void)
{
	WiFi.useDynamicIP();
}

int wifi_scan(WiFiAccessPoint* results, size_t result_count)
{
    return WiFi.scan(results, result_count);
}

int wifi_getCredentials(WiFiAccessPoint* results, size_t result_count)
{
    return WiFi.getCredentials(results, result_count);
}


#ifdef __cplusplus
}
#endif
