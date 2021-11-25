/*
 * Copyright (c) 2020, HiHope Community.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "sntp_port.h"
#include "lwip/apps/sntp_opts.h"
#include "lwip/pbuf.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#if SNTP_SERVER_DNS

#if 0
typedef struct {
  void* arg;
  __dns_found_callback found;
} hisi_dns_found_callback_data;

// for hisi: typedef void (*dns_found_callback)(const char *name, const ip_addr_t *ipaddr, u32_t count, void *arg);
void hisi_dns_found_callback(const char *name, const ip_addr_t *ipaddr, u32_t count, void *callback_arg)
{
  (void) count;
  hisi_dns_found_callback_data* data = (hisi_dns_found_callback_data*) callback_arg;
  if (data && data->found) {
    data->found(name, ipaddr, data->arg);
  }
  printf("DNS for %s done, \r\n", name);
}

err_t __dns_gethostbyname(const char *hostname, ip_addr_t *addr, __dns_found_callback found, void *arg)
{
  static u32_t count = 0;
  static hisi_dns_found_callback_data data = {0};
  data.arg = arg;
  data.found = found;
  printf("DNS for name %s start\r\n", hostname);
  return dns_gethostbyname_addrtype(hostname, addr, &count, hisi_dns_found_callback, &data, LWIP_DNS_ADDRTYPE_IPV4);
}
#else

err_t __dns_gethostbyname(const char *hostname, ip_addr_t *addr, __dns_found_callback found, void *arg)
{
  struct hostent *result = NULL;
  char addrString[20] = {0};
  int i = 0;
  result = gethostbyname(hostname);
  if (result) {
    while (1) {
      ip_addr_t* ipAddr = *(((ip_addr_t **)result->h_addr_list) + i);
      if (ipAddr == NULL) {
        break;
      }
      inet_ntoa_r(*ipAddr, addrString, 20);
      printf("dns call for %s, returns %s\n", hostname, addrString);
      if (found && addr) {
        memcpy(addr, ipAddr, sizeof(*ipAddr));
        found(hostname, addr, arg);
      }
    }
  } else {
    printf("dns call failed\n");
  }
  printf("DNS for %s end!\r\n", hostname);
  return 0;
}

#endif

#endif

#if defined(LWIP_DEBUG) && !defined(sntp_format_time)
#include <time.h>

char *ctime(const time_t *t)
{
  struct tm tm = {0};
  localtime_r(t, &tm);

  static char buff[32];
  snprintf(buff, sizeof(buff), "%04d-%02d-%02d %02d:%02d:%02d",
    1900 + tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  return buff;
}

#endif
