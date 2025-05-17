#include <iostream>
#include <string.h>
#include <unistd.h>

// #include <libusb-1.0/libusb.h>
// #include <vector>
// #include <thread>
// #include <chrono>
// #include <cstring>

#include "UacSource.h"


// Error handling macro for libusb
#define LIBUSB_CHECK(ret)                                                                    \
    if (ret < 0)                                                                             \
    {                                                                                        \
        std::cerr << "libusb error " << ret << " at " << __FILE__ << ":" << __LINE__ << ": " \
                  << libusb_error_name(ret) << std::endl;                                    \
        libusb_exit(NULL);                                                                   \
        return 1;                                                                            \
    }

// Function to print raw audio data as a hex string (for debugging)
void printHex(const unsigned char *data, int len)
{
    for (int i = 0; i < len; ++i)
    {
        printf("%02X ", data[i]);
        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}

int main(void)
{
    UacSourceImpl uacSource;
    uacSource.init();
    uacSource.test({
        .vendor_id = 0x12d1, 
        .product_id = 0x0010, 
        .interface_number = 1, 
        .endpoint_address = 0x82});
    uacSource.deinit();           // Deinitialize UacSource
    return 0;
}
