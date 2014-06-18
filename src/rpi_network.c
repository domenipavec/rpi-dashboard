/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Raspberry Pi Dasboard
 *  =====================
 *  Copyright 2014 Domen Ipavec <domen.ipavec@z-v.si>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "rpi_network.h"

#include "rpi_modules.h"

#include "packages/json/json.h"

#include <dirent.h>
#include <sys/ioctl.h>
#include <net/if.h>


static const char *dir_name = "/sys/class/net/";

#define RX "/statistics/rx_"
#define TX "/statistics/tx_"

#define IPV6_ADDR_GLOBAL        0x0000U
#define IPV6_ADDR_LOOPBACK      0x0010U
#define IPV6_ADDR_LINKLOCAL     0x0020U
#define IPV6_ADDR_SITELOCAL     0x0040U
#define IPV6_ADDR_COMPATv4      0x0080U

static double rpi_network_read_file(const char *if_name,
                                    const char *rxtx,
                                    const char *bytes_packets)
{
    FILE *f;
    int ret;
    double value;
    char path[50];
    
    strcpy(path, dir_name);
    strcat(path, if_name);
    strcat(path, rxtx);
    strcat(path, bytes_packets);

    f = fopen(path, "r");
    if (f == NULL) {
        return -1.0;
    }

    ret = fscanf(f, " %lf", &value);
    if (ret != 1) {
        return -1.0;
    }
    
    fclose(f);
    
    return value;
}

static int check_iface(const char *qsiface, char *iface) {
    int i, last;
    
    if (qsiface == NULL) {
        return 0;
    }
    
    for (i = 0, last = 0; ; ++i) {
        if (qsiface[i] != '\0' && qsiface[i] != '|') {
            continue;
        }
        
        if (strlen(iface) == i - last) {
            if (strncmp(iface, qsiface + last, i - last) == 0) {
                return 0;
            }
        }
        
        if (qsiface[i] == '\0') {
            break;
        }
        last = i + 1;
    }
    
    return -1;
}

static const char * get_qsiface(duda_request_t *dr) {
    if (dr == NULL) {
        return "";
    }
    return qs->get(dr, "iface");
}

static json_t *rpi_network_get(duda_request_t *dr, const char *bytes_packets)
{
    DIR *d;
    struct dirent *de;
    json_t *object;
    json_t *rx_object;
    json_t *tx_object;
    json_t *total_object;
    double rx_value, tx_value;
    const char *qsiface;

    object = json->create_object();
    
    d = opendir(dir_name);
    if (d == NULL) {
        return object;
    }
    
    rx_object = json->create_object();
    json->add_to_object(object, "rx", rx_object);
    
    tx_object = json->create_object();
    json->add_to_object(object, "tx", tx_object);

    total_object = json->create_object();
    json->add_to_object(object, "total", total_object);
    
    qsiface = get_qsiface(dr);

    while (NULL != (de = readdir(d))) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
            continue;
        }
        
        if (check_iface(qsiface, de->d_name) == -1) {
            continue;
        }
        
        rx_value = rpi_network_read_file(de->d_name, RX, bytes_packets);
        tx_value = rpi_network_read_file(de->d_name, TX, bytes_packets);
        
        json->add_to_object(rx_object, de->d_name, json->create_number(rx_value));
        json->add_to_object(tx_object, de->d_name, json->create_number(tx_value));
        json->add_to_object(total_object, de->d_name, json->create_number(rx_value + tx_value));
    }
    closedir(d);
    
    return object;
}

static void rpi_network_parse_ioctl(const char *ifname, json_t *object)
{
    int sock;
    struct ifreq ifr;
    struct sockaddr_in *ipaddr;
    json_t *ip_object;
    char address[INET_ADDRSTRLEN];
    char mac[18];
    size_t ifnamelen;
    
    /* copy ifname to ifr object */
    ifnamelen = strlen(ifname);
    if (ifnamelen >= sizeof(ifr.ifr_name)) {
        return ;
    }
    memcpy(ifr.ifr_name, ifname, ifnamelen);
    ifr.ifr_name[ifnamelen] = '\0';

    /* open socket */
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        return;
    }
    
    /* process mac */
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) != -1) {
        sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
                (unsigned char)ifr.ifr_hwaddr.sa_data[0],
                (unsigned char)ifr.ifr_hwaddr.sa_data[1],
                (unsigned char)ifr.ifr_hwaddr.sa_data[2],
                (unsigned char)ifr.ifr_hwaddr.sa_data[3],
                (unsigned char)ifr.ifr_hwaddr.sa_data[4],
                (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
        json->add_to_object(object, "mac", json->create_string(mac));
    }
    
    /* process mtu */
    if (ioctl(sock, SIOCGIFMTU, &ifr) != -1) {
        json->add_to_object(object, "mtu", json->create_number((double)ifr.ifr_mtu));
    }
    
    /* die if cannot get address */
    if (ioctl(sock, SIOCGIFADDR, &ifr) == -1) {
        close(sock);
        return;
    }
    
    ip_object = json->create_object();
    json->add_to_object(object, "ip", ip_object);
    
    /* process ip */
    ipaddr = (struct sockaddr_in *)&ifr.ifr_addr;
    if (inet_ntop(AF_INET, &ipaddr->sin_addr, address, sizeof(address)) != NULL) {
        json->add_to_object(ip_object, "address", json->create_string(address));
    }
    
    /* try to get broadcast */
    if (ioctl(sock, SIOCGIFBRDADDR, &ifr) != -1) {
        ipaddr = (struct sockaddr_in *)&ifr.ifr_broadaddr;
        if (inet_ntop(AF_INET, &ipaddr->sin_addr, address, sizeof(address)) != NULL) {
            json->add_to_object(ip_object, "broadcast", json->create_string(address));
        }
    }
    
    /* try to get mask */
    if (ioctl(sock, SIOCGIFNETMASK, &ifr) != -1) {
        ipaddr = (struct sockaddr_in *)&ifr.ifr_netmask;
        if (inet_ntop(AF_INET, &ipaddr->sin_addr, address, sizeof(address)) != NULL) {
            json->add_to_object(ip_object, "netmask", json->create_string(address));
        }
    }
    
    close(sock);
}

