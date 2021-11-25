#ifndef SNTP_PORT_H
#define SNTP_PORT_H

#include "lwip/err.h"
#include "lwip/ip_addr.h"

typedef void (*__dns_found_callback)(const char *name, const ip_addr_t *ipaddr, void *arg);

err_t __dns_gethostbyname(const char *hostname, ip_addr_t *addr, __dns_found_callback found, void *arg);

#endif // SNTP_PORT_H
