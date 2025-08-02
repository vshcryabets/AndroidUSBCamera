#include "TestFileSource.h"
#include <fstream>
#include <iostream>

TestFileWritter::TestFileWritter(const std::string &fileName,
                uint16_t width, 
                uint16_t height,
                std::string mimeType,
                float fps)
{
    dataFile = new std::ofstream(fileName, std::ios::binary);
    if (!dataFile->is_open()) {
        throw std::runtime_error("Failed to open file");
    }
    const char magic[4] = {'A', 'U', 'V', 'C'};
    dataFile->write(magic, sizeof(magic));
    uint16_t data = width;
    dataFile->write((const char*)&data, sizeof(data));
    data = height;
    dataFile->write((const char*)&data, sizeof(data));
    dataFile->write((const char*)&fps, sizeof(fps));
    uint32_t toc = 0;
    dataFile->write((const char*)&toc, sizeof(toc)); // Placeholder for TO
    data = mimeType.size();
    dataFile->write((const char*)&data, sizeof(data));
    dataFile->write(mimeType.c_str(), mimeType.size());
    filePosition = dataFile->tellp();
}

TestFileWritter::~TestFileWritter()
{
    finalize();
}

void TestFileWritter::write(const uint8_t *data, uint32_t size)
{
    if (dataFile != nullptr && data != nullptr && size > 0) {
        dataFile->seekp(filePosition);
        if (filePosition % 4 != 0) {
            uint32_t pad = 4 - (filePosition % 4);
            char padding[4] = {0x55, (char)0xAA, 0x55, (char)0xAA};
            dataFile->write(padding, pad);
            filePosition += pad;
        }
        framesTocItems.push_back(filePosition);
        dataFile->write((const char*)&size, sizeof(size));
        filePosition += sizeof(size);
        dataFile->write(reinterpret_cast<const char*>(data), size);
        filePosition += size;
    }
}

void TestFileWritter::finalize()
{
    if (dataFile) {
        uint32_t toc = filePosition;
        // write frames count
        uint32_t framesCount = framesTocItems.size();
        dataFile->write((const char*)&framesCount, sizeof(framesCount));
        for (const auto& pos : framesTocItems) {
            dataFile->write((const char*)&pos, sizeof(pos));
        }
        dataFile->seekp(TOC_POSITION);
        dataFile->write((const char*)&toc, sizeof(toc));

        dataFile->close();
        delete dataFile;
        dataFile = nullptr;
    }
}
