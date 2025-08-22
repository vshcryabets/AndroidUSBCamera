#pragma once
#include "DataTypes.h"
#include <functional>
#include <vector>
#include <string>
#include <map>

class SourceError : public std::exception {
    public:
        static const uint16_t SOURCE_ERROR_WRONG_CONFIG = 0x0001;
        static const uint16_t SOURCE_ERROR_CAPTURE_NOT_STARTED = 0x0002;
    private:
        uint16_t code;
        std::string message;
    public:
        SourceError(uint16_t code, const std::string &message) : code(code), message(message) {}
        const char* what() const noexcept override;
};

class Source {
public:
    struct Resolution {
        const uint8_t id;
        const uint16_t width;
        const uint16_t height;
        std::vector<float> fps;
    };
    struct OpenConfiguration {

    };
    struct CaptureConfiguration {
        uint32_t width;
        uint32_t height;
        float fps;
    };
    
protected:
    OpenConfiguration sourceConfig;
    CaptureConfiguration captureConfiguration;
protected:
    uint32_t frameCounter {0};
public:
    Source() {
        sourceConfig = OpenConfiguration();
        captureConfiguration = CaptureConfiguration {0, 0, 0.0f};
    };
    virtual ~Source() = default;
    virtual void open(const OpenConfiguration &config) {
        this->sourceConfig = config;
    }
    const OpenConfiguration getOpenConfiguration() const;
    const CaptureConfiguration getCaptureConfiguration() const;


    virtual void startCapturing(const CaptureConfiguration &config) {
        this->captureConfiguration = config;
    }
    virtual bool isReadyForCapture() const;
    virtual void stopCapturing() = 0;
    virtual void close() = 0;
    virtual std::map<uint16_t, std::vector<Resolution>> getSupportedResolutions() const = 0;
    virtual std::vector<auvc::FrameFormat> getSupportedFrameFormats() const = 0;
    virtual bool isPullSource() const = 0;
    virtual bool isPushSource() const = 0;
};

class PullSource : public Source {
public:
    PullSource() : Source() {}
    virtual ~PullSource() = default;
    virtual auvc::Frame readFrame() = 0;
    virtual bool waitNextFrame() = 0;
    [[nodiscard]] bool isPullSource() const override {;
        return true;
    }
    [[nodiscard]] bool isPushSource() const override {
        return false;
    }
};

class PushSource : public Source {
public:
    using FrameCallback = std::function<void(const auvc::Frame &frame)>;
    struct OpenConfiguration: public Source::OpenConfiguration {
        FrameCallback frameCallback;
    };
protected:
    FrameCallback frameCallback;
public:
    PushSource() : Source() {}
    virtual ~PushSource() = default;
    virtual void open(const OpenConfiguration &config) {
        Source::open(config);
        this->frameCallback = config.frameCallback;
    }
    virtual void pushFrame(const auvc::Frame &frame) {
        if (frameCallback) {
            frameCallback(frame);
        }
    }
    [[nodiscard]] bool isPullSource() const override {;
        return false;
    }
    [[nodiscard]] bool isPushSource() const override {
        return true;
    }
};
