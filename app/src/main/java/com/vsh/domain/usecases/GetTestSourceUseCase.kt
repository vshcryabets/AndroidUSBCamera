package com.vsh.domain.usecases

import com.vsh.font.FontSrc
import com.vsh.source.Source
import com.vsh.source.TestSource

interface GetTestSourceUseCase {
    operator fun invoke(): Source<Source.OpenConfiguration>
}

class GetTestSourceUseCaseImpl(
    private val fontSrc: FontSrc
): GetTestSourceUseCase {
    override operator fun invoke(): Source<Source.OpenConfiguration> {
        return TestSource(fontSrc)
    }
}