package com.vsh.domain.usecases

import com.vsh.font.FontSrc
import com.vsh.source.JniSource
import com.vsh.source.Source
import com.vsh.source.TestSource

interface GetTestSourceUseCase {
    operator fun invoke(): JniSource<Source.OpenConfiguration, Source.ProducingConfiguration>
}

class GetTestSourceUseCaseImpl(
    private val fontSrc: FontSrc
): GetTestSourceUseCase {
    override operator fun invoke(): JniSource<Source.OpenConfiguration, Source.ProducingConfiguration> {
        return TestSource(fontSrc)
    }
}