#include "jni/JniSourcesRepo.h"

std::unique_ptr<JniSourcesRepo> JniSourcesRepo::instance = nullptr;

JniSourcesRepo* JniSourcesRepo::getInstance() {
    if (instance == nullptr) {
        instance = std::make_unique<JniSourcesRepo>();
    }
    return instance.get();
}

void JniSourcesRepo::releaseInstance() {
    instance.reset();
}

int JniSourcesRepo::addSource(std::shared_ptr<Source> source) {
    int id = nextId++;
    sources[id] = source;
    return id;
}

std::shared_ptr<Source> JniSourcesRepo::getSource(int id) const {
    auto it = sources.find(id);
    if (it != sources.end()) {
        return it->second;
    }
    return nullptr;
}

void JniSourcesRepo::removeSource(int id) {
    sources.erase(id);
}

int JniSourcesRepo::addConsumer(std::shared_ptr<auvc::Consumer> consumer) {
    int id = nextId++;
    consumers[id] = consumer;
    return id;
}

std::shared_ptr<auvc::Consumer> JniSourcesRepo::getConsumer(int id) const {
    auto it = consumers.find(id);
    if (it != consumers.end()) {
        return it->second;
    }
    return nullptr;
}

void JniSourcesRepo::removeConsumer(int id) {
    consumers.erase(id);
}