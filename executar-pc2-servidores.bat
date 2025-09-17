@echo off
echo =========================================
echo    PC2 - EXECUTAR SERVIDORES APENAS
echo =========================================
echo.
echo Este script deve ser executado no PC2 (servidor)
echo.

echo [1/4] Verificando Docker...
docker --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ Docker não encontrado! Instale o Docker Desktop primeiro.
    pause
    exit /b 1
)
echo ✓ Docker encontrado

echo.
echo [2/4] Construindo imagens dos servidores...
docker-compose -f docker-compose-servidores.yml build

echo.
echo [3/4] Iniciando servidores...
docker-compose -f docker-compose-servidores.yml up -d

echo.
echo [4/4] Aguardando inicializacao...
timeout /t 8 >nul

echo.
echo ==========================================
echo    STATUS DOS SERVIDORES
echo ==========================================
docker-compose -f docker-compose-servidores.yml ps

echo.
echo ==========================================
echo    INFORMAÇÕES DE CONEXÃO
echo ==========================================
echo 🖥️  IP desta máquina (PC2):
for /f "tokens=2 delims=:" %%i in ('ipconfig ^| findstr /C:"IPv4"') do echo    %%i

echo.
echo 📡 Portas expostas:
echo    - Servidor Mestre:  8080
echo    - Escravo Letras:   8081  
echo    - Escravo Números:  8082

echo.
echo ✅ SERVIDORES PRONTOS!
echo.
echo O PC1 (cliente) pode agora conectar neste IP.
echo.
echo Para parar os servidores:
echo docker-compose -f docker-compose-servidores.yml down
echo.
pause