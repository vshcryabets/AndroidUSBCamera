#pragma once
#include "DataTypes.h"
#include <vector>
#include <string>
#include <map>
#include <future>
#include <expected>

namespace auvc {
    std::future<void> completed();

    enum class SourceErrorCode : uint16_t {
        SOURCE_ERROR_WRONG_CONFIG,
        SOURCE_ERROR_CAPTURE_NOT_STARTED,
        SOURCE_ERROR_NOT_OPENED,
        SOURCE_ERROR_READ_AGAIN,
        SOURCE_FRAME_NOT_AVAILABLE,
    };

    class SourceError : public std::exception {
        public:
        private:
            SourceErrorCode code;
            std::string message;
        public:
            SourceError(SourceErrorCode code, const std::string &message) : code(code), message(message) {}
            const char* what() const noexcept override;
    };

    struct Resolution {
        uint8_t id;
        uint16_t width;
        uint16_t height;
        std::vector<float> fps;
    };

    using ExpectedResolutions = std::expected<std::map<uint16_t, std::vector<Resolution>>, SourceError>;
    using ExpectedFrame = std::expected<auvc::Frame, auvc::SourceError>;
}



class Source {
public:
    struct OpenConfiguration {

    };
    struct ProducingConfiguration {
        uint32_t width {0};
        uint32_t height {0};
        float fps {0.0f};
    };
    
protected:
    OpenConfiguration sourceConfig;
    ProducingConfiguration captureConfiguration;
protected:
    uint32_t frameCounter {0};
public:
    Source() {
        sourceConfig = OpenConfiguration();
        captureConfiguration = ProducingConfiguration();
    };
    virtual ~Source() = default;
    // open-close
    virtual void open(const OpenConfiguration &config) {
        this->sourceConfig = config;
    }
    [[nodiscard]] const OpenConfiguration getOpenConfiguration() const;
    [[nodiscard]] virtual std::future<void> close() = 0;
    // producing
    [[nodiscard]] const ProducingConfiguration getProducingConfiguration() const;
    [[nodiscard]] virtual std::future<void> startProducing(const ProducingConfiguration &config) {
        this->captureConfiguration = config;
        return auvc::completed();
    }
    [[nodiscard]] virtual std::future<void> stopProducing() {
        this->captureConfiguration = ProducingConfiguration();
        return auvc::completed();
    };
    [[nodiscard]] virtual bool isReadyForProducing() const;

    [[nodiscard]] virtual auvc::ExpectedResolutions getSupportedResolutions() const = 0;
    [[nodiscard]] virtual std::vector<auvc::FrameFormat> getSupportedFrameFormats() const = 0;
    [[nodiscard]] virtual bool isPullSource() const = 0;
    [[nodiscard]] virtual bool isPushSource() const = 0;
};

class PullSource : public Source {
public:
    PullSource() : Source() {}
    virtual ~PullSource() = default;
    virtual auvc::ExpectedFrame readFrame() = 0;
    virtual bool waitNextFrame() = 0;
    [[nodiscard]] bool isPullSource() const override {
        return true;
    }
    [[nodiscard]] bool isPushSource() const override {
        return false;
    }
};
