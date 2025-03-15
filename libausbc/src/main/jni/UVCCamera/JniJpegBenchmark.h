/*
 * Copyright 2025 vschryabets@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROIDUSBCAMERA_JNIJPEGBENCHMARK_H
#define ANDROIDUSBCAMERA_JNIJPEGBENCHMARK_H
#include "JpegBenchmark.h"

class JniJpegBenchmark: public JpegBenchmark {
private:
    int sockfd;
public:
    JniJpegBenchmark(const std::string &socketFilePath);
    ~JniJpegBenchmark() override;
    [[nodiscard]] int getSockFd() const;
    void startAndSendToSockFd();
};


#endif //ANDROIDUSBCAMERA_JNIJPEGBENCHMARK_H
