# Dockerfile para Servidor Mestre
FROM gcc:11

# Instalar dependências
RUN apt-get update && apt-get install -y \
    cmake \
    pkg-config \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Criar diretório de trabalho
WORKDIR /app

# Copiar arquivos do projeto
COPY . .

# Criar build directory
RUN mkdir -p build

# Compilar o projeto
RUN cd build && \
    cmake .. && \
    make

# Expor porta
EXPOSE 8080

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=40s --retries=3 \
    CMD curl -f http://localhost:8080/health || exit 1

# Comando de execução
CMD ["./build/servidor_mestre"]