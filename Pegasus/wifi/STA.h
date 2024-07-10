#ifndef WIFI_EXAMPLE_H
#define WIFI_EXAMPLE_H

#include <hi_types_base.h>
#include "hi_wifi_api.h"

#define WIFI_IFNAME_MAX_SIZE 16
#define WIFI_SCAN_AP_LIMIT 64

hi_s32 example_get_match_network(hi_wifi_assoc_request *expected_bss);
hi_bool example_check_connect_status(hi_void);
hi_s32 example_sta_function(hi_void);

#endif // WIFI_EXAMPLE_H
