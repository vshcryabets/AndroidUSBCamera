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
package com.vsh.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.navigation.NavController

@Composable
fun TestSourceConfiguration(
    navController: NavController,
    viewModel: TestSourceViewModel
) {

    val uiState by viewModel.state.collectAsState()
    Column {
        if (uiState.resolutionStrs.isEmpty()) {
            Text(
                "No Source configurations available",
                modifier = Modifier.padding(16.dp)
            )
        } else {
            Column(modifier = Modifier.fillMaxWidth()) {
                if (uiState.resolutionStrs.isEmpty()) {
                    Text(
                        "No Source configurations available",
                        modifier = Modifier.padding(16.dp)
                    )
                } else {
                    LazyColumn(
                        modifier = Modifier.weight(1f),
                        contentPadding = PaddingValues(horizontal = 8.dp, vertical = 8.dp),
                    ) {
                        this.itemsIndexed(uiState.resolutionStrs) { index, item ->
                            val isSelected = index == uiState.selectedResolutionIdx
                            Box(
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .padding(4.dp)
                                    .clickable {
                                        viewModel.onResolutionSelected(index)
                                    }
                                    .background(
                                        if (isSelected)
                                            androidx.compose.ui.graphics.Color.LightGray
                                        else
                                            androidx.compose.ui.graphics.Color.Transparent
                                    )
                                    .padding(8.dp)
                            ) {
                                Text(text = item)
                            }
                        }
                    }
                    androidx.compose.material3.Button(
                        onClick = { navController.navigate(AusbcScreen.TestSourcePreview.name) },
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(16.dp)
                    ) {
                        Text("Continue")
                    }
                }
            }
        }
    }
}

