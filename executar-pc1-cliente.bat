@echo off
echo =========================================
echo    PC1 - EXECUTAR CLIENTE REMOTO
echo =========================================
echo.

set /p IP_SERVIDOR="Digite o IP do PC2 (servidor): "
if "%IP_SERVIDOR%"=="" (
    echo ❌ IP não fornecido!
    pause
    exit /b 1
)

echo.
echo Testando conectividade com %IP_SERVIDOR%...
ping -n 2 %IP_SERVIDOR% >nul
if %errorlevel% neq 0 (
    echo ⚠️  Não foi possível fazer ping no servidor
    echo    Verifique se o IP está correto e se o PC2 está online
    echo.
)

echo.
echo [1/5] Verificando XLaunch...
tasklist /FI "IMAGENAME eq vcxsrv.exe" | find /i "vcxsrv.exe" >nul
if %errorlevel% == 0 (
    echo ✓ XLaunch está rodando
) else (
    echo ! Iniciando XLaunch...
    start "" "C:\Program Files\VcXsrv\vcxsrv.exe" :0 -ac -terminate -lesspointer -multiwindow -clipboard -wgl
    timeout /t 3 >nul
)

echo.
echo [2/5] Construindo cliente remoto...
docker build -f cliente-gtk/Dockerfile-remoto -t cliente-gtk-remoto ./cliente-gtk
if %errorlevel% neq 0 (
    echo ❌ Erro ao construir a imagem Docker!
    pause
    exit /b 1
)

echo.
echo [3/5] Verificando conectividade com servidores...
echo Testando servidor mestre em %IP_SERVIDOR%:8080...
curl -s --connect-timeout 5 http://%IP_SERVIDOR%:8080/health >nul 2>&1
if %errorlevel% == 0 (
    echo ✓ Servidor mestre está respondendo
) else (
    echo ⚠️  Servidor mestre não está respondendo
    echo    Verifique se os servidores estão rodando no PC2
    echo    Continuando mesmo assim...
)

echo.
echo [4/5] Configurando display...
set DISPLAY=host.docker.internal:0

echo.
echo [5/5] Executando cliente remoto...
echo ==========================================
echo    CONECTANDO EM: %IP_SERVIDOR%
echo ==========================================
echo.
echo * Cliente executando localmente (PC1)
echo * Servidores em %IP_SERVIDOR% (PC2)
echo * Interface GTK com conexão remota
echo.

docker run --rm -it ^
    -e DISPLAY=%DISPLAY% ^
    -v "%cd%:/home/arquivos" ^
    cliente-gtk-remoto %IP_SERVIDOR%

if %errorlevel% neq 0 (
    echo ❌ Erro ao executar o cliente!
    echo    Verifique se o Docker está rodando
    echo    e se os servidores estão acessíveis.
)

echo.
echo Cliente encerrado. Pressione qualquer tecla...
pause >nul