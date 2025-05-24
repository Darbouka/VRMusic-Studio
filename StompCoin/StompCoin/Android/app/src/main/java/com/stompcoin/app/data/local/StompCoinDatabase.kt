package com.stompcoin.app.data.local

import androidx.room.*
import com.stompcoin.app.data.model.*
import kotlinx.coroutines.flow.Flow

@Database(
    entities = [
        User::class,
        Friend::class,
        FriendRequest::class,
        FriendActivity::class,
        Event::class,
        Challenge::class,
        Achievement::class,
        Transaction::class,
        StepCount::class
    ],
    version = 1,
    exportSchema = false
)
@TypeConverters(Converters::class)
abstract class StompCoinDatabase : RoomDatabase() {
    abstract fun userDao(): UserDao
    abstract fun friendDao(): FriendDao
    abstract fun eventDao(): EventDao
    abstract fun challengeDao(): ChallengeDao
    abstract fun achievementDao(): AchievementDao
    abstract fun transactionDao(): TransactionDao
    abstract fun stepCountDao(): StepCountDao
}

@Dao
interface UserDao {
    @Query("SELECT * FROM users")
    fun getAllUsers(): Flow<List<User>>

    @Query("SELECT * FROM users WHERE id = :userId")
    suspend fun getUserById(userId: String): User?

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insertUser(user: User)

    @Update
    suspend fun updateUser(user: User)

    @Delete
    suspend fun deleteUser(user: User)
}

@Dao
interface FriendDao {
    @Query("SELECT * FROM friends WHERE userId = :userId")
    fun getFriendsForUser(userId: String): Flow<List<Friend>>

    @Query("SELECT * FROM friend_requests WHERE receiverId = :userId AND status = 'PENDING'")
    fun getPendingRequestsForUser(userId: String): Flow<List<FriendRequest>>

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insertFriend(friend: Friend)

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insertFriendRequest(request: FriendRequest)

    @Update
    suspend fun updateFriendRequest(request: FriendRequest)

    @Delete
    suspend fun deleteFriend(friend: Friend)
}

@Dao
interface EventDao {
    @Query("SELECT * FROM events")
    fun getAllEvents(): Flow<List<Event>>

    @Query("SELECT * FROM events WHERE id = :eventId")
    suspend fun getEventById(eventId: String): Event?

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insertEvent(event: Event)

    @Update
    suspend fun updateEvent(event: Event)

    @Delete
    suspend fun deleteEvent(event: Event)
}

@Dao
interface ChallengeDao {
    @Query("SELECT * FROM challenges")
    fun getAllChallenges(): Flow<List<Challenge>>

    @Query("SELECT * FROM challenges WHERE id = :challengeId")
    suspend fun getChallengeById(challengeId: String): Challenge?

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insertChallenge(challenge: Challenge)

    @Update
    suspend fun updateChallenge(challenge: Challenge)

    @Delete
    suspend fun deleteChallenge(challenge: Challenge)
}

@Dao
interface AchievementDao {
    @Query("SELECT * FROM achievements WHERE userId = :userId")
    fun getAchievementsForUser(userId: String): Flow<List<Achievement>>

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insertAchievement(achievement: Achievement)

    @Update
    suspend fun updateAchievement(achievement: Achievement)
}

@Dao
interface TransactionDao {
    @Query("SELECT * FROM transactions WHERE userId = :userId")
    fun getTransactionsForUser(userId: String): Flow<List<Transaction>>

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insertTransaction(transaction: Transaction)

    @Update
    suspend fun updateTransaction(transaction: Transaction)
}

@Dao
interface StepCountDao {
    @Query("SELECT * FROM step_counts WHERE userId = :userId ORDER BY timestamp DESC")
    fun getStepCountsForUser(userId: String): Flow<List<StepCount>>

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun insertStepCount(stepCount: StepCount)

    @Update
    suspend fun updateStepCount(stepCount: StepCount)
}

class Converters {
    @TypeConverter
    fun fromTimestamp(value: Long?): java.time.LocalDateTime? {
        return value?.let { java.time.LocalDateTime.ofEpochSecond(it, 0, java.time.ZoneOffset.UTC) }
    }

    @TypeConverter
    fun dateToTimestamp(date: java.time.LocalDateTime?): Long? {
        return date?.toEpochSecond(java.time.ZoneOffset.UTC)
    }

    @TypeConverter
    fun fromString(value: String): java.util.UUID {
        return java.util.UUID.fromString(value)
    }

    @TypeConverter
    fun fromUUID(uuid: java.util.UUID): String {
        return uuid.toString()
    }
} 