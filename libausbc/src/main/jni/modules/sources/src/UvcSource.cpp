#include <iostream>
#include <optional>

#include "UvcSource.h"
#include <string.h> //memset
#include <stdlib.h> //calloc
#include <unistd.h> //close
#include <stdio.h>  //printf
#include <errno.h>  //errno
#include <sys/mman.h> //mmap
#include <sys/ioctl.h> //ioctl
#include <sys/stat.h> //stat
#include <fcntl.h> // low-level i/o
// V4L UVC
#include <linux/videodev2.h> //v4l2_buffer

#define CLEAR(x) memset(&(x), 0, sizeof(x))


UvcException::UvcException(Type type, const std::string& message) : errorType(type) {
    std::string base;
    switch (errorType) {
        case Other:
            base = "Other error";
            break;
        case WrongDevice:
            base = "Wrong device";
            break;
        case CantOpenDevice:
            base = "Can't open device";
            break;
        case CantCloseDevice:
            base = "Can't close device";
            break;
        case IoCtlError:
            base = "Ioctl error";
            break;
        case MmapError:
            base = "Mmap error";
            break;
        case ReadError:
            base = "Read error";
            break;
        case FrameTimeout:
            base = "Frame timeout";
            break;
        default:
            base = "Unknown error";
    }
    errorMessage = base + ": " + message;
}

const char* UvcException::what() const noexcept {
    return errorMessage.c_str();    
}

int UvcSource::xioctl(int fh, int request, void *arg) {
    int r;

    do {
        r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}

void UvcSource::init_read(size_t buffer_size)
{
    buffers.push_back(buffer{.buffer = std::vector<uint8_t>(buffer_size)});
}

void UvcSource::init_mmap()
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (xioctl(deviceFd, VIDIOC_REQBUFS, &req) == -1) {
        if (EINVAL == errno) {
            throw UvcException(UvcException::Type::IoCtlError, this->uvcConfig.dev_name + " does not support memory mapping");
        } else {
            throw UvcException(UvcException::Type::IoCtlError, "VIDIOC_REQBUFS");
        }
    }

    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory on %s\n",
            this->uvcConfig.dev_name);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < req.count; ++i) {
        v4l2_buffer buf;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == xioctl(deviceFd, VIDIOC_QUERYBUF, &buf))
            throw UvcException(UvcException::Type::IoCtlError, "VIDIOC_QUERYBUF");

        buffer current = {
            .mmapBuffer = (uint8_t*)mmap(0 /* start anywhere */,
                    buf.length,
                    PROT_READ | PROT_WRITE /* required */,
                    MAP_SHARED /* recommended */,
                    deviceFd, 
                    buf.m.offset),
            .mmapSize = buf.length
        };
        if (current.mmapBuffer == MAP_FAILED)
            throw UvcException(UvcException::Type::MmapError, "mmap failed");
        buffers.push_back(current);
    }
}

void UvcSource::init_userp(size_t buffer_size)
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count  = 4;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_USERPTR;

    if (-1 == xioctl(deviceFd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s does not support "
                            "user pointer i/o\n", this->uvcConfig.dev_name);
            exit(EXIT_FAILURE);
        } else {
            throw UvcException(UvcException::Type::IoCtlError, "VIDIOC_REQBUFS");
        }
    }

    for (size_t n_buffers = 0; n_buffers < 4; ++n_buffers) {
        buffers.push_back(buffer{
            .buffer = std::vector<uint8_t>(buffer_size),
        });
    }
}

UvcSource::UvcSource() {
}

UvcSource::~UvcSource() {
    close().get();
}

void UvcSource::open(const UvcSource::OpenConfiguration & config) {
    PullSource::open(config);
    this->uvcConfig = config;
    struct stat st;
    const char* dev_name = uvcConfig.dev_name.c_str();

    if (-1 == stat(dev_name, &st)) {
        throw UvcException(UvcException::Type::WrongDevice, "Cannot identify device");
    }

    if (!S_ISCHR(st.st_mode)) {
        throw UvcException(UvcException::Type::WrongDevice, "Not a character device");
    }

    deviceFd = ::open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);
    if (-1 == deviceFd) {
        throw UvcException(UvcException::Type::CantOpenDevice, "Can't open device " + config.dev_name);
    }
}

