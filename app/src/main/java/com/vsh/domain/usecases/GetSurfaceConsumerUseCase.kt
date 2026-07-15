package com.vsh.domain.usecases

import com.vsh.source.JniConsumer
import com.vsh.source.SurfaceConsumer

interface GetSurfaceConsumerUseCase {
    operator fun invoke(): JniConsumer
}

class GetSurfaceConsumerUseCaseImpl(): GetSurfaceConsumerUseCase {
    override operator fun invoke(): JniConsumer {
        return SurfaceConsumer()
    }
}