package com.stompcoin.app.utils

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import dagger.hilt.android.qualifiers.ApplicationContext
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.io.ByteArrayOutputStream
import java.io.File
import java.io.FileOutputStream
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class ImageCompressor @Inject constructor(
    @ApplicationContext private val context: Context
) {
    companion object {
        private const val MAX_WIDTH = 1024
        private const val MAX_HEIGHT = 1024
        private const val QUALITY = 80
    }

    suspend fun compressImage(uri: Uri): File = withContext(Dispatchers.IO) {
        val inputStream = context.contentResolver.openInputStream(uri)
        val options = BitmapFactory.Options().apply {
            inJustDecodeBounds = true
        }
        
        BitmapFactory.decodeStream(inputStream, null, options)
        inputStream?.close()

        var width = options.outWidth
        var height = options.outHeight
        var inSampleSize = 1

        if (height > MAX_HEIGHT || width > MAX_WIDTH) {
            val halfHeight = height / 2
            val halfWidth = width / 2

            while (halfHeight / inSampleSize >= MAX_HEIGHT && halfWidth / inSampleSize >= MAX_WIDTH) {
                inSampleSize *= 2
            }
        }

        val newOptions = BitmapFactory.Options().apply {
            this.inSampleSize = inSampleSize
        }

        val newInputStream = context.contentResolver.openInputStream(uri)
        val bitmap = BitmapFactory.decodeStream(newInputStream, null, newOptions)
        newInputStream?.close()

        val compressedFile = File(context.cacheDir, "compressed_${System.currentTimeMillis()}.jpg")
        FileOutputStream(compressedFile).use { out ->
            bitmap?.compress(Bitmap.CompressFormat.JPEG, QUALITY, out)
        }

        bitmap?.recycle()
        compressedFile
    }

    suspend fun compressBitmap(bitmap: Bitmap): ByteArray = withContext(Dispatchers.IO) {
        val outputStream = ByteArrayOutputStream()
        bitmap.compress(Bitmap.CompressFormat.JPEG, QUALITY, outputStream)
        outputStream.toByteArray()
    }

    fun calculateInSampleSize(options: BitmapFactory.Options, reqWidth: Int, reqHeight: Int): Int {
        val height = options.outHeight
        val width = options.outWidth
        var inSampleSize = 1

        if (height > reqHeight || width > reqWidth) {
            val halfHeight = height / 2
            val halfWidth = width / 2

            while (halfHeight / inSampleSize >= reqHeight && halfWidth / inSampleSize >= reqWidth) {
                inSampleSize *= 2
            }
        }

        return inSampleSize
    }
} 