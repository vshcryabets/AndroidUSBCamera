package com.vsh.domain.usecases

import com.vsh.font.FontSrc
import com.vsh.source.TestSource

interface GetTestSourceUseCase {
    operator fun invoke(): TestSource
}

class GetTestSourceUseCaseImpl(
    private val fontSrc: FontSrc
): GetTestSourceUseCase {
    override operator fun invoke(): TestSource {
        return TestSource(fontSrc)
    }
}