using System;
using System.Threading.Tasks;
using Windows.Devices.Sensors;

namespace StompCoin.Managers
{
    public class StepCounterManager
    {
        private static StepCounterManager _instance;
        private readonly Pedometer _pedometer;
        private bool _isCounting;

        public static StepCounterManager Instance
        {
            get
            {
                if (_instance == null)
                {
                    _instance = new StepCounterManager();
                }
                return _instance;
            }
        }

        private StepCounterManager()
        {
            _pedometer = Pedometer.GetDefault();
        }

        public async Task StartCountingAsync()
        {
            if (_pedometer == null)
            {
                NotificationManager.Instance.ShowStepCounterErrorNotification("Schrittzähler nicht verfügbar");
                return;
            }

            _isCounting = true;
            _pedometer.ReadingChanged += Pedometer_ReadingChanged;
        }

        public void StopCounting()
        {
            _isCounting = false;
            if (_pedometer != null)
            {
                _pedometer.ReadingChanged -= Pedometer_ReadingChanged;
            }
        }

        private void Pedometer_ReadingChanged(Pedometer sender, PedometerReadingChangedEventArgs args)
        {
            if (!_isCounting) return;

            var steps = args.Reading.CumulativeSteps;
            NotificationManager.Instance.ShowStepsUpdatedNotification(steps);
        }

        public async Task<int> GetTodayStepsAsync()
        {
            if (_pedometer == null)
            {
                return 0;
            }

            try
            {
                var reading = await _pedometer.GetCurrentReadingAsync();
                return (int)reading.CumulativeSteps;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Fehler beim Abrufen der Schritte: {ex.Message}");
                return 0;
            }
        }

        public bool IsStepCountingAvailable()
        {
            return _pedometer != null;
        }
    }
} 