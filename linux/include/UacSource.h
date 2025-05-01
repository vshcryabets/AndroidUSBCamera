#pragma once

#include <string>
#include <libusb-1.0/libusb.h>
#include <fstream> // For writing to a file 

class UacSource {
public:
    class Error : public std::exception {
    private:
        std::string message;
    public:
        Error(const char* msg) : message(msg) {}
        Error(const std::string& msg) : message(msg) {}
        const char* what() const noexcept {
            return message.c_str();
        }
    };

    struct StartConfiguration {
        const uint16_t vendor_id;
        const uint16_t product_id;
        const int interface_number;
        const int endpoint_address;
    };
public:
    virtual ~UacSource() = default;
    virtual void init() = 0;
    virtual void deinit() = 0;
    virtual void test(const StartConfiguration &cfg) = 0;
};

class UacSourceImpl : public UacSource {
public:

protected:
    libusb_context* ctx = nullptr;

    libusb_device_handle *handle;
    int endpoint;
    std::ofstream outputFile;
protected:
    static void entryPoint(UacSourceImpl* uacSource);
    libusb_device_handle *findAudioDevice(const StartConfiguration &cfg);
    void handleAudioData();
public:
    UacSourceImpl();
    ~UacSourceImpl() override;
    void init() override;
    void deinit() override;
    void test(const StartConfiguration &cfg) override;
};