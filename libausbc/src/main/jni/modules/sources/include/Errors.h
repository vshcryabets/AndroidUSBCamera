#pragma once

#include <exception>
#include <string>
#include <cstdint>

namespace auvc {
    enum class ConsumerErrorCode : uint16_t {
        SUCCESS = 0,
        WRONG_CONFIGURATION,
        OBJECT_NOT_FOUND
    };

    class ConsumerError : public std::exception {
    public:
    private:
        ConsumerErrorCode code;
        std::string message;
    public:
        ConsumerError(ConsumerErrorCode code, const std::string &message) : code(code), message(message) {}
        ~ConsumerError() override = default;
        const char* what() const noexcept override;
        ConsumerErrorCode getCode() const noexcept { return code; }

        static ConsumerError SUCCESS;
        static ConsumerError NOT_FOUND;
    };

    enum class SourceErrorCode : uint16_t {
        SUCCESS = 0,
        SOURCE_ERROR_WRONG_CONFIG,
        SOURCE_ERROR_CAPTURE_NOT_STARTED,
        SOURCE_ERROR_NOT_OPENED,
        SOURCE_ERROR_READ_AGAIN,
        SOURCE_FRAME_NOT_AVAILABLE,
        OBJECT_NOT_FOUND
    };

    class SourceError : public std::exception {
        public:
        private:
            SourceErrorCode code;
            std::string message;
        public:
            SourceError(SourceErrorCode code, const std::string &message) : code(code), message(message) {}
            const char* what() const noexcept override;
            SourceErrorCode getCode() const noexcept { return code; }

            static const SourceError NOT_FOUND;
            static const SourceError SUCCESS;
    };
}