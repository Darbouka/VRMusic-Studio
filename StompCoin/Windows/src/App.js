import React from 'react';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import { ThemeProvider, createTheme } from '@mui/material/styles';
import CssBaseline from '@mui/material/CssBaseline';
import { Box } from '@mui/material';

// Komponenten
import Dashboard from './components/Dashboard';
import Fitness from './components/Fitness';
import Wallet from './components/Wallet';
import Social from './components/Social';
import Settings from './components/Settings';
import Login from './components/Login';
import Navigation from './components/Navigation';

// Theme
const theme = createTheme({
  palette: {
    mode: 'dark',
    primary: {
      main: '#2196f3',
    },
    secondary: {
      main: '#f50057',
    },
  },
});

function App() {
  const [isLoggedIn, setIsLoggedIn] = React.useState(false);
  const [userData, setUserData] = React.useState(null);

  React.useEffect(() => {
    // Benutzerdaten beim Start laden
    const loadUserData = async () => {
      const data = await window.electron.ipcRenderer.invoke('get-user-data');
      if (data) {
        setUserData(data);
        setIsLoggedIn(true);
      }
    };
    loadUserData();
  }, []);

  const handleLogin = async (credentials) => {
    try {
      // Hier würde die tatsächliche Login-Logik implementiert
      const userData = {
        id: '1',
        email: credentials.email,
        username: 'TestUser',
        walletBalance: 1000
      };
      
      await window.electron.ipcRenderer.invoke('set-user-data', userData);
      setUserData(userData);
      setIsLoggedIn(true);
    } catch (error) {
      console.error('Login error:', error);
    }
  };

  const handleLogout = async () => {
    await window.electron.ipcRenderer.invoke('set-user-data', null);
    setUserData(null);
    setIsLoggedIn(false);
  };

  if (!isLoggedIn) {
    return (
      <ThemeProvider theme={theme}>
        <CssBaseline />
        <Login onLogin={handleLogin} />
      </ThemeProvider>
    );
  }

  return (
    <ThemeProvider theme={theme}>
      <CssBaseline />
      <Router>
        <Box sx={{ display: 'flex' }}>
          <Navigation onLogout={handleLogout} />
          <Box component="main" sx={{ flexGrow: 1, p: 3 }}>
            <Routes>
              <Route path="/" element={<Dashboard userData={userData} />} />
              <Route path="/fitness" element={<Fitness />} />
              <Route path="/wallet" element={<Wallet userData={userData} />} />
              <Route path="/social" element={<Social />} />
              <Route path="/settings" element={<Settings />} />
            </Routes>
          </Box>
        </Box>
      </Router>
    </ThemeProvider>
  );
}

export default App; 