std::future<void> UvcSource::close() {
    return std::async(std::launch::async, [this]() {
        unsigned int i;
        switch (io) {
            case IO_METHOD_MMAP:
                for (i = 0; i < buffers.size(); ++i)
                    if (-1 == munmap(buffers[i].mmapBuffer, buffers[i].mmapSize))
                        throw UvcException(UvcException::Type::MmapError, "munmap() failed");
                break;

            case IO_METHOD_READ:
            case IO_METHOD_USERPTR:
                break;
        }

        buffers.clear();

        if (deviceFd > 0) {
            ::close(deviceFd);
            deviceFd = -1;
        }
    });
}

void UvcSource::init_device() {
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    unsigned int min;

    if (deviceFd < 0) {
        throw UvcException(UvcException::Type::DeviceNotOpened, "Call open() before startProducing()");
    }

    if (-1 == xioctl(deviceFd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s is no V4L2 device\n", this->uvcConfig.dev_name);
            exit(EXIT_FAILURE);
        } else {
            throw UvcException(UvcException::Type::IoCtlError, "VIDIOC_QUERYCAP");
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "%s is no video capture device\n",
            this->uvcConfig.dev_name);
        exit(EXIT_FAILURE);
    }
    switch (io) {
        case IO_METHOD_READ:
            if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
                fprintf(stderr, "%s does not support read i/o\n",
                    this->uvcConfig.dev_name);
                exit(EXIT_FAILURE);
            }
            break;

        case IO_METHOD_MMAP:
        case IO_METHOD_USERPTR:
            if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                fprintf(stderr, "%s does not support streaming i/o\n",
                    this->uvcConfig.dev_name);
                exit(EXIT_FAILURE);
            }
            break;
    }


    /* Select video input, video standard and tune here. */


    CLEAR(cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl(deviceFd, VIDIOC_CROPCAP, &cropcap)) {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */

        if (-1 == xioctl(deviceFd, VIDIOC_S_CROP, &crop)) {
            switch (errno) {
                case EINVAL:
                    /* Cropping not supported. */
                    break;
                default:
                    /* Errors ignored. */
                    break;
            }
        }
    } else {
        /* Errors ignored. */
    }       
    CLEAR(fmt);

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (force_format) {
        auto config = getProducingConfiguration();
        fmt.fmt.pix.width = config.width;
        fmt.fmt.pix.height = config.height;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; //V4L2_PIX_FMT_H264; //replace
        fmt.fmt.pix.field = V4L2_FIELD_ANY;

        if (-1 == xioctl(deviceFd, VIDIOC_S_FMT, &fmt))
            throw UvcException(UvcException::Type::IoCtlError, "VIDIOC_S_FMT");

        /* Note VIDIOC_S_FMT may change width and height. */
    } else {
        /* Preserve original settings as set by v4l2-ctl for example */
        if (-1 == xioctl(deviceFd, VIDIOC_G_FMT, &fmt))
            throw UvcException(UvcException::Type::IoCtlError, "VIDIOC_G_FMT");
    }

    /* Buggy driver paranoia. */
    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min)
        fmt.fmt.pix.bytesperline = min;
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)
        fmt.fmt.pix.sizeimage = min;

    switch (io) {
        case IO_METHOD_READ:
            init_read(fmt.fmt.pix.sizeimage);
            break;

        case IO_METHOD_MMAP:
            init_mmap();
            break;

        case IO_METHOD_USERPTR:
            init_userp(fmt.fmt.pix.sizeimage);
            break;
    }
}

