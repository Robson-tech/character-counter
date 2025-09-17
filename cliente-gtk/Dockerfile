# Dockerfile para Cliente GTK
FROM ubuntu:20.04

# Evitar prompts interativos
ENV DEBIAN_FRONTEND=noninteractive

# Instalar dependências GTK e ferramentas de desenvolvimento
RUN apt-get update && apt-get install -y \
    build-essential \
    pkg-config \
    libgtk-3-dev \
    curl \
    xauth \
    x11-apps \
    && rm -rf /var/lib/apt/lists/*

# Criar diretório de trabalho
WORKDIR /app

# Copiar arquivos do projeto
COPY . .

# Compilar o cliente GTK (versão compatível)
RUN g++ -o cliente_gtk main_compativel.cpp `pkg-config --cflags --libs gtk+-3.0`

# Configurar X11
ENV DISPLAY=:0

# Comando de execução
CMD ["./cliente_gtk"]