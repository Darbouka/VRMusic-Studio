package com.stompcoin.app.ui.components

import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.size
import androidx.compose.material.CircularProgressIndicator
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.Dp
import coil.compose.AsyncImagePainter
import coil.compose.rememberAsyncImagePainter
import coil.request.ImageRequest
import coil.size.Size

@Composable
fun LazyImage(
    imageUrl: String,
    contentDescription: String?,
    modifier: Modifier = Modifier,
    contentScale: ContentScale = ContentScale.Crop,
    loadingSize: Dp? = null
) {
    val painter = rememberAsyncImagePainter(
        model = ImageRequest.Builder(LocalContext.current)
            .data(imageUrl)
            .size(Size.ORIGINAL)
            .build()
    )

    Box(
        modifier = modifier,
        contentAlignment = Alignment.Center
    ) {
        when (painter.state) {
            is AsyncImagePainter.State.Loading -> {
                CircularProgressIndicator(
                    modifier = Modifier.size(loadingSize ?: Dp.Unspecified)
                )
            }
            is AsyncImagePainter.State.Error -> {
                // Fehlerbehandlung
            }
            else -> {
                Image(
                    painter = painter,
                    contentDescription = contentDescription,
                    modifier = Modifier.fillMaxSize(),
                    contentScale = contentScale
                )
            }
        }
    }
}

@Composable
fun LazyImageWithPlaceholder(
    imageUrl: String,
    contentDescription: String?,
    placeholderUrl: String,
    modifier: Modifier = Modifier,
    contentScale: ContentScale = ContentScale.Crop
) {
    var isLoading by remember { mutableStateOf(true) }
    var hasError by remember { mutableStateOf(false) }

    Box(modifier = modifier) {
        if (isLoading) {
            LazyImage(
                imageUrl = placeholderUrl,
                contentDescription = "Placeholder",
                modifier = Modifier.fillMaxSize(),
                contentScale = contentScale
            )
        }

        LazyImage(
            imageUrl = imageUrl,
            contentDescription = contentDescription,
            modifier = Modifier.fillMaxSize(),
            contentScale = contentScale,
            onLoadingComplete = { isLoading = false },
            onError = { hasError = true }
        )

        if (hasError) {
            // Fehlerbehandlung
        }
    }
}

@Composable
fun LazyImageWithRetry(
    imageUrl: String,
    contentDescription: String?,
    modifier: Modifier = Modifier,
    contentScale: ContentScale = ContentScale.Crop,
    maxRetries: Int = 3
) {
    var retryCount by remember { mutableStateOf(0) }
    var hasError by remember { mutableStateOf(false) }

    Box(modifier = modifier) {
        LazyImage(
            imageUrl = imageUrl,
            contentDescription = contentDescription,
            modifier = Modifier.fillMaxSize(),
            contentScale = contentScale,
            onError = {
                if (retryCount < maxRetries) {
                    retryCount++
                    // Retry logic
                } else {
                    hasError = true
                }
            }
        )

        if (hasError) {
            // Fehlerbehandlung
        }
    }
}

@Composable
fun LazyImageWithFade(
    imageUrl: String,
    contentDescription: String?,
    modifier: Modifier = Modifier,
    contentScale: ContentScale = ContentScale.Crop,
    fadeDuration: Int = 300
) {
    var isLoaded by remember { mutableStateOf(false) }

    Box(modifier = modifier) {
        LazyImage(
            imageUrl = imageUrl,
            contentDescription = contentDescription,
            modifier = Modifier.fillMaxSize(),
            contentScale = contentScale,
            onLoadingComplete = { isLoaded = true }
        )

        if (!isLoaded) {
            // Fade-in Animation
        }
    }
} 