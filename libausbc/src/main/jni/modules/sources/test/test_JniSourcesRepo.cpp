#include <catch2/catch_test_macros.hpp>
#include "jni/JniSourcesRepo.h"
#include "TestFileSource.h"
#include "Consumer.h"

using namespace auvc::jni;

class TestConsumer : public auvc::Consumer {
public:
    void consume(const auvc::Frame& frame) override {
        // Do nothing for testing purposes
    }
};

TEST_CASE("JniSourcesRepo Test source operations", "[JniSourcesRepo]") {
    auto repo = JniSourcesRepo::getInstance();

    SECTION("Add and get source") {
        auto source = std::make_shared<auvc::TestFileSource>();
        int id = repo->addSource(source);
        REQUIRE(id > 0);

        auto retrieved = repo->getSource(id);
        REQUIRE(retrieved != nullptr);
        REQUIRE(retrieved == source);
    }

    SECTION("Remove source") {
        auto source = std::make_shared<auvc::TestFileSource>();
        int id = repo->addSource(source);
        REQUIRE(id > 0);

        repo->removeSource(id);
        auto retrieved = repo->getSource(id);
        REQUIRE(retrieved == nullptr);
    }

    JniSourcesRepo::releaseInstance();
}

TEST_CASE("JniSourcesRepo test consumer operations", "[JniSourcesRepo]") {
    auto repo = JniSourcesRepo::getInstance();
    SECTION("Add and get consumer") {
        auto consumer = std::make_shared<TestConsumer>();
        int id = repo->addConsumer(consumer);
        REQUIRE(id > 0);

        auto retrieved = repo->getConsumer(id);
        REQUIRE(retrieved != nullptr);
        REQUIRE(retrieved == consumer);
    }

    SECTION("Remove consumer") {
        auto consumer = std::make_shared<TestConsumer>();
        int id = repo->addConsumer(consumer);
        REQUIRE(id > 0);

        repo->removeConsumer(id);
        auto retrieved = repo->getConsumer(id);
        REQUIRE(retrieved == nullptr);
    }

    JniSourcesRepo::releaseInstance();
}