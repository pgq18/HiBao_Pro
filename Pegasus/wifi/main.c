#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "STA.h"
#include "wifi_connect.h"

int main(void)
{
    app_main();  // Initialize Wi-Fi driver

    // Attempt to connect to Wi-Fi network as STA
    if (example_sta_function() == HISI_OK) {
        printf("Wi-Fi connected and IP address obtained successfully.\n");
    } else {
        printf("Wi-Fi connection failed.\n");
    }

    return 0;
}
