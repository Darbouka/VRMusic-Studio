import React from 'react';
import { Grid, Paper, Typography, Box } from '@mui/material';
import { styled } from '@mui/material/styles';
import TrendingUpIcon from '@mui/icons-material/TrendingUp';
import DirectionsWalkIcon from '@mui/icons-material/DirectionsWalk';
import LocalFireDepartmentIcon from '@mui/icons-material/LocalFireDepartment';
import StraightenIcon from '@mui/icons-material/Straighten';

const StatCard = styled(Paper)(({ theme }) => ({
  padding: theme.spacing(3),
  display: 'flex',
  flexDirection: 'column',
  alignItems: 'center',
  gap: theme.spacing(1),
}));

const IconWrapper = styled(Box)(({ theme }) => ({
  backgroundColor: theme.palette.primary.main,
  borderRadius: '50%',
  padding: theme.spacing(1),
  display: 'flex',
  alignItems: 'center',
  justifyContent: 'center',
}));

function Dashboard({ userData }) {
  const [fitnessData, setFitnessData] = React.useState({
    steps: 0,
    calories: 0,
    distance: 0
  });

  React.useEffect(() => {
    const loadFitnessData = async () => {
      const data = await window.electron.ipcRenderer.invoke('get-fitness-data');
      if (data) {
        setFitnessData(data);
      }
    };
    loadFitnessData();
  }, []);

  return (
    <Box>
      <Typography variant="h4" gutterBottom>
        Willkommen, {userData?.username}
      </Typography>

      <Grid container spacing={3}>
        <Grid item xs={12} md={4}>
          <StatCard>
            <IconWrapper>
              <DirectionsWalkIcon />
            </IconWrapper>
            <Typography variant="h6">Schritte</Typography>
            <Typography variant="h4">{fitnessData.steps}</Typography>
          </StatCard>
        </Grid>

        <Grid item xs={12} md={4}>
          <StatCard>
            <IconWrapper>
              <LocalFireDepartmentIcon />
            </IconWrapper>
            <Typography variant="h6">Kalorien</Typography>
            <Typography variant="h4">{Math.round(fitnessData.calories)}</Typography>
          </StatCard>
        </Grid>

        <Grid item xs={12} md={4}>
          <StatCard>
            <IconWrapper>
              <StraightenIcon />
            </IconWrapper>
            <Typography variant="h6">Distanz</Typography>
            <Typography variant="h4">{fitnessData.distance.toFixed(2)} km</Typography>
          </StatCard>
        </Grid>

        <Grid item xs={12}>
          <Paper sx={{ p: 3 }}>
            <Box display="flex" alignItems="center" gap={2}>
              <TrendingUpIcon color="primary" />
              <Typography variant="h6">Wallet Balance</Typography>
            </Box>
            <Typography variant="h3" sx={{ mt: 2 }}>
              {userData?.walletBalance.toFixed(2)} STOMP
            </Typography>
          </Paper>
        </Grid>
      </Grid>
    </Box>
  );
}

export default Dashboard; 