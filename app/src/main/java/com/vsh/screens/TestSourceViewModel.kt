package com.vsh.screens

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
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
    init {
        Timber.d("TestSourceViewModel created")
    }

    override fun onCleared() {
        super.onCleared()
        Timber.d("TestSourceViewModel cleared")
    }
}