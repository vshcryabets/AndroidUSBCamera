#pragma once
#include "DataTypes.h"
#include <vector>
#include <string>
#include <map>
#include <future>
#include <expected>
#include <queue>

#include "Errors.h"

namespace auvc {
    std::future<void> completed();

    struct ProducingConfiguration {
        uint8_t id;
        uint16_t width;
        uint16_t height;
        float fps;
        FrameFormat frameFormat;
    };

    using ExpectedResolutions = std::expected<std::map<uint16_t, std::vector<ProducingConfiguration>>, SourceError>;
    using ExpectedFrame = std::expected<auvc::Frame, auvc::SourceError>;

    class Source;

    class SourceConfiguration {
        private:
            std::string tag;
        public:
            SourceConfiguration() : tag("") {}
            SourceConfiguration(const std::string &tag) : tag(tag) {}
            [[nodiscard]] const std::string &getTag() const { return tag; }
    };

    class Consumer {
    protected:
        std::shared_ptr<Source> source;
    public:
        virtual ~Consumer() = default;
        virtual void consume(const Frame& frame) = 0;
        virtual ConsumerError attachTo(std::shared_ptr<Source> source);
        [[nodiscard]] std::shared_ptr<Source> getAttachedSource() const {
            return source;
        }
        virtual ConsumerError openChain(std::queue<SourceConfiguration> &openConfigurations);
        virtual ConsumerError startChain(std::queue<ProducingConfiguration> &openConfigurations);
        virtual ConsumerError stopChain();
        virtual ConsumerError closeChain();
    };

    class Source {  
    protected:
        SourceConfiguration sourceConfig;
        ProducingConfiguration captureConfiguration;
    protected:
        uint32_t frameCounter {0};
    public:
        Source() {
            sourceConfig = SourceConfiguration();
            captureConfiguration = ProducingConfiguration();
        };
        virtual ~Source() = default;
        // open-close
        virtual void open(const SourceConfiguration &config) {
            this->sourceConfig = config;
        }
        [[nodiscard]] const SourceConfiguration getOpenConfiguration() const;
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
}
