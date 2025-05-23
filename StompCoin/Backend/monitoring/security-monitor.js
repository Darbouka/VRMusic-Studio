const winston = require('winston');
const { createLogger, format, transports } = winston;
const { combine, timestamp, json } = format;
const fs = require('fs');
const path = require('path');
const nodemailer = require('nodemailer');
const { promisify } = require('util');
const exec = promisify(require('child_process').exec);

// Logger Konfiguration
const logger = createLogger({
    format: combine(
        timestamp(),
        json()
    ),
    transports: [
        new transports.File({ filename: 'logs/security.log' }),
        new transports.Console()
    ]
});

// E-Mail-Konfiguration
const transporter = nodemailer.createTransport({
    host: process.env.SMTP_HOST,
    port: process.env.SMTP_PORT,
    secure: true,
    auth: {
        user: process.env.SMTP_USER,
        pass: process.env.SMTP_PASS
    }
});

// Sicherheits-Checks
class SecurityMonitor {
    constructor() {
        this.alertThresholds = {
            failedLogins: 5,
            suspiciousIPs: 3,
            apiErrors: 100,
            memoryUsage: 85,
            cpuUsage: 80
        };
    }

    // System-Integritäts-Check
    async checkSystemIntegrity() {
        try {
            // Überprüfe kritische Dateien
            const criticalFiles = [
                'config/security.js',
                'middleware/security.js',
                'server.js'
            ];

            for (const file of criticalFiles) {
                const filePath = path.join(__dirname, '..', file);
                const stats = await fs.promises.stat(filePath);
                const fileHash = await this.calculateFileHash(filePath);
                
                // Überprüfe Änderungen
                if (this.hasFileChanged(file, fileHash)) {
                    await this.sendAlert('SYSTEM_INTEGRITY', `Kritische Datei geändert: ${file}`);
                }
            }
        } catch (error) {
            logger.error('System-Integritäts-Check fehlgeschlagen', { error });
        }
    }

    // Netzwerk-Sicherheits-Check
    async checkNetworkSecurity() {
        try {
            // Überprüfe offene Ports
            const { stdout } = await exec('netstat -tuln');
            const openPorts = stdout.split('\n')
                .filter(line => line.includes('LISTEN'))
                .map(line => line.split(/\s+/)[3]);

            // Überprüfe auf unerwartete Ports
            const allowedPorts = ['3000', '27017']; // Erlaubte Ports
            const unexpectedPorts = openPorts.filter(port => !allowedPorts.includes(port));
            
            if (unexpectedPorts.length > 0) {
                await this.sendAlert('NETWORK_SECURITY', `Unerwartete offene Ports: ${unexpectedPorts.join(', ')}`);
            }
        } catch (error) {
            logger.error('Netzwerk-Sicherheits-Check fehlgeschlagen', { error });
        }
    }

    // Datenbank-Sicherheits-Check
    async checkDatabaseSecurity() {
        try {
            // Überprüfe Datenbank-Zugriffe
            const dbLogs = await this.getDatabaseLogs();
            const suspiciousQueries = this.analyzeDatabaseQueries(dbLogs);
            
            if (suspiciousQueries.length > 0) {
                await this.sendAlert('DATABASE_SECURITY', `Verdächtige Datenbankabfragen erkannt: ${suspiciousQueries.length}`);
            }
        } catch (error) {
            logger.error('Datenbank-Sicherheits-Check fehlgeschlagen', { error });
        }
    }

    // Anwendungs-Sicherheits-Check
    async checkApplicationSecurity() {
        try {
            // Überprüfe API-Zugriffe
            const apiLogs = await this.getApiLogs();
            const suspiciousRequests = this.analyzeApiRequests(apiLogs);
            
            if (suspiciousRequests.length > 0) {
                await this.sendAlert('APPLICATION_SECURITY', `Verdächtige API-Anfragen erkannt: ${suspiciousRequests.length}`);
            }
        } catch (error) {
            logger.error('Anwendungs-Sicherheits-Check fehlgeschlagen', { error });
        }
    }

    // Hilfsfunktionen
    async calculateFileHash(filePath) {
        const { stdout } = await exec(`sha256sum ${filePath}`);
        return stdout.split(' ')[0];
    }

    hasFileChanged(file, hash) {
        // Implementiere Hash-Vergleich mit gespeicherten Werten
        return false; // Placeholder
    }

    async getDatabaseLogs() {
        // Implementiere Datenbank-Log-Abruf
        return []; // Placeholder
    }

    async getApiLogs() {
        // Implementiere API-Log-Abruf
        return []; // Placeholder
    }

    analyzeDatabaseQueries(logs) {
        // Implementiere Analyse von Datenbankabfragen
        return []; // Placeholder
    }

    analyzeApiRequests(logs) {
        // Implementiere Analyse von API-Anfragen
        return []; // Placeholder
    }

    // Alert-System
    async sendAlert(type, message) {
        const alert = {
            type,
            message,
            timestamp: new Date().toISOString(),
            severity: this.determineSeverity(type)
        };

        // Logge Alert
        logger.warn('Sicherheitsalert', alert);

        // Sende E-Mail bei kritischen Alerts
        if (alert.severity === 'CRITICAL') {
            await this.sendEmailAlert(alert);
        }

        // Speichere Alert
        await this.storeAlert(alert);
    }

    determineSeverity(type) {
        const severityMap = {
            'SYSTEM_INTEGRITY': 'CRITICAL',
            'NETWORK_SECURITY': 'HIGH',
            'DATABASE_SECURITY': 'HIGH',
            'APPLICATION_SECURITY': 'MEDIUM'
        };
        return severityMap[type] || 'LOW';
    }

    async sendEmailAlert(alert) {
        const mailOptions = {
            from: process.env.ALERT_EMAIL_FROM,
            to: process.env.ALERT_EMAIL_TO,
            subject: `Sicherheitsalert: ${alert.type}`,
            text: JSON.stringify(alert, null, 2)
        };

        try {
            await transporter.sendMail(mailOptions);
        } catch (error) {
            logger.error('E-Mail-Versand fehlgeschlagen', { error });
        }
    }

    async storeAlert(alert) {
        // Implementiere Alert-Speicherung
        // z.B. in Datenbank oder Datei
    }
}

// Monitor-Instanz erstellen und starten
const monitor = new SecurityMonitor();

// Regelmäßige Checks durchführen
setInterval(async () => {
    await monitor.checkSystemIntegrity();
    await monitor.checkNetworkSecurity();
    await monitor.checkDatabaseSecurity();
    await monitor.checkApplicationSecurity();
}, 5 * 60 * 1000); // Alle 5 Minuten

module.exports = monitor; 