static void rpi_network_parse_inet6(json_t *object) {
    FILE *f;
    int scope, prefix;
    json_t *ipv6_array;
    json_t *interface_object;
    json_t *ipv6_object;
    json_t *scope_json;
    unsigned char ipv6[16];
    char dname[IFNAMSIZ];
    char address[INET6_ADDRSTRLEN];

    f = fopen("/proc/net/if_inet6", "r");
    if (f == NULL) {
        return;
    }
    
    while (19 == fscanf(f, 
                        " %2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx %*x %x %x %*x %s",
                        &ipv6[0], 
                        &ipv6[1], 
                        &ipv6[2], 
                        &ipv6[3], 
                        &ipv6[4], 
                        &ipv6[5], 
                        &ipv6[6], 
                        &ipv6[7], 
                        &ipv6[8], 
                        &ipv6[9], 
                        &ipv6[10], 
                        &ipv6[11], 
                        &ipv6[12], 
                        &ipv6[13], 
                        &ipv6[14], 
                        &ipv6[15], 
                        &prefix, 
                        &scope, 
                        dname)) {
        
        interface_object = json->get_object_item(object, dname);
        if (interface_object == NULL) {
            continue;
        }

        if (inet_ntop(AF_INET6, ipv6, address, sizeof(address)) == NULL) {
            continue;
        }
        
        ipv6_array = json->get_object_item(interface_object, "ipv6");
        if (ipv6_array == NULL) {
            ipv6_array = json->create_array();
            json->add_to_object(interface_object, "ipv6", ipv6_array);
        }
        
        ipv6_object = json->create_object();
        json->add_to_array(ipv6_array, ipv6_object);
        
        json->add_to_object(ipv6_object, "address", json->create_string(address));
        json->add_to_object(ipv6_object, "prefix", json->create_number((double)prefix));

        switch (scope) {
        case IPV6_ADDR_GLOBAL:
            scope_json = json->create_string("Global");
            break;
        case IPV6_ADDR_LINKLOCAL:
            scope_json = json->create_string("Link");
            break;
        case IPV6_ADDR_SITELOCAL:
            scope_json = json->create_string("Site");
            break;
        case IPV6_ADDR_COMPATv4:
            scope_json = json->create_string("Compat");
            break;
        case IPV6_ADDR_LOOPBACK:
            scope_json = json->create_string("Host");
            break;
        default:
            scope_json = json->create_string("Unknown");
        }
        json->add_to_object(ipv6_object, "scope", scope_json);
    }
    
    fclose(f);
}

json_t * rpi_network_get_list(duda_request_t *dr)
{
    DIR *d;
    struct dirent *de;
    json_t *object;
    json_t *interface_object;
    const char *qsiface;

    object = json->create_object();
    
    d = opendir(dir_name);
    if (d == NULL) {
        return object;
    }

    qsiface = get_qsiface(dr);

    while (NULL != (de = readdir(d))) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
            continue;
        }
        
        if (check_iface(qsiface, de->d_name) == -1) {
            continue;
        }
        
        interface_object = json->create_object();
        json->add_to_object(object, de->d_name, interface_object);
        
        rpi_network_parse_ioctl(de->d_name, interface_object);
    }
    closedir(d);
    
    rpi_network_parse_inet6(object);
    
    return object;
}

json_t *rpi_network_get_bytes(duda_request_t *dr)
{
    return rpi_network_get(dr, "bytes");
}

json_t *rpi_network_get_packets(duda_request_t *dr)
{
    return rpi_network_get(dr, "packets");
}

/* register and initialize module */
void rpi_network_init(void)
{
    rpi_module_t *module = rpi_modules_module_init("network", NULL);
    
    rpi_modules_value_init("packets", rpi_network_get_packets, &(module->values_head.values));
    rpi_modules_value_init("bytes", rpi_network_get_bytes, &(module->values_head.values));
    rpi_modules_value_init("list", rpi_network_get_list, &(module->values_head.values));
}