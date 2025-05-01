#pragma once

#include <exception>
#include <stdint.h>
#include <chrono>
#include <functional>

class UvcException : public std::exception {
    public:
    enum Type {
        Other,
        WrongDevice,
        CantOpenDevice,
        CantCloseDevice,
        IoCtlError,
        MmapError,
        ReadError
    };
    private:
        Type errorType;
    public:
    UvcException(Type type) : errorType(type) {

    }
};

class UvcCamera {
    public:
        struct Frame {
            uint8_t* data {nullptr};
            size_t size {0};
            std::chrono::high_resolution_clock::time_point timestamp {std::chrono::high_resolution_clock::now()};
        };
        enum io_method {
            IO_METHOD_READ,
            IO_METHOD_MMAP,
            IO_METHOD_USERPTR,
        };
    
        struct buffer {
            void   *start;
            size_t  length;
        };
    
        uint32_t width {0};
        uint32_t height {0};
    private:
        int fd = -1;
        struct buffer *buffers;
        unsigned int n_buffers;
        enum io_method io = IO_METHOD_MMAP;
        int force_format = 1;
    
    private:
        int xioctl(int fh, int request, void *arg);        
        void init_read(unsigned int buffer_size);
        void init_mmap(const char* dev_name);
        void init_userp(unsigned int buffer_size, const char* dev_name);
    public:
        UvcCamera();
        virtual ~UvcCamera();
        void open_device(const char *dev_name);
        void close_device();
        void init_device(const char *dev_name);
        void uninit_device(void);
        void start_capturing(void);
        void stop_capturing(void);
        Frame readFrame(std::function<void(Frame)> processImageCallback = nullptr);
        int getFd();
    };