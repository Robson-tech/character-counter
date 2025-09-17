// Cliente GTK para conexão remota - Aceita IP do servidor como parâmetro
#include <gtk/gtk.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <cstdlib>

class ClienteGTKRemoto {
private:
    GtkWidget *janela;
    GtkWidget *comboArquivos;
    GtkWidget *btnProcessar;
    GtkWidget *textResultados;
    GtkWidget *progressBar;
    GtkWidget *statusLabel;
    GtkWidget *iconStatus;
    GtkWidget *labelServidor;
    std::vector<std::string> arquivosDisponiveis;
    std::string ipServidor;

public:
    ClienteGTKRemoto(const std::string& ip) : ipServidor(ip) {
        criarInterface();
        aplicarCSS();
        carregarArquivos();
    }

    void criarInterface() {
        // Janela principal
        janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(janela), "🌐 Sistema Distribuído - Cliente Remoto");
        gtk_window_set_default_size(GTK_WINDOW(janela), 850, 650);
        gtk_window_set_position(GTK_WINDOW(janela), GTK_WIN_POS_CENTER);
        
        // Container principal
        GtkWidget *mainContainer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_container_add(GTK_CONTAINER(janela), mainContainer);
        
        // Header
        GtkWidget *headerBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_widget_set_name(headerBox, "header");
        gtk_container_set_border_width(GTK_CONTAINER(headerBox), 25);
        gtk_box_pack_start(GTK_BOX(mainContainer), headerBox, FALSE, FALSE, 0);

        // Título principal
        GtkWidget *titulo = gtk_label_new(nullptr);
        gtk_label_set_markup(GTK_LABEL(titulo), 
            "<span size='18000' weight='bold' color='white'>"
            "🌐 Sistema Distribuído - Cliente Remoto"
            "</span>");
        gtk_widget_set_halign(titulo, GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(headerBox), titulo, FALSE, FALSE, 0);

        // Info do servidor
        labelServidor = gtk_label_new(nullptr);
        std::string textoServidor = "<span size='12000' color='#ecf0f1'>🖥️ Conectando em: " + ipServidor + ":8080</span>";
        gtk_label_set_markup(GTK_LABEL(labelServidor), textoServidor.c_str());
        gtk_widget_set_halign(labelServidor, GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(headerBox), labelServidor, FALSE, FALSE, 0);

        // Subtítulo
        GtkWidget *subtitulo = gtk_label_new(nullptr);
        gtk_label_set_markup(GTK_LABEL(subtitulo), 
            "<span size='11000' color='#ecf0f1'>"
            "Cliente executando em PC1 | Servidores em PC2"
            "</span>");
        gtk_widget_set_halign(subtitulo, GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(headerBox), subtitulo, FALSE, FALSE, 0);

        // Container do conteúdo
        GtkWidget *contentContainer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
        gtk_widget_set_name(contentContainer, "content");
        gtk_container_set_border_width(GTK_CONTAINER(contentContainer), 25);
        gtk_box_pack_start(GTK_BOX(mainContainer), contentContainer, TRUE, TRUE, 0);

        // Card de seleção
        GtkWidget *selecaoFrame = gtk_frame_new("📁 Seleção de Arquivo");
        gtk_widget_set_name(selecaoFrame, "card-frame");
        gtk_box_pack_start(GTK_BOX(contentContainer), selecaoFrame, FALSE, FALSE, 0);
        
        GtkWidget *selecaoBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
        gtk_container_set_border_width(GTK_CONTAINER(selecaoBox), 20);
        gtk_container_add(GTK_CONTAINER(selecaoFrame), selecaoBox);

        // Dropdown
        GtkWidget *comboBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        GtkWidget *labelCombo = gtk_label_new("Arquivo:");
        gtk_widget_set_name(labelCombo, "label-texto");
        
        comboArquivos = gtk_combo_box_text_new();
        gtk_widget_set_name(comboArquivos, "combo-arquivo");
        gtk_widget_set_size_request(comboArquivos, 350, 35);
        
        gtk_box_pack_start(GTK_BOX(comboBox), labelCombo, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(comboBox), comboArquivos, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(selecaoBox), comboBox, FALSE, FALSE, 0);

