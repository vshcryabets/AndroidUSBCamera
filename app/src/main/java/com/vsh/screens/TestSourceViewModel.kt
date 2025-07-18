package com.vsh.screens

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import com.jiangdg.uvc.IUvcCamera
import com.jiangdg.uvc.TestSource
import timber.log.Timber

class TestSourceViewModelFactory(
) : ViewModelProvider.Factory {
    @Suppress("UNCHECKED_CAST")
    override fun <T : ViewModel> create(modelClass: Class<T>): T =
        TestSourceViewModel(
        ) as T
}

class TestSourceViewModel(
) : ViewModel() {
    private val source: IUvcCamera<*>

    init {
        Timber.d("TestSourceViewModel created")
        source = TestSource()
    }

    override fun onCleared() {
        super.onCleared()
        Timber.d("TestSourceViewModel cleared")
    }
}