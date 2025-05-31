using System.Windows;
using StompCoin.Views;

namespace StompCoin
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            MainFrame.Navigate(new HomePage());
        }

        private void HomeButton_Click(object sender, RoutedEventArgs e)
        {
            MainFrame.Navigate(new HomePage());
        }

        private void MusicButton_Click(object sender, RoutedEventArgs e)
        {
            // TODO: Implement MusicPage
            MessageBox.Show("Musik-Seite wird noch implementiert");
        }

        private void LeaderboardButton_Click(object sender, RoutedEventArgs e)
        {
            // TODO: Implement LeaderboardPage
            MessageBox.Show("Ranglisten-Seite wird noch implementiert");
        }

        private void SettingsButton_Click(object sender, RoutedEventArgs e)
        {
            // TODO: Implement SettingsPage
            MessageBox.Show("Einstellungen-Seite wird noch implementiert");
        }

        private void ProfileButton_Click(object sender, RoutedEventArgs e)
        {
            // TODO: Implement ProfilePage
            MessageBox.Show("Profil-Seite wird noch implementiert");
        }

        private void WalletButton_Click(object sender, RoutedEventArgs e)
        {
            // TODO: Implement WalletPage
            MessageBox.Show("Wallet-Seite wird noch implementiert");
        }

        private void LogoutButton_Click(object sender, RoutedEventArgs e)
        {
            var result = MessageBox.Show(
                "Möchten Sie sich wirklich abmelden?",
                "Abmelden",
                MessageBoxButton.YesNo,
                MessageBoxImage.Question);

            if (result == MessageBoxResult.Yes)
            {
                // TODO: Implement logout logic
                MessageBox.Show("Abmelden wird noch implementiert");
            }
        }
    }
} 