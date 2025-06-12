# Build-Stage
FROM node:18-alpine AS builder

WORKDIR /app

# Installiere Abhängigkeiten
COPY package*.json ./
RUN npm ci

# Kopiere Quellcode
COPY . .

# Baue Anwendung
RUN npm run build

# Production-Stage
FROM nginx:alpine

# Kopiere Nginx-Konfiguration
COPY deployment/frontend/nginx.conf /etc/nginx/conf.d/default.conf

# Kopiere Build-Artefakte
COPY --from=builder /app/dist /usr/share/nginx/html

# Erstelle SSL-Verzeichnis
RUN mkdir -p /etc/nginx/ssl

# Erstelle nicht-root Benutzer
RUN adduser -D -H -u 101 -s /sbin/nologin nginx

# Setze Berechtigungen
RUN chown -R nginx:nginx /usr/share/nginx/html && \
    chmod -R 755 /usr/share/nginx/html && \
    chown -R nginx:nginx /var/cache/nginx && \
    chown -R nginx:nginx /var/log/nginx && \
    chown -R nginx:nginx /etc/nginx/conf.d && \
    touch /var/run/nginx.pid && \
    chown -R nginx:nginx /var/run/nginx.pid

# Wechsle zu nicht-root Benutzer
USER nginx

# Exponiere Ports
EXPOSE 80 443

# Starte Nginx
CMD ["nginx", "-g", "daemon off;"] 