        // Botão processar
        btnProcessar = gtk_button_new_with_label("⚡ PROCESSAR NO SERVIDOR REMOTO");
        gtk_widget_set_name(btnProcessar, "btn-principal");
        gtk_widget_set_size_request(btnProcessar, -1, 45);
        gtk_widget_set_sensitive(btnProcessar, FALSE);
        gtk_widget_set_halign(btnProcessar, GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(selecaoBox), btnProcessar, FALSE, FALSE, 10);

        // Status e progresso
        GtkWidget *statusFrame = gtk_frame_new("📊 Status da Conexão Remota");
        gtk_widget_set_name(statusFrame, "status-frame");
        gtk_box_pack_start(GTK_BOX(contentContainer), statusFrame, FALSE, FALSE, 0);
        
        GtkWidget *statusBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_set_border_width(GTK_CONTAINER(statusBox), 15);
        gtk_container_add(GTK_CONTAINER(statusFrame), statusBox);

        // Status atual
        GtkWidget *statusRow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        iconStatus = gtk_label_new("🌐");
        gtk_widget_set_name(iconStatus, "status-icon");
        statusLabel = gtk_label_new("Conectado ao servidor remoto");
        gtk_widget_set_name(statusLabel, "status-text");
        
        gtk_box_pack_start(GTK_BOX(statusRow), iconStatus, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(statusRow), statusLabel, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(statusBox), statusRow, FALSE, FALSE, 0);

