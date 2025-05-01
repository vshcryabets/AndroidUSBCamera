#include <iostream>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <libusb-1.0/libusb.h>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <fstream> // For writing to a file

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

// Function to find the USB audio device
libusb_device_handle *findAudioDevice(libusb_context *ctx, uint16_t vendor_id,
                                      uint16_t product_id)
{
    libusb_device **dev_list;
    ssize_t dev_count = libusb_get_device_list(ctx, &dev_list);
    if (dev_count < 0)
    {
        std::cerr << "Failed to get device list: " << libusb_error_name(dev_count)
                  << std::endl;
        return nullptr;
    }

    libusb_device_handle *handle = nullptr;
    for (ssize_t i = 0; i < dev_count; ++i)
    {
        libusb_device_descriptor desc;
        int ret = libusb_get_device_descriptor(dev_list[i], &desc);
        if (ret < 0)
        {
            std::cerr << "Failed to get device descriptor: "
                      << libusb_error_name(ret) << std::endl;
            continue; // Skip to the next device
        }

        if (desc.idVendor == vendor_id && desc.idProduct == product_id)
        {
            ret = libusb_open(dev_list[i], &handle);
            if (ret < 0)
            {
                std::cerr << "Failed to open device: "
                          << libusb_error_name(ret) << std::endl;
                handle = nullptr; // Ensure handle is null
            }
            else
            {
                std::cout << "Found and opened audio device: Vendor ID = "
                          << vendor_id << ", Product ID = " << product_id << std::endl;
                break; // Exit the loop when device is found and opened.
            }
        }
    }
    libusb_free_device_list(dev_list, 1); // Free the device list
    return handle;
}

// Function to handle audio data transfer
void handleAudioData(libusb_device_handle *handle, int endpoint,
                     std::ofstream &outputFile)
{                               // Pass the file stream
    unsigned char buffer[4096]; // Adjust buffer size as needed
    int transferred = 0;
    int ret = 0;

    while (true)
    {
        ret = libusb_bulk_transfer(handle, endpoint, buffer, sizeof(buffer),
                                   &transferred, 0); // No timeout
        if (ret == LIBUSB_ERROR_TIMEOUT)
        {
            // Handle timeout if needed, or just continue
            std::cerr << "Timeout occurred" << std::endl;
            continue;
        }
        else if (ret < 0 && ret != LIBUSB_ERROR_TIMEOUT)
        {
            std::cerr << "Error in bulk transfer: " << libusb_error_name(ret)
                      << std::endl;
            break; // Exit loop on error
        }
        else if (ret == 0)
        {
            // Successful transfer
            if (transferred > 0)
            {
                // Do something with the audio data.  For example:
                std::cout << "Received " << transferred << " bytes of audio data."
                          << std::endl;
                // printHex(buffer, transferred); // Print hex data

                // Write the data to the output file
                outputFile.write(reinterpret_cast<char *>(buffer), transferred);
                if (outputFile.fail())
                {
                    std::cerr << "Error writing to file!" << std::endl;
                    break; // Stop loop
                }
            }
        }
        // Add a small delay
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main(void)
{
    libusb_context* ctx = NULL;
    int ret = libusb_init(&ctx);
    LIBUSB_CHECK(ret);

    // Set verbosity level for debugging
    libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);

    // Replace with your device's actual vendor and product IDs.
    uint16_t vendor_id = 0x07d0; // Example: Rode Vendor ID
    uint16_t product_id = 0x2002; // Example: Rode NT-USB+ Product ID

    libusb_device_handle* handle = findAudioDevice(ctx, vendor_id, product_id);
    if (!handle) {
        std::cerr << "Audio device not found or could not be opened." << std::endl;
        libusb_exit(ctx);
        return 1;
    }

    // Claim the audio interface.  The interface number (0) may need to be
    // changed.  You'll need to determine the correct interface number
    // for your device, possibly by using lsusb -v or examining the device
    // with a USB analyzer.
    int interface_number = 0; //  <-- IMPORTANT:  May need to change!
    ret = libusb_claim_interface(handle, interface_number);
    if (ret < 0) {
        std::cerr << "Failed to claim interface " << interface_number << ": "
                  << libusb_error_name(ret) << std::endl;
        libusb_close(handle);
        libusb_exit(ctx);
        return 1;
    }
    std::cout << "Claimed interface " << interface_number << std::endl;

    // Find the endpoint address.  This is crucial and device-specific.
    // You *must* determine the correct endpoint address for your device
    // using a tool like `lsusb -v`.  Look for an endpoint with
    // `Direction = IN` and `Transfer Type = Bulk` or `Isochronous`.
    int endpoint_address = 0x82; // Example:  <-- IMPORTANT: May need to change!

    // Open a file to save the captured audio data
    std::ofstream outputFile("captured_audio.raw", std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Could not open output file!" << std::endl;
        libusb_release_interface(handle, interface_number);
        libusb_close(handle);
        libusb_exit(ctx);
        return 1;
    }

    // Start the audio data handling thread
    std::thread audioThread(handleAudioData, handle, endpoint_address,
                             std::ref(outputFile)); // Pass the file stream

    // Keep the main thread running to prevent the program from exiting
    // prematurely.  You might want to add a signal handler (e.g., for Ctrl+C)
    // to stop the recording and clean up resources.
    std::cout << "Capturing audio... Press Ctrl+C to stop." << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // The following code will never be reached in the current loop.
    // You would need to modify the loop (e.g., with a signal handler)
    // to break out of it and execute this cleanup.
    audioThread.join();             // Wait for the thread to finish
    libusb_release_interface(handle, interface_number); // Release interface
    libusb_close(handle);           // Close the device
    libusb_exit(ctx);               // Exit libusb

    outputFile.close(); // Close the file
    return 0;
}
