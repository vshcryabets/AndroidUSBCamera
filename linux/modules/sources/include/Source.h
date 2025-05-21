#pragma once
#include <cstdint>
#include <stddef.h>
#include <chrono>
#include <functional>
#include <vector>
#include <string>


class SourceError : public std::exception {
    public:
        static const uint16_t SOURCE_ERROR_WRONG_CONFIG = 0x0001;
    private:
        uint16_t code;
        std::string message;
    public:
        SourceError(uint16_t code, std::string message) : code(code), message(message) {}
        const char* what() const noexcept override;
};

class Source {
    public:
        enum FrameFormat {
            YUV422,
            RGBA,
            RGB,
            RGBX,
            NONE
        };
        struct ConnectConfiguration {

        };
        struct CaptureConfiguration {
            uint32_t width;
            uint32_t height;
            float fps;
        };
        struct Frame {
            uint8_t* data {nullptr};
            size_t size {0};
            std::chrono::high_resolution_clock::time_point timestamp {std::chrono::high_resolution_clock::now()};
            FrameFormat format {NONE};
        };
    protected:
        ConnectConfiguration sourceConfig;
        CaptureConfiguration captureConfigutation;
    protected:
        uint32_t frameCounter {0};
    public:
        Source() {};
        virtual ~Source() = default;
        virtual Frame readFrame() = 0;
        virtual void open(const ConnectConfiguration &config) {
            this->sourceConfig = config;
        }
        virtual void close() = 0;
    
        virtual void startCapturing(const CaptureConfiguration &config) {
            this->captureConfigutation = config;
        }
        virtual void stopCapturing() = 0;
    
        const ConnectConfiguration getSourceConfiguration();
        const CaptureConfiguration getCaptureConfiguration();
        virtual bool waitNextFrame() = 0;
        virtual std::vector<FrameFormat> getSupportedFrameFormats() = 0;
};
    