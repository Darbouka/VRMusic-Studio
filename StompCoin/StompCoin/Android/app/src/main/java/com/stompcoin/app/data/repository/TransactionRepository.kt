package com.stompcoin.app.data.repository

import com.stompcoin.app.data.model.Transaction
import java.time.LocalDateTime

interface TransactionRepository {
    suspend fun createTransaction(transaction: Transaction)
    suspend fun getTransaction(transactionId: String): Transaction
    suspend fun getTransactionsForUser(userId: String): List<Transaction>
    suspend fun getTransactionsForDateRange(
        userId: String,
        startDate: LocalDateTime,
        endDate: LocalDateTime
    ): List<Transaction>
    suspend fun updateTransaction(transaction: Transaction)
    suspend fun deleteTransaction(transactionId: String)
} 