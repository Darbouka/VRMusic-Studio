const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');
const AWS = require('aws-sdk');
const moment = require('moment');

// Konfiguration
const BACKUP_DIR = process.env.BACKUP_DIR || '/var/backups/stompcoin';
const MONGODB_URI = process.env.MONGODB_URI;
const RETENTION_DAYS = 30;

// AWS S3 Konfiguration
const s3 = new AWS.S3({
    accessKeyId: process.env.AWS_ACCESS_KEY_ID,
    secretAccessKey: process.env.AWS_SECRET_ACCESS_KEY,
    region: process.env.AWS_REGION
});

// Backup erstellen
async function createBackup() {
    const timestamp = moment().format('YYYY-MM-DD_HH-mm');
    const backupPath = path.join(BACKUP_DIR, `backup_${timestamp}`);
    
    // Backup-Verzeichnis erstellen
    if (!fs.existsSync(BACKUP_DIR)) {
        fs.mkdirSync(BACKUP_DIR, { recursive: true });
    }

    // MongoDB Dump erstellen
    const dumpCommand = `mongodump --uri="${MONGODB_URI}" --out="${backupPath}"`;
    
    try {
        await executeCommand(dumpCommand);
        console.log(`Backup erstellt: ${backupPath}`);
        
        // Backup komprimieren
        const archivePath = `${backupPath}.tar.gz`;
        const compressCommand = `tar -czf "${archivePath}" -C "${BACKUP_DIR}" "backup_${timestamp}"`;
        await executeCommand(compressCommand);
        
        // Backup zu S3 hochladen
        await uploadToS3(archivePath, `backups/backup_${timestamp}.tar.gz`);
        
        // Lokales Backup löschen
        fs.rmSync(backupPath, { recursive: true });
        fs.unlinkSync(archivePath);
        
        // Alte Backups bereinigen
        await cleanupOldBackups();
        
        return true;
    } catch (error) {
        console.error('Backup-Fehler:', error);
        return false;
    }
}

// Backup zu S3 hochladen
async function uploadToS3(filePath, key) {
    const fileContent = fs.readFileSync(filePath);
    
    const params = {
        Bucket: process.env.AWS_BACKUP_BUCKET,
        Key: key,
        Body: fileContent
    };
    
    try {
        await s3.upload(params).promise();
        console.log(`Backup zu S3 hochgeladen: ${key}`);
    } catch (error) {
        console.error('S3 Upload-Fehler:', error);
        throw error;
    }
}

// Alte Backups bereinigen
async function cleanupOldBackups() {
    const params = {
        Bucket: process.env.AWS_BACKUP_BUCKET,
        Prefix: 'backups/'
    };
    
    try {
        const data = await s3.listObjects(params).promise();
        const oldBackups = data.Contents.filter(obj => {
            const backupDate = moment(obj.LastModified);
            return moment().diff(backupDate, 'days') > RETENTION_DAYS;
        });
        
        for (const backup of oldBackups) {
            await s3.deleteObject({
                Bucket: process.env.AWS_BACKUP_BUCKET,
                Key: backup.Key
            }).promise();
            console.log(`Altes Backup gelöscht: ${backup.Key}`);
        }
    } catch (error) {
        console.error('Cleanup-Fehler:', error);
        throw error;
    }
}

// Backup wiederherstellen
async function restoreBackup(backupKey) {
    const localPath = path.join(BACKUP_DIR, 'restore_temp.tar.gz');
    
    try {
        // Backup von S3 herunterladen
        const data = await s3.getObject({
            Bucket: process.env.AWS_BACKUP_BUCKET,
            Key: backupKey
        }).promise();
        
        fs.writeFileSync(localPath, data.Body);
        
        // Backup entpacken
        const extractCommand = `tar -xzf "${localPath}" -C "${BACKUP_DIR}"`;
        await executeCommand(extractCommand);
        
        // MongoDB Restore durchführen
        const restoreCommand = `mongorestore --uri="${MONGODB_URI}" --drop "${path.join(BACKUP_DIR, 'backup_*')}"`;
        await executeCommand(restoreCommand);
        
        // Temporäre Dateien löschen
        fs.unlinkSync(localPath);
        fs.rmSync(path.join(BACKUP_DIR, 'backup_*'), { recursive: true });
        
        console.log('Backup erfolgreich wiederhergestellt');
        return true;
    } catch (error) {
        console.error('Restore-Fehler:', error);
        return false;
    }
}

// Hilfsfunktion für Command-Ausführung
function executeCommand(command) {
    return new Promise((resolve, reject) => {
        exec(command, (error, stdout, stderr) => {
            if (error) {
                reject(error);
                return;
            }
            resolve(stdout);
        });
    });
}

// Skript ausführen
if (require.main === module) {
    const action = process.argv[2];
    const backupKey = process.argv[3];
    
    if (action === 'backup') {
        createBackup()
            .then(() => process.exit(0))
            .catch(() => process.exit(1));
    } else if (action === 'restore' && backupKey) {
        restoreBackup(backupKey)
            .then(() => process.exit(0))
            .catch(() => process.exit(1));
    } else {
        console.error('Ungültige Aktion. Verwenden Sie "backup" oder "restore <backup-key>"');
        process.exit(1);
    }
}

module.exports = {
    createBackup,
    restoreBackup
}; 