package com.stompcoin.app.data.paging

import androidx.paging.PagingSource
import androidx.paging.PagingState
import com.stompcoin.app.data.model.Transaction
import com.stompcoin.app.data.repository.TransactionRepository
import java.time.LocalDateTime
import javax.inject.Inject

class TransactionPagingSource @Inject constructor(
    private val transactionRepository: TransactionRepository,
    private val userId: String
) : PagingSource<Int, Transaction>() {

    override fun getRefreshKey(state: PagingState<Int, Transaction>): Int? {
        return state.anchorPosition?.let { anchorPosition ->
            state.closestPageToPosition(anchorPosition)?.prevKey?.plus(1)
                ?: state.closestPageToPosition(anchorPosition)?.nextKey?.minus(1)
        }
    }

    override suspend fun load(params: LoadParams<Int>): LoadResult<Int, Transaction> {
        return try {
            val page = params.key ?: 0
            val pageSize = params.loadSize

            val endDate = LocalDateTime.now()
            val startDate = endDate.minusMonths(1) // Letzter Monat

            val transactions = transactionRepository.getTransactionsForDateRange(
                userId = userId,
                startDate = startDate,
                endDate = endDate
            )

            val pagedTransactions = transactions
                .drop(page * pageSize)
                .take(pageSize)

            LoadResult.Page(
                data = pagedTransactions,
                prevKey = if (page == 0) null else page - 1,
                nextKey = if (pagedTransactions.isEmpty()) null else page + 1
            )
        } catch (e: Exception) {
            LoadResult.Error(e)
        }
    }
} 