std::future<void> UvcSource::startProducing(const Source::ProducingConfiguration &config)
{    
    return std::async(std::launch::async, [this,&config]() {
        PullSource::startProducing(config).get();
        init_device();

        unsigned int i;
        enum v4l2_buf_type type;

        switch (io) {
            case IO_METHOD_READ:
                /* Nothing to do. */
                break;

            case IO_METHOD_MMAP:
                for (i = 0; i < buffers.size(); ++i) {
                    struct v4l2_buffer buf;

                    CLEAR(buf);
                    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    buf.memory = V4L2_MEMORY_MMAP;
                    buf.index = i;

                    if (-1 == xioctl(deviceFd, VIDIOC_QBUF, &buf))
                        throw UvcException(UvcException::Type::IoCtlError, "VIDIOC_QBUF");
                }
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(deviceFd, VIDIOC_STREAMON, &type))
                    throw UvcException(UvcException::Type::IoCtlError, "VIDIOC_STREAMON");
                break;

            case IO_METHOD_USERPTR:
                for (i = 0; i < buffers.size(); ++i) {
                    struct v4l2_buffer buf;

                    CLEAR(buf);
                    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    buf.memory = V4L2_MEMORY_USERPTR;
                    buf.index = i;
                    buf.m.userptr = (unsigned long)buffers[i].buffer.data();
                    buf.length = buffers[i].buffer.size();

                    if (-1 == xioctl(deviceFd, VIDIOC_QBUF, &buf))
                        throw UvcException(UvcException::Type::IoCtlError, "VIDIOC_QBUF");
                }
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(deviceFd, VIDIOC_STREAMON, &type))
                    throw UvcException(UvcException::Type::IoCtlError, "VIDIOC_STREAMON");
                break;
        }
    });
}

std::future<void> UvcSource::stopProducing()
{
    return std::async(std::launch::async, [this]() {
        PullSource::stopProducing().get();
        enum v4l2_buf_type type;
        switch (io) {
            case IO_METHOD_READ:
                /* Nothing to do. */
                break;
            case IO_METHOD_MMAP:
            case IO_METHOD_USERPTR:
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(deviceFd, VIDIOC_STREAMOFF, &type))
                    throw UvcException(UvcException::Type::IoCtlError, "VIDIOC_STREAMOFF");
                break;
        }
    });
}


auvc::ExpectedFrame UvcSource::readFrame(){
    frameCounter++;
    struct v4l2_buffer buf;
    unsigned int i;
    std::optional<auvc::Frame> result = std::nullopt;

    switch (io) {
        case IO_METHOD_READ:
            if (-1 == read(deviceFd, buffers[0].buffer.data(), buffers[0].buffer.size())) {
                switch (errno) {
                    case EAGAIN:
                        return std::unexpected(auvc::SourceError(auvc::SourceErrorCode::SOURCE_ERROR_READ_AGAIN, "read again"));
                    case EIO:
                        /* Could ignore EIO, see spec. */

                        /* fall through */
                    default:
                        throw UvcException(UvcException::Type::ReadError, "read device");
                }
            }
            result = auvc::Frame(
                getProducingConfiguration().width,
                getProducingConfiguration().height,
                auvc::FrameFormat::YUYV,
                (uint8_t*)buffers[0].buffer.data(),
                buffers[0].buffer.size(),
                std::chrono::high_resolution_clock::now()
            );
            break;

        case IO_METHOD_MMAP:
            CLEAR(buf);

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;

            if (-1 == xioctl(deviceFd, VIDIOC_DQBUF, &buf)) {
                switch (errno) {
                    case EAGAIN:
                        return std::unexpected(auvc::SourceError(auvc::SourceErrorCode::SOURCE_ERROR_READ_AGAIN, "read again"));
                    case EIO:
                        /* Could ignore EIO, see spec. */
                        /* fall through */
                    default:
                        throw UvcException(UvcException::Type::IoCtlError, "readFrame IO_METHOD_MMAP VIDIOC_DQBUF " + 
                            std::to_string(errno) + " " + 
                            std::string(strerror(errno)));
                }
            }

            result = auvc::Frame(
                getProducingConfiguration().width,
                getProducingConfiguration().height,
                auvc::FrameFormat::YUYV,
                (uint8_t*)buffers[buf.index].mmapBuffer,
                buf.bytesused,
                std::chrono::high_resolution_clock::now()
            );

            if (-1 == xioctl(deviceFd, VIDIOC_QBUF, &buf))
                throw UvcException(UvcException::Type::IoCtlError, "VIDIOC_QBUF");
            break;

        case IO_METHOD_USERPTR:
            CLEAR(buf);

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_USERPTR;

            if (-1 == xioctl(deviceFd, VIDIOC_DQBUF, &buf)) {
                switch (errno) {
                    case EAGAIN:
                        return std::unexpected(auvc::SourceError(auvc::SourceErrorCode::SOURCE_ERROR_READ_AGAIN, "read again"));
                    case EIO:
                        /* Could ignore EIO, see spec. */

                        /* fall through */

                    default:
                        throw UvcException(UvcException::Type::IoCtlError, "VIDIOC_DQBUF");
                }
            }

            for (i = 0; i < buffers.size(); ++i)
                if (buf.m.userptr == (unsigned long)buffers[i].buffer.data()
                    && buf.length == buffers[i].buffer.size())
                    break;

            result = auvc::Frame(
                getProducingConfiguration().width,
                getProducingConfiguration().height,
                auvc::FrameFormat::YUYV,
                (uint8_t*)buf.m.userptr,
                buf.bytesused,
                std::chrono::high_resolution_clock::now()
            );
            if (-1 == xioctl(deviceFd, VIDIOC_QBUF, &buf))
                throw UvcException(UvcException::Type::IoCtlError, "VIDIOC_QBUF");
            break;
    }

    if (!result.has_value()) {
        return std::unexpected(auvc::SourceError(auvc::SourceErrorCode::SOURCE_FRAME_NOT_AVAILABLE, "No frame available"));
    }
    return result.value();
}

