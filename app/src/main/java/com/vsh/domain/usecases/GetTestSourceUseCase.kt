package com.vsh.domain.usecases

import com.jiangdg.uvc.IUvcCamera
import com.jiangdg.uvc.TestSource

interface GetTestSourceUseCase {
    operator fun invoke(): IUvcCamera<IUvcCamera.OpenConfiguration>
}

class GetTestSourceUseCaseImpl: GetTestSourceUseCase {
    override operator fun invoke(): IUvcCamera<IUvcCamera.OpenConfiguration> {
        return TestSource()
    }
}