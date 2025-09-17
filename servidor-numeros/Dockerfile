# Dockerfile para Servidor de Números
FROM gcc:11

# Instalar dependências
RUN apt-get update && apt-get install -y \
    cmake \
    pkg-config \
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
EXPOSE 8082

# Comando de execução
CMD ["./build/servidor_numeros"]