bool UvcSource::waitNextFrame() {
    fd_set fds;
    struct timeval tv;
    int r;

    FD_ZERO(&fds);
    FD_SET(deviceFd, &fds);

    /* Timeout. */
    tv.tv_sec = 4;
    tv.tv_usec = 0;

    r = select(deviceFd + 1, &fds, NULL, NULL, &tv);

    if (0 == r) {
        throw UvcException(UvcException::Type::FrameTimeout, "select timeout");
    }
    return r > 0;
}

std::vector<auvc::FrameFormat> UvcSource::getSupportedFrameFormats() const {
    return { auvc::FrameFormat::YUYV };
}

auvc::ExpectedResolutions UvcSource::getSupportedResolutions() const {
    std::map<uint16_t, std::vector<auvc::Resolution>> result;
    if (deviceFd < 0) {
        return std::unexpected(auvc::SourceError(auvc::SourceErrorCode::SOURCE_ERROR_NOT_OPENED, "Device not opened"));
    }

    struct v4l2_fmtdesc fmtdesc;
    memset(&fmtdesc, 0, sizeof(fmtdesc));
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    while (ioctl(deviceFd, VIDIOC_ENUM_FMT, &fmtdesc) == 0) {
        uint16_t pixfmt = fmtdesc.pixelformat;
        std::vector<auvc::Resolution> resolutions;

        struct v4l2_frmsizeenum frmsize;
        memset(&frmsize, 0, sizeof(frmsize));
        frmsize.pixel_format = fmtdesc.pixelformat;

        while (ioctl(deviceFd, VIDIOC_ENUM_FRAMESIZES, &frmsize) == 0) {
            if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                resolutions.emplace_back(frmsize.discrete.width, frmsize.discrete.height);
            } else if (frmsize.type == V4L2_FRMSIZE_TYPE_STEPWISE) {
                for (uint32_t w = frmsize.stepwise.min_width; w <= frmsize.stepwise.max_width; w += frmsize.stepwise.step_width) {
                    for (uint32_t h = frmsize.stepwise.min_height; h <= frmsize.stepwise.max_height; h += frmsize.stepwise.step_height) {
                        resolutions.emplace_back(w, h);
                    }
                }
            }
            frmsize.index++;
        }

        if (!resolutions.empty()) {
            result[pixfmt] = resolutions;
        }
        fmtdesc.index++;
    }

    return result;
}