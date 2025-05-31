using System;
using System.Collections.ObjectModel;
using System.Windows.Controls;

namespace StompCoin.Views
{
    public partial class HomePage : Page
    {
        public ObservableCollection<Activity> Activities { get; set; }

        public HomePage()
        {
            InitializeComponent();
            LoadData();
        }

        private void LoadData()
        {
            // Beispiel-Daten laden
            Activities = new ObservableCollection<Activity>
            {
                new Activity
                {
                    Name = "Täglicher Spaziergang",
                    Description = "Gehen Sie 10.000 Schritte am Tag",
                    Reward = "100 STOMP"
                },
                new Activity
                {
                    Name = "Wöchentliche Challenge",
                    Description = "Erreichen Sie 50.000 Schritte in einer Woche",
                    Reward = "500 STOMP"
                },
                new Activity
                {
                    Name = "Monatliches Ziel",
                    Description = "Erreichen Sie 200.000 Schritte in einem Monat",
                    Reward = "2000 STOMP"
                }
            };

            ActivityList.ItemsSource = Activities;

            // Beispiel-Statistiken
            LevelValue.Text = "42";
            WalletValue.Text = "1,234 STOMP";
            XpValue.Text = "12,345";
            StepsValue.Text = "8,547";
            CaloriesValue.Text = "423";
            DistanceValue.Text = "6.2 km";
        }
    }

    public class Activity
    {
        public string Name { get; set; }
        public string Description { get; set; }
        public string Reward { get; set; }
    }
} 