#pragma once

#include <exception>
#include <stdint.h>
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
        FrameTimeout,
        DeviceNotOpened
    };
    private:
        Type errorType;
        std::string errorMessage;
    public:
    UvcException(Type type, const std::string& message);
    const char* what() const noexcept override;
};

class UvcSource: public PullSource {
    public:
        struct OpenConfiguration: public Source::OpenConfiguration  {
            std::string dev_name;
        };
        enum io_method {
            IO_METHOD_READ,
            IO_METHOD_MMAP,
            IO_METHOD_USERPTR,
        };

        struct buffer {
            uint8_t* mmapBuffer {nullptr};
            size_t mmapSize {0};
            std::vector<uint8_t> buffer;
        };
        
    protected:
        int deviceFd = -1;
        std::vector<buffer> buffers;
        io_method io = IO_METHOD_MMAP;
        int force_format = 1;
        OpenConfiguration uvcConfig;
    private:
        int xioctl(int fh, int request, void *arg);        
        void init_read(size_t buffer_size);
        void init_mmap();
        void init_userp(size_t buffer_size);
        void init_device();
    public:
        UvcSource();
        virtual ~UvcSource();
        virtual void open(const UvcSource::OpenConfiguration & config);
        std::future<void> close() override;
        std::future<void> startProducing(const Source::ProducingConfiguration &config) override;
        std::future<void> stopProducing() override;
        auvc::ExpectedFrame readFrame() override;
        bool waitNextFrame() override;
        std::vector<auvc::FrameFormat> getSupportedFrameFormats() const override;
        [[nodiscard]] virtual auvc::ExpectedResolutions getSupportedResolutions() const override;
};