        // Barra de progresso
        progressBar = gtk_progress_bar_new();
        gtk_widget_set_name(progressBar, "progress-bar");
        gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progressBar), TRUE);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressBar), "Aguardando...");
        gtk_widget_set_visible(progressBar, FALSE);
        gtk_widget_set_size_request(progressBar, -1, 25);
        gtk_box_pack_start(GTK_BOX(statusBox), progressBar, FALSE, FALSE, 0);

        // Resultados
        GtkWidget *resultFrame = gtk_frame_new("📋 Resultados do Processamento Remoto");
        gtk_widget_set_name(resultFrame, "result-frame");
        gtk_box_pack_start(GTK_BOX(contentContainer), resultFrame, TRUE, TRUE, 0);

        GtkWidget *resultBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_set_border_width(GTK_CONTAINER(resultBox), 20);
        gtk_container_add(GTK_CONTAINER(resultFrame), resultBox);

        // Área de texto
        GtkWidget *scrolled = gtk_scrolled_window_new(nullptr, nullptr);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                     GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_size_request(scrolled, -1, 200);

        textResultados = gtk_text_view_new();
        gtk_widget_set_name(textResultados, "text-area");
        gtk_text_view_set_editable(GTK_TEXT_VIEW(textResultados), FALSE);
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textResultados), GTK_WRAP_WORD);
        gtk_container_add(GTK_CONTAINER(scrolled), textResultados);
        gtk_box_pack_start(GTK_BOX(resultBox), scrolled, TRUE, TRUE, 0);

        // Texto inicial
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textResultados));
        std::string textoInicial = "🌐 SISTEMA DISTRIBUÍDO - CONFIGURAÇÃO REMOTA\n\n"
            "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
            "  🖥️  ARQUITETURA DISTRIBUÍDA EM DUAS MÁQUINAS\n"
            "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
            "  💻 PC1 (Cliente):        Interface GTK\n"
            "  🖥️  PC2 (Servidores):     " + ipServidor + "\n"
            "      ├── Servidor Mestre:     " + ipServidor + ":8080\n"
            "      ├── Escravo Letras:      " + ipServidor + ":8081\n"
            "      └── Escravo Números:     " + ipServidor + ":8082\n"
            "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n"
            "⚡ CONFIGURAÇÃO REMOTA ATIVA:\n"
            "   ✓ Cliente executando localmente (PC1)\n"
            "   ✓ Servidores rodando remotamente (PC2)\n"
            "   ✓ Comunicação HTTP através da rede\n"
            "   ✓ Processamento distribuído entre máquinas\n\n"
            "📋 COMO USAR:\n"
            "   1️⃣ Selecione um arquivo no dropdown\n"
            "   2️⃣ Clique 'PROCESSAR NO SERVIDOR REMOTO'\n"
            "   3️⃣ Aguarde o processamento via rede\n"
            "   4️⃣ Visualize resultados consolidados\n\n"
            "🔄 Pronto para processar arquivos remotamente...";
        
        gtk_text_buffer_set_text(buffer, textoInicial.c_str(), -1);

        // Conectar sinais
        g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), nullptr);
        g_signal_connect(comboArquivos, "changed", G_CALLBACK(onArquivoSelecionado), this);
        g_signal_connect(btnProcessar, "clicked", G_CALLBACK(onProcessarArquivo), this);
    }

    void aplicarCSS() {
        GtkCssProvider *provider = gtk_css_provider_new();
        const char *css = 
            "window { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); font-family: 'Arial', sans-serif; }\n"
            "#header { background: rgba(0, 0, 0, 0.15); border-radius: 0 0 15px 15px; }\n"
            "#content { background: rgba(255, 255, 255, 0.95); border-radius: 15px; box-shadow: 0 10px 30px rgba(0, 0, 0, 0.2); }\n"
            "#card-frame, #status-frame, #result-frame { background: white; border: 2px solid #e9ecef; border-radius: 12px; box-shadow: 0 4px 15px rgba(0, 0, 0, 0.1); margin: 5px; }\n"
            "#card-frame > label, #status-frame > label, #result-frame > label { color: white; font-weight: bold; font-size: 14px; padding: 5px 10px; background: linear-gradient(90deg, #e74c3c, #c0392b); border-radius: 6px; margin: 0 10px; }\n"
            "#label-texto { color: #495057; font-weight: bold; font-size: 14px; padding: 8px; }\n"
            "#combo-arquivo { border: 2px solid #dee2e6; border-radius: 8px; padding: 6px 12px; background: white; font-size: 13px; }\n"
            "#combo-arquivo:focus { border-color: #e74c3c; box-shadow: 0 0 8px rgba(231, 76, 60, 0.3); }\n"
            "#btn-principal { background: linear-gradient(135deg, #e74c3c 0%, #c0392b 100%); border: none; border-radius: 10px; color: white; font-weight: bold; font-size: 14px; padding: 12px 20px; box-shadow: 0 6px 20px rgba(231, 76, 60, 0.4); }\n"
            "#btn-principal:hover { background: linear-gradient(135deg, #dc3545 0%, #a71e2a 100%); box-shadow: 0 8px 25px rgba(231, 76, 60, 0.6); }\n"
            "#btn-principal:disabled { background: linear-gradient(135deg, #adb5bd 0%, #6c757d 100%); box-shadow: 0 3px 10px rgba(0, 0, 0, 0.2); color: #dee2e6; }\n"
            "#status-icon { font-size: 16px; padding: 5px; }\n"
            "#status-text { color: #495057; font-weight: 500; font-size: 13px; }\n"
            "#progress-bar { border-radius: 10px; background: #e9ecef; border: 1px solid #dee2e6; }\n"
            "#progress-bar trough { background: #e9ecef; border-radius: 8px; }\n"
            "#progress-bar progress { background: linear-gradient(90deg, #e74c3c 0%, #c0392b 100%); border-radius: 6px; }\n"
            "#text-area { font-family: 'Consolas', 'Monaco', monospace; font-size: 12px; background: linear-gradient(135deg, #2c3e50 0%, #34495e 100%); color: #ecf0f1; border-radius: 8px; padding: 15px; border: 1px solid #34495e; }\n"
            "#text-area text { background: transparent; color: #ecf0f1; }";
        
        gtk_css_provider_load_from_data(provider, css, -1, nullptr);
        gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                                 GTK_STYLE_PROVIDER(provider),
                                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        g_object_unref(provider);
    }

    void carregarArquivos() {
        arquivosDisponiveis.clear();
        
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboArquivos), "📋 Selecione um arquivo...");
        
        const char* pasta = "/home/arquivos";
        DIR* dir = opendir(pasta);
        if (dir) {
            struct dirent* entrada;
            while ((entrada = readdir(dir)) != nullptr) {
                std::string nome = entrada->d_name;
                if (nome != "." && nome != ".." && 
                    (nome.find(".txt") != std::string::npos ||
                     nome.find(".cpp") != std::string::npos ||
                     nome.find(".log") != std::string::npos ||
                     nome.find(".md") != std::string::npos)) {
                    
                    arquivosDisponiveis.push_back(nome);
                    
                    std::string icone = "📄";
                    if (nome.find(".txt") != std::string::npos) icone = "📝";
                    else if (nome.find(".cpp") != std::string::npos) icone = "💻";
                    else if (nome.find(".log") != std::string::npos) icone = "📋";
                    else if (nome.find(".md") != std::string::npos) icone = "📖";
                    
                    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboArquivos), 
                                                  (icone + " " + nome).c_str());
                }
            }
            closedir(dir);
        }
        
        if (arquivosDisponiveis.empty()) {
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboArquivos), 
                                          "❌ Nenhum arquivo encontrado");
            atualizarStatus("❌", "Nenhum arquivo disponível");
        } else {
            atualizarStatus("🌐", "Conectado a " + ipServidor + " - " + std::to_string(arquivosDisponiveis.size()) + " arquivo(s)");
        }
        
        gtk_combo_box_set_active(GTK_COMBO_BOX(comboArquivos), 0);
    }

    void atualizarStatus(const std::string& icone, const std::string& texto) {
        gtk_label_set_text(GTK_LABEL(iconStatus), icone.c_str());
        gtk_label_set_text(GTK_LABEL(statusLabel), texto.c_str());
    }

    static void onArquivoSelecionado(GtkWidget *widget, gpointer data) {
        ClienteGTKRemoto *cliente = static_cast<ClienteGTKRemoto*>(data);
        
        gint indice = gtk_combo_box_get_active(GTK_COMBO_BOX(cliente->comboArquivos));
        if (indice > 0 && indice <= cliente->arquivosDisponiveis.size()) {
            gtk_widget_set_sensitive(cliente->btnProcessar, TRUE);
            std::string arquivo = cliente->arquivosDisponiveis[indice - 1];
            cliente->atualizarStatus("✅", "Pronto para enviar: " + arquivo);
        } else {
            gtk_widget_set_sensitive(cliente->btnProcessar, FALSE);
            if (!cliente->arquivosDisponiveis.empty()) {
                cliente->atualizarStatus("🌐", "Conectado a " + cliente->ipServidor);
            }
        }
    }

    static void onProcessarArquivo(GtkWidget *widget, gpointer data) {
        ClienteGTKRemoto *cliente = static_cast<ClienteGTKRemoto*>(data);
        cliente->processarArquivoRemoto();
    }

    void processarArquivoRemoto() {
        gint indice = gtk_combo_box_get_active(GTK_COMBO_BOX(comboArquivos));
        if (indice <= 0 || indice > arquivosDisponiveis.size()) {
            mostrarMensagem("Erro", "Selecione um arquivo válido!");
            return;
        }

        std::string nomeArquivo = arquivosDisponiveis[indice - 1];
        std::string caminhoCompleto = "/home/arquivos/" + nomeArquivo;

        std::ifstream arquivo(caminhoCompleto);
        if (!arquivo.is_open()) {
            mostrarMensagem("Erro", "Não foi possível abrir: " + nomeArquivo);
            return;
        }

        std::string conteudo((std::istreambuf_iterator<char>(arquivo)),
                            std::istreambuf_iterator<char>());
        arquivo.close();

        if (conteudo.empty()) {
            mostrarMensagem("Aviso", "Arquivo vazio!");
            return;
        }

        processarRemoto(nomeArquivo, conteudo);
    }

    void processarRemoto(const std::string& nomeArquivo, const std::string& conteudo) {
        gtk_widget_set_sensitive(btnProcessar, FALSE);
        gtk_widget_set_visible(progressBar, TRUE);
        
        atualizarStatus("📡", "Enviando para servidor remoto...");
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 0.2);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressBar), "Conectando via rede...");
        atualizarUI();
        g_usleep(500000);

        // Criar JSON
        std::ofstream jsonFile("/tmp/request.json");
        jsonFile << "{\n";
        jsonFile << "  \"nome_arquivo\": \"" << nomeArquivo << "\",\n";
        jsonFile << "  \"conteudo\": \"" << escapeJson(conteudo) << "\"\n";
        jsonFile << "}";
        jsonFile.close();

        atualizarStatus("🌐", "Processamento remoto iniciado...");
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 0.5);
        std::string progressText = "Executando em " + ipServidor + "...";
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressBar), progressText.c_str());
        atualizarUI();

        // Curl para servidor remoto
        std::string url = "http://" + ipServidor + ":8080/processar";
        std::string comando = "curl -s -X POST " + url + " "
                            "-H \"Content-Type: application/json\" "
                            "-d @/tmp/request.json > /tmp/response.json 2>&1";
        
        system(comando.c_str());

        atualizarStatus("📊", "Recebendo resultados...");
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 0.9);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressBar), "Consolidando...");
        atualizarUI();
        g_usleep(300000);

        // Ler resposta
        std::ifstream resposta("/tmp/response.json");
        std::string jsonResposta((std::istreambuf_iterator<char>(resposta)),
                                std::istreambuf_iterator<char>());
        resposta.close();

        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 1.0);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressBar), "✅ Concluído!");
        atualizarStatus("✅", "Processamento remoto concluído!");

        mostrarResultadosRemotos(jsonResposta, nomeArquivo);
        
        gtk_widget_set_sensitive(btnProcessar, TRUE);
        g_timeout_add(2500, esconderProgressBar, progressBar);
    }

    void mostrarResultadosRemotos(const std::string& jsonResposta, const std::string& arquivo) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textResultados));
        
        std::string resultado = "╔══════════════════════════════════════════════════════════╗\n";
        resultado += "║           🌐 PROCESSAMENTO REMOTO CONCLUÍDO              ║\n";
        resultado += "╚══════════════════════════════════════════════════════════╝\n\n";
        resultado += "📁 ARQUIVO: " + arquivo + "\n";
        resultado += "🖥️  SERVIDOR: " + ipServidor + "\n";
        resultado += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
        
        if (jsonResposta.find("erro") != std::string::npos || jsonResposta.empty() || 
            jsonResposta.find("Connection refused") != std::string::npos) {
            resultado += "❌ ERRO DE CONEXÃO REMOTA:\n\n";
            resultado += jsonResposta + "\n\n";
            resultado += "🔧 VERIFICAÇÕES NECESSÁRIAS:\n";
            resultado += "   • Servidor PC2 (" + ipServidor + ") está rodando?\n";
            resultado += "   • Containers ativos: docker-compose ps\n";
            resultado += "   • Portas abertas: 8080, 8081, 8082\n";
            resultado += "   • Firewall permite conexões?\n";
            resultado += "   • Rede entre PC1 e PC2 funcionando?\n";
        } else {
            resultado += "🏗️  EXECUÇÃO DISTRIBUÍDA ENTRE MÁQUINAS:\n\n";
            resultado += "┌─────────────────────────────────────────────────────────┐\n";
            resultado += "│  💻 PC1 (Cliente)    →  Interface GTK local           │\n";
            resultado += "│  🖥️  PC2 (Servidores) →  " + ipServidor + "                    │\n";
            resultado += "│     ├── Mestre       →  :8080 (coordenação)           │\n";
            resultado += "│     ├── Letras       →  :8081 (processamento)         │\n";
            resultado += "│     └── Números      →  :8082 (processamento)         │\n";
            resultado += "└─────────────────────────────────────────────────────────┘\n\n";
            
            // Parse dos resultados
            size_t pos;
            
            pos = jsonResposta.find("\"letras\"");
            if (pos != std::string::npos) {
                size_t start = jsonResposta.find(":", pos) + 1;
                size_t end = jsonResposta.find(",", start);
                if (end == std::string::npos) end = jsonResposta.find("}", start);
                std::string letras = jsonResposta.substr(start, end - start);
                letras.erase(0, letras.find_first_not_of(" \t"));
                letras.erase(letras.find_last_not_of(" \t") + 1);
                
                resultado += "🔤 RESULTADO - SERVIDOR LETRAS (" + ipServidor + ":8081):\n";
                resultado += "   📊 Letras encontradas: " + letras + "\n";
                resultado += "   🌐 Comunicação HTTP remota realizada\n\n";
            }
            
            pos = jsonResposta.find("\"numeros\"");
            if (pos != std::string::npos) {
                size_t start = jsonResposta.find(":", pos) + 1;
                size_t end = jsonResposta.find(",", start);
                if (end == std::string::npos) end = jsonResposta.find("}", start);
                std::string numeros = jsonResposta.substr(start, end - start);
                numeros.erase(0, numeros.find_first_not_of(" \t"));
                numeros.erase(numeros.find_last_not_of(" \t") + 1);
                
                resultado += "🔢 RESULTADO - SERVIDOR NÚMEROS (" + ipServidor + ":8082):\n";
                resultado += "   📊 Dígitos encontrados: " + numeros + "\n";
                resultado += "   🌐 Comunicação HTTP remota realizada\n\n";
            }
            
            pos = jsonResposta.find("\"tempo_ms\"");
            if (pos != std::string::npos) {
                size_t start = jsonResposta.find(":", pos) + 1;
                size_t end = jsonResposta.find(",", start);
                if (end == std::string::npos) end = jsonResposta.find("}", start);
                std::string tempo = jsonResposta.substr(start, end - start);
                tempo.erase(0, tempo.find_first_not_of(" \t"));
                tempo.erase(tempo.find_last_not_of(" \t") + 1);
                
                resultado += "⏱️  MÉTRICAS DE REDE:\n";
                resultado += "   🚀 Tempo total (incluindo rede): " + tempo + " ms\n";
                resultado += "   📡 Protocolo: HTTP/REST via TCP/IP\n";
                resultado += "   🌐 Distribuição: PC1 ↔ PC2\n\n";
            }
        }
        
        resultado += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        resultado += "🎉 SISTEMA DISTRIBUÍDO MULTI-MÁQUINA EXECUTADO!\n";
        resultado += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
        resultado += "💻 Cliente: GTK3 executando localmente\n";
        resultado += "🖥️  Servidores: Docker containers em " + ipServidor + "\n";
        resultado += "🌐 Comunicação: HTTP/REST através da rede\n";
        resultado += "🏗️  Arquitetura: Master-Slave distribuída entre PCs";
        
        gtk_text_buffer_set_text(buffer, resultado.c_str(), -1);
    }

    void atualizarUI() {
        while (gtk_events_pending()) {
            gtk_main_iteration();
        }
    }

    static gboolean esconderProgressBar(gpointer data) {
        GtkWidget *progressBar = static_cast<GtkWidget*>(data);
        gtk_widget_set_visible(progressBar, FALSE);
        return FALSE;
    }

    std::string escapeJson(const std::string& input) {
        std::string output;
        for (char c : input) {
            switch (c) {
                case '"': output += "\\\""; break;
                case '\\': output += "\\\\"; break;
                case '\n': output += "\\n"; break;
                case '\r': output += "\\r"; break;
                case '\t': output += "\\t"; break;
                default: output += c; break;
            }
        }
        return output;
    }

    void mostrarMensagem(const std::string& titulo, const std::string& mensagem) {
        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(janela),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "%s", mensagem.c_str());
        
        gtk_window_set_title(GTK_WINDOW(dialog), titulo.c_str());
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }

    void executar() {
        gtk_widget_show_all(janela);
        gtk_main();
    }
};

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    std::string ipServidor = "192.168.1.100"; // IP padrão
    
    // Verificar se foi fornecido IP como argumento
    if (argc > 1) {
        ipServidor = argv[1];
    } else {
        std::cout << "Uso: " << argv[0] << " <IP_DO_SERVIDOR>\n";
        std::cout << "Usando IP padrão: " << ipServidor << "\n";
    }
    
    std::cout << "Conectando ao servidor: " << ipServidor << "\n";
    
    ClienteGTKRemoto cliente(ipServidor);
    cliente.executar();
    
    return 0;
}