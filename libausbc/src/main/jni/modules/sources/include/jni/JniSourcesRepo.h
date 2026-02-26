#pragma once

#include <memory>
#include <map>
#include <atomic>

#include "Source.h"
#include "Consumer.h"

class JniSourcesRepo {
private:
    static std::unique_ptr<JniSourcesRepo> instance;
    std::map<int, std::shared_ptr<auvc::Source>> sources;
    std::map<int, std::shared_ptr<auvc::Consumer>> consumers;
    std::atomic<int> nextId {1};

public:
    static JniSourcesRepo* getInstance();
    static void releaseInstance();

    int addSource(std::shared_ptr<auvc::Source> source);
    std::shared_ptr<auvc::Source> getSource(int id) const;
    void removeSource(int id);

    int addConsumer(std::shared_ptr<auvc::Consumer> consumer);
    std::shared_ptr<auvc::Consumer> getConsumer(int id) const;
    void removeConsumer(int id);
};