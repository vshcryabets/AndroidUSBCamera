#pragma once
#include <cstdint>
#include <cstddef>
#include <chrono>
#include <functional>
#include <vector>
#include <string>
#include <map>

class SourceError : public std::exception {
    public:
        static const uint16_t SOURCE_ERROR_WRONG_CONFIG = 0x0001;
    private:
        uint16_t code;
        std::string message;
    public:
        SourceError(uint16_t code, const std::string &message) : code(code), message(message) {}
        const char* what() const noexcept override;
};

class Source {
    public:
        enum FrameFormat {
            YUYV,
            RGBA,
            RGB,
            RGBX,
            YUV420P,
            NONE
        };
        struct Resolution {
            const uint8_t id;
            const uint16_t width;
            const uint16_t height;
            std::vector<float> fps;
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
        virtual std::map<uint16_t, std::vector<Resolution>> getSupportedResolutions() = 0;
        virtual std::vector<FrameFormat> getSupportedFrameFormats() = 0;
};

class PullSource : public Source {
    public:
        using FrameCallback = std::function<void(const Frame &frame)>;
    protected:
        FrameCallback frameCallback;
    public:
        PullSource() : Source() {}
        virtual ~PullSource() = default;
        virtual Frame readFrame() = 0;
        virtual bool waitNextFrame() = 0;
};

class PushSource : public Source {
    public:
        using FrameCallback = std::function<void(const Frame &frame)>;
    protected:
        FrameCallback frameCallback;
    public:
        PushSource() : Source() {}
        virtual ~PushSource() = default;
        virtual void setFrameCallback(const FrameCallback &callback) {
            this->frameCallback = callback;
        }
        virtual void pushFrame(const Frame &frame) {
            if (frameCallback) {
                frameCallback(frame);
            }
        }
};