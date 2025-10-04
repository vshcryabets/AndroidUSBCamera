#include <iostream>

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

const char* UvcException::what() const noexcept {
    switch (errorType) {
        case Other:
            return "Other error";
        case WrongDevice:
            return "Wrong device";
        case CantOpenDevice:
            return "Can't open device";
        case CantCloseDevice:
            return "Can't close device";
        case IoCtlError:
            return "Ioctl error";
        case MmapError:
            return "Mmap error";
        case ReadError:
            return "Read error";
        case FrameTimeout:
            return "Frame timeout";
        default:
            return "Unknown error";
    }
}

int UvcSource::xioctl(int fh, int request, void *arg) {
    int r;

    do {
        r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}

void UvcSource::init_read(unsigned int buffer_size)
{
    buffers = (buffer*)calloc(1, sizeof(*buffers));

    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    buffers[0].length = buffer_size;
    buffers[0].start = malloc(buffer_size);

    if (!buffers[0].start) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }
}

void UvcSource::init_mmap()
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(deviceFd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s does not support "
                            "memory mapping\n", this->uvcConfig.dev_name);
            exit(EXIT_FAILURE);
        } else {
            throw UvcException(UvcException::Type::IoCtlError);
        }
    }

    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory on %s\n",
            this->uvcConfig.dev_name);
        exit(EXIT_FAILURE);
    }

    buffers = (buffer*)calloc(req.count, sizeof(*buffers));

    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = n_buffers;

        if (-1 == xioctl(deviceFd, VIDIOC_QUERYBUF, &buf))
            throw UvcException(UvcException::Type::IoCtlError);

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start =
                mmap(NULL /* start anywhere */,
                    buf.length,
                    PROT_READ | PROT_WRITE /* required */,
                    MAP_SHARED /* recommended */,
                    deviceFd, buf.m.offset);

        if (MAP_FAILED == buffers[n_buffers].start)
            throw UvcException(UvcException::Type::MmapError);
    }
}

void UvcSource::init_userp(unsigned int buffer_size)
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
            throw UvcException(UvcException::Type::IoCtlError);
        }
    }

    buffers = (buffer*)calloc(4, sizeof(*buffers));

    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
        buffers[n_buffers].length = buffer_size;
        buffers[n_buffers].start = malloc(buffer_size);

        if (!buffers[n_buffers].start) {
            fprintf(stderr, "Out of memory\n");
            exit(EXIT_FAILURE);
        }
    }
}

UvcSource::UvcSource() {
}

UvcSource::~UvcSource() {
    // Destructor
    // uninit_device();
    // close_device();
}

void UvcSource::open(const UvcSource::OpenConfiguration & config) {
    PullSource::open(config);
    this->uvcConfig = config;
    struct stat st;
    const char* dev_name = uvcConfig.dev_name.c_str();

    if (-1 == stat(dev_name, &st)) {
        std::cerr << "Cannot identify '" << dev_name << "': " << errno << ", " << strerror(errno) << std::endl;
        throw UvcException(UvcException::Type::WrongDevice);
    }

    if (!S_ISCHR(st.st_mode)) {
        std::cerr << dev_name << " is not a character device" << std::endl;
        throw UvcException(UvcException::Type::WrongDevice);
    }

    deviceFd = ::open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

    if (-1 == deviceFd) {
        std::cerr << "Cannot open '" << dev_name << "': " << errno << ", " << strerror(errno) << std::endl;
        throw UvcException(UvcException::Type::CantOpenDevice);
    }
}

std::future<void> UvcSource::close() {
    return std::async(std::launch::async, [this]() {
        unsigned int i;
        switch (io) {
            case IO_METHOD_READ:
                free(buffers[0].start);
                break;
            case IO_METHOD_MMAP:
                for (i = 0; i < n_buffers; ++i)
                    if (-1 == munmap(buffers[i].start, buffers[i].length))
                        throw UvcException(UvcException::Type::MmapError);
                break;

            case IO_METHOD_USERPTR:
                for (i = 0; i < n_buffers; ++i)
                    free(buffers[i].start);
                break;
        }

        free(buffers);

        if (-1 == ::close(deviceFd))
            throw UvcException(UvcException::Type::CantCloseDevice);
        deviceFd = -1;
    });
}

void UvcSource::init_device() {
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    unsigned int min;

    if (-1 == xioctl(deviceFd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s is no V4L2 device\n", this->uvcConfig.dev_name);
            exit(EXIT_FAILURE);
        } else {
            throw UvcException(UvcException::Type::IoCtlError);
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
            throw UvcException(UvcException::Type::IoCtlError);

        /* Note VIDIOC_S_FMT may change width and height. */
    } else {
        /* Preserve original settings as set by v4l2-ctl for example */
        if (-1 == xioctl(deviceFd, VIDIOC_G_FMT, &fmt))
            throw UvcException(UvcException::Type::IoCtlError);
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
                for (i = 0; i < n_buffers; ++i) {
                    struct v4l2_buffer buf;

                    CLEAR(buf);
                    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    buf.memory = V4L2_MEMORY_MMAP;
                    buf.index = i;

                    if (-1 == xioctl(deviceFd, VIDIOC_QBUF, &buf))
                        throw UvcException(UvcException::Type::IoCtlError);
                }
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(deviceFd, VIDIOC_STREAMON, &type))
                    throw UvcException(UvcException::Type::IoCtlError);
                break;

            case IO_METHOD_USERPTR:
                for (i = 0; i < n_buffers; ++i) {
                    struct v4l2_buffer buf;

                    CLEAR(buf);
                    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    buf.memory = V4L2_MEMORY_USERPTR;
                    buf.index = i;
                    buf.m.userptr = (unsigned long)buffers[i].start;
                    buf.length = buffers[i].length;

                    if (-1 == xioctl(deviceFd, VIDIOC_QBUF, &buf))
                        throw UvcException(UvcException::Type::IoCtlError);
                }
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(deviceFd, VIDIOC_STREAMON, &type))
                    throw UvcException(UvcException::Type::IoCtlError);
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
                    throw UvcException(UvcException::Type::IoCtlError);
                break;
        }
    });
}


auvc::Frame UvcSource::readFrame(){
    frameCounter++;
    struct v4l2_buffer buf;
    unsigned int i;
    auvc::Frame result(0,0,auvc::FrameFormat::NONE,nullptr,0,std::chrono::high_resolution_clock::now());

    switch (io) {
        case IO_METHOD_READ:
            if (-1 == read(deviceFd, buffers[0].start, buffers[0].length)) {
                switch (errno) {
                    case EAGAIN:
                        return result;
                    case EIO:
                        /* Could ignore EIO, see spec. */

                        /* fall through */
                    default:
                        throw UvcException(UvcException::Type::ReadError);
                }
            }
            result = auvc::Frame(
                getProducingConfiguration().width,
                getProducingConfiguration().height,
                auvc::FrameFormat::YUYV,
                (uint8_t*)buffers[0].start,
                buffers[0].length,
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
                        return auvc::Frame(
                            0,
                            0,
                            auvc::FrameFormat::NONE,
                            nullptr,
                            0,
                            std::chrono::high_resolution_clock::now()
                        );

                    case EIO:
                        /* Could ignore EIO, see spec. */

                        /* fall through */

                    default:
                        throw UvcException(UvcException::Type::IoCtlError);
                }
            }

            //printf("Process image 2 %p %d\n", buffers[buf.index].start, buf.bytesused);
            result = auvc::Frame(
                getProducingConfiguration().width,
                getProducingConfiguration().height,
                auvc::FrameFormat::YUYV,
                (uint8_t*)buffers[buf.index].start,
                buf.bytesused,
                std::chrono::high_resolution_clock::now()
            );

            if (-1 == xioctl(deviceFd, VIDIOC_QBUF, &buf))
                throw UvcException(UvcException::Type::IoCtlError);
            break;

        case IO_METHOD_USERPTR:
            CLEAR(buf);

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_USERPTR;

            if (-1 == xioctl(deviceFd, VIDIOC_DQBUF, &buf)) {
                switch (errno) {
                    case EAGAIN:
                        return auvc::Frame(
                            0,
                            0,
                            auvc::FrameFormat::NONE,
                            nullptr,
                            0,
                            std::chrono::high_resolution_clock::now()
                        );

                    case EIO:
                        /* Could ignore EIO, see spec. */

                        /* fall through */

                    default:
                        throw UvcException(UvcException::Type::IoCtlError);
                }
            }

            for (i = 0; i < n_buffers; ++i)
                if (buf.m.userptr == (unsigned long)buffers[i].start
                    && buf.length == buffers[i].length)
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
                throw UvcException(UvcException::Type::IoCtlError);
            break;
    }

    return result;
}

bool UvcSource::waitNextFrame() {
    fd_set fds;
    struct timeval tv;
    int r;

    FD_ZERO(&fds);
    FD_SET(deviceFd, &fds);

    /* Timeout. */
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    r = select(deviceFd + 1, &fds, NULL, NULL, &tv);

    if (0 == r) {
        throw UvcException(UvcException::Type::FrameTimeout);
    }
    return r > 0;
}

std::vector<auvc::FrameFormat> UvcSource::getSupportedFrameFormats() const {
    return { auvc::FrameFormat::YUYV };
}

auvc::ExpectedResolutions UvcSource::getSupportedResolutions() const {
    std::map<uint16_t, std::vector<auvc::Resolution>> result;
    if (deviceFd < 0) {
        return std::unexpected(auvc::SourceError(auvc::SourceError::SOURCE_ERROR_NOT_OPENED, "Device not opened"));
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