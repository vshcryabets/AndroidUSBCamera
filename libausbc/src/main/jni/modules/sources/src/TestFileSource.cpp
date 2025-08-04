#include "TestFileSource.h"
#include <fstream>
#include <iostream>

TestFileWriter::TestFileWriter(const std::string &fileName,
                uint16_t width, 
                uint16_t height,
                std::string mimeType,
                float fps)
{
    dataFile.open(fileName, std::ios::binary);
    if (!dataFile.is_open()) {
        throw std::runtime_error("Failed to open file");
    }
    const char magic[4] = {'A', 'U', 'V', 'C'};
    dataFile.write(magic, sizeof(magic));
    uint16_t data = width;
    dataFile.write((const char*)&data, sizeof(data));
    data = height;
    dataFile.write((const char*)&data, sizeof(data));
    dataFile.write((const char*)&fps, sizeof(fps));
    uint32_t toc = 0;
    dataFile.write((const char*)&toc, sizeof(toc)); // Placeholder for TOC
    data = mimeType.size();
    dataFile.write((const char*)&data, sizeof(data));
    dataFile.write(mimeType.c_str(), mimeType.size());
    filePosition = dataFile.tellp();
}

TestFileWriter::~TestFileWriter()
{
    finalize();
}

void TestFileWriter::write(const uint8_t *data, uint32_t size)
{
    if (dataFile.is_open() && data != nullptr && size > 0) {
        dataFile.seekp(filePosition);
        if (filePosition % 4 != 0) {
            uint32_t pad = 4 - (filePosition % 4);
            char padding[4] = {0x55, (char)0xAA, 0x55, (char)0xAA};
            dataFile.write(padding, pad);
            filePosition += pad;
        }
        framesTocItems.push_back(filePosition);
        dataFile.write((const char*)&size, sizeof(size));
        filePosition += sizeof(size);
        dataFile.write(reinterpret_cast<const char*>(data), size);
        filePosition += size;
    }
}

void TestFileWriter::finalize()
{
    if (dataFile.is_open()) {
        uint32_t toc = filePosition;
        // write frames count
        uint32_t framesCount = framesTocItems.size();
        dataFile.write((const char*)&framesCount, sizeof(framesCount));
        for (const auto& pos : framesTocItems) {
            dataFile.write((const char*)&pos, sizeof(pos));
        }
        dataFile.seekp(TOC_POSITION);
        dataFile.write((const char*)&toc, sizeof(toc));

        dataFile.close();
        framesTocItems.clear();
        filePosition = 0;
    }
}

void TestFileSource::open(const ConnectConfiguration &config)
{
    if (config.fileName.empty()) {
        throw std::runtime_error("File name cannot be empty");
    }
    // Open the file for reading
    dataFile.open(config.fileName, std::ios::binary);
    if (!dataFile.is_open()) {
        throw std::runtime_error("Failed to open file: " + config.fileName);
    }
    // verify magic
    char magic[4];
    dataFile.read(magic, sizeof(magic));
    if (std::string(magic, sizeof(magic)) != "AUVC") {
        throw std::runtime_error("Invalid file format");
    }
    // Read width and height
    uint16_t width, height;
    float fps;
    dataFile.read(reinterpret_cast<char*>(&width), sizeof(width));
    dataFile.read(reinterpret_cast<char*>(&height), sizeof(height));
    dataFile.read(reinterpret_cast<char*>(&fps), sizeof(fps));
    dataFile.seekg(TOC_POSITION, std::ios::beg);
    dataFile.read(reinterpret_cast<char*>(&fileTocPosition), sizeof(fileTocPosition));

    supportedResolutions[0].push_back({0, width, height, {fps}});

    dataFile.seekg(fileTocPosition, std::ios::beg);
    dataFile.read(reinterpret_cast<char*>(&framesCount), sizeof(framesCount));
    framesTocItems.resize(framesCount);
    for (uint32_t i = 0; i < framesCount; ++i)
    {
        dataFile.read(reinterpret_cast<char*>(&framesTocItems[i]), sizeof(framesTocItems[i]));
    }
    supportedFormats.push_back(Source::FrameFormat::ENCODED);
}

void TestFileSource::close()
{
    if (dataFile.is_open()) {
        dataFile.close();
    }
}

std::map<uint16_t, std::vector<Source::Resolution>> TestFileSource::getSupportedResolutions()
{
    return supportedResolutions;
}

std::vector<Source::FrameFormat> TestFileSource::getSupportedFrameFormats()
{
    return supportedFormats;
}

TestFileSource::TestFileSource(): framesCount(0)
{

}

TestFileSource::~TestFileSource()
{
    stopCapturing();
    close();
}

void TestFileSource::stopCapturing()
{
}

bool TestFileSource::waitNextFrame() 
{
    return false;
}

Source::Frame TestFileSource::readFrame()
{
    if (currentFrame < framesCount) {
        dataFile.seekg(framesTocItems[currentFrame], std::ios::beg);
        currentFrame++;
        if (currentFrame >= framesCount) {
            currentFrame = 0; // Reset to the first frame if we reach the end
        }
        uint32_t size;
        dataFile.read(reinterpret_cast<char*>(&size), sizeof(size));
        if (size > 0) {
            auto frameData = std::make_unique<uint8_t[]>(size);
            dataFile.read(reinterpret_cast<char*>(frameData.get()), size);
            auto result = Source::Frame(640, 480, Source::FrameFormat::ENCODED);
            result.data = frameData.release();
            result.size = size;
            return result;
        }
    }
    return Source::Frame(0, 0, Source::FrameFormat::NONE);
}

void TestFileSource::startCapturing(const CaptureConfiguration &config)
{

}

uint32_t TestFileSource::setCurrentFrame(uint32_t frameIndex)
{
    if (frameIndex < framesCount) {
        currentFrame = frameIndex;
    }
    return currentFrame;
}