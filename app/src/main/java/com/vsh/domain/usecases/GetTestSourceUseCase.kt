package com.vsh.domain.usecases

import com.jiangdg.uvc.Source
import com.jiangdg.uvc.TestSource

interface GetTestSourceUseCase {
    operator fun invoke(): Source<Source.OpenConfiguration>
}

class GetTestSourceUseCaseImpl: GetTestSourceUseCase {
    override operator fun invoke(): Source<Source.OpenConfiguration> {
        return TestSource()
    }
}