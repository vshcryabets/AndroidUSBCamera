#pragma once

#include <exception>
#include <stdint.h>
#include <chrono>
#include "Source.h"

class UvcException : public std::exception {
    public:
    enum Type {
        Other,
        WrongDevice,
        CantOpenDevice,
        CantCloseDevice,
        IoCtlError,
        MmapError,
        ReadError,
        FrameTimeout
    };
    private:
        Type errorType;
    public:
    UvcException(Type type) : errorType(type) {}
    const char* what() const noexcept override;
};

class UvcCamera: public Source {
    public:
        struct Configuration  {
            const char* dev_name;
            Source::ConnectConfiguration source;
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
    
    protected:
        int fd = -1;
        struct buffer *buffers;
        unsigned int n_buffers;
        enum io_method io = IO_METHOD_MMAP;
        int force_format = 1;
        Configuration uvcConfig;
    private:
        int xioctl(int fh, int request, void *arg);        
        void init_read(unsigned int buffer_size);
        void init_mmap();
        void init_userp(unsigned int buffer_size);
        void init_device();
    public:
        UvcCamera();
        virtual ~UvcCamera();
        virtual void open(const UvcCamera::Configuration & config);
        void close() override;
        void startCapturing(const Source::CaptureConfiguration &config) override;
        void stopCapturing() override;
        Frame readFrame() override;
        bool waitNextFrame() override;
        std::vector<Source::FrameFormat> getSupportedFrameFormats() override;
    };