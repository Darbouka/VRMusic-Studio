using System;
using System.IO;
using System.Text.Json;
using System.Threading.Tasks;

namespace StompCoin.Managers
{
    public class OfflineManager
    {
        private static OfflineManager _instance;
        private readonly NotificationManager _notificationManager;
        private const string OFFLINE_DATA_FILE = "offline_data.json";

        public static OfflineManager Instance
        {
            get
            {
                if (_instance == null)
                {
                    _instance = new OfflineManager();
                }
                return _instance;
            }
        }

        private OfflineManager()
        {
            _notificationManager = NotificationManager.Instance;
        }

        public async Task SaveDataLocallyAsync(object data)
        {
            try
            {
                string jsonData = JsonSerializer.Serialize(data);
                await File.WriteAllTextAsync(OFFLINE_DATA_FILE, jsonData);
                _notificationManager.ShowOfflineDataSavedNotification();
            }
            catch (Exception ex)
            {
                _notificationManager.ShowOfflineErrorNotification();
                Console.WriteLine($"Fehler beim Speichern der Offline-Daten: {ex.Message}");
            }
        }

        public async Task SyncWhenOnlineAsync()
        {
            try
            {
                if (!File.Exists(OFFLINE_DATA_FILE))
                {
                    return;
                }

                string jsonData = await File.ReadAllTextAsync(OFFLINE_DATA_FILE);
                var data = JsonSerializer.Deserialize<object>(jsonData);

                // Synchronisiere mit dem Server
                bool success = await APIManager.Instance.SyncDataAsync(data);
                if (success)
                {
                    File.Delete(OFFLINE_DATA_FILE);
                    _notificationManager.ShowOfflineSyncNotification();
                }
                else
                {
                    _notificationManager.ShowOfflineErrorNotification();
                }
            }
            catch (Exception ex)
            {
                _notificationManager.ShowOfflineErrorNotification();
                Console.WriteLine($"Fehler bei der Offline-Synchronisation: {ex.Message}");
            }
        }

        public void HandleConnectivityChange(bool isConnected)
        {
            if (isConnected)
            {
                _ = SyncWhenOnlineAsync();
            }
        }
    }
} 