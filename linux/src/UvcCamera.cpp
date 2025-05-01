#include "UvcCamera.h"
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

int UvcCamera::xioctl(int fh, int request, void *arg) {
    int r;

    do {
        r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}

void UvcCamera::init_read(unsigned int buffer_size)
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

void UvcCamera::init_mmap(const char* dev_name)
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s does not support "
                            "memory mapping\n", dev_name);
            exit(EXIT_FAILURE);
        } else {
            throw UvcException(UvcException::Type::IoCtlError);
        }
    }

    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory on %s\n",
                dev_name);
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

        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
            throw UvcException(UvcException::Type::IoCtlError);

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start =
                mmap(NULL /* start anywhere */,
                    buf.length,
                    PROT_READ | PROT_WRITE /* required */,
                    MAP_SHARED /* recommended */,
                    fd, buf.m.offset);

        if (MAP_FAILED == buffers[n_buffers].start)
            throw UvcException(UvcException::Type::MmapError);
    }
}

void UvcCamera::init_userp(unsigned int buffer_size, const char* dev_name)
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count  = 4;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_USERPTR;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s does not support "
                            "user pointer i/o\n", dev_name);
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

UvcCamera::UvcCamera() {
    width = 640;
    height = 480;
}

UvcCamera::~UvcCamera() {
    // Destructor
    // uninit_device();
    // close_device();
}
void UvcCamera::open_device(const char *dev_name) {
    struct stat st;

    if (-1 == stat(dev_name, &st)) {
        fprintf(stderr, "Cannot identify '%s': %d, %s\n",
                dev_name, errno, strerror(errno));
        throw UvcException(UvcException::Type::WrongDevice);
    }

    if (!S_ISCHR(st.st_mode)) {
        fprintf(stderr, "%s is no device\n", dev_name);
        throw UvcException(UvcException::Type::WrongDevice);
    }

    fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

    if (-1 == fd) {
        fprintf(stderr, "Cannot open '%s': %d, %s\n",
                dev_name, errno, strerror(errno));
        throw UvcException(UvcException::Type::CantOpenDevice);
    }
}

void UvcCamera::close_device() {
    if (-1 == close(fd))
        throw UvcException(UvcException::Type::CantCloseDevice);
    fd = -1;
}

void UvcCamera::init_device(const char *dev_name) {
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    unsigned int min;

    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s is no V4L2 device\n", dev_name);
            exit(EXIT_FAILURE);
        } else {
            throw UvcException(UvcException::Type::IoCtlError);
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "%s is no video capture device\n",
                dev_name);
        exit(EXIT_FAILURE);
    }

    switch (io) {
        case IO_METHOD_READ:
            if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
                fprintf(stderr, "%s does not support read i/o\n",
                        dev_name);
                exit(EXIT_FAILURE);
            }
            break;

        case IO_METHOD_MMAP:
        case IO_METHOD_USERPTR:
            if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                fprintf(stderr, "%s does not support streaming i/o\n",
                        dev_name);
                exit(EXIT_FAILURE);
            }
            break;
    }


    /* Select video input, video standard and tune here. */


    CLEAR(cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */

        if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
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
        fmt.fmt.pix.width = width;
        fmt.fmt.pix.height = height;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; //V4L2_PIX_FMT_H264; //replace
        fmt.fmt.pix.field = V4L2_FIELD_ANY;

        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
            throw UvcException(UvcException::Type::IoCtlError);

        /* Note VIDIOC_S_FMT may change width and height. */
    } else {
        /* Preserve original settings as set by v4l2-ctl for example */
        if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt))
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
            init_mmap(dev_name);
            break;

        case IO_METHOD_USERPTR:
            init_userp(fmt.fmt.pix.sizeimage, dev_name);
            break;
    }
}

void UvcCamera::uninit_device(void) {
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
}

void UvcCamera::start_capturing(void)
{
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

                if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                    throw UvcException(UvcException::Type::IoCtlError);
            }
            type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
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

                if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                    throw UvcException(UvcException::Type::IoCtlError);
            }
            type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
                throw UvcException(UvcException::Type::IoCtlError);
            break;
    }
}

void UvcCamera::stop_capturing(void)
{
    enum v4l2_buf_type type;

    switch (io) {
        case IO_METHOD_READ:
            /* Nothing to do. */
            break;
        case IO_METHOD_MMAP:
        case IO_METHOD_USERPTR:
            type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
                throw UvcException(UvcException::Type::IoCtlError);
            break;
    }
}


UvcCamera::Frame UvcCamera::readFrame(std::function<void(Frame)> processImageCallback){
    struct v4l2_buffer buf;
    unsigned int i;
    Frame result;

    switch (io) {
        case IO_METHOD_READ:
            if (-1 == read(fd, buffers[0].start, buffers[0].length)) {
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
            printf("Process image 1 %p %d\n", buffers[0].start, buffers[0].length);
            result.data = (uint8_t*)buffers[0].start;
            result.size = buffers[0].length;
            if (processImageCallback != nullptr)
                processImageCallback(result);
            break;

        case IO_METHOD_MMAP:
            CLEAR(buf);

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;

            if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
                switch (errno) {
                    case EAGAIN:
                        return Frame();

                    case EIO:
                        /* Could ignore EIO, see spec. */

                        /* fall through */

                    default:
                        throw UvcException(UvcException::Type::IoCtlError);
                }
            }

            //printf("Process image 2 %p %d\n", buffers[buf.index].start, buf.bytesused);
            result.data = (uint8_t*)buffers[buf.index].start;
            result.size = buf.bytesused;
            if (processImageCallback != nullptr)
                processImageCallback(result);

            if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                throw UvcException(UvcException::Type::IoCtlError);
            break;

        case IO_METHOD_USERPTR:
            CLEAR(buf);

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_USERPTR;

            if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
                switch (errno) {
                    case EAGAIN:
                        return Frame();

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

            printf("Process image 3 %p %d\n", (void *)buf.m.userptr, buf.bytesused);
            result.data = (uint8_t*)buf.m.userptr;
            result.size = buf.bytesused;
            if (processImageCallback != nullptr)
                processImageCallback(result);
            
            if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                throw UvcException(UvcException::Type::IoCtlError);
            break;
    }

    return result;
}

int UvcCamera::getFd() {
    return fd;
}