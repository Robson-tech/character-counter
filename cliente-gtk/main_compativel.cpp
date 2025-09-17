// Cliente GTK - Versão CSS Corrigida para compatibilidade
#include <gtk/gtk.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <cstdlib>

class ClienteGTKCompativel {
private:
    GtkWidget *janela;
    GtkWidget *comboArquivos;
    GtkWidget *btnProcessar;
    GtkWidget *textResultados;
    GtkWidget *progressBar;
    GtkWidget *statusLabel;
    GtkWidget *iconStatus;
    std::vector<std::string> arquivosDisponiveis;

public:
    ClienteGTKCompativel() {
        criarInterface();
        aplicarCSS();
        carregarArquivos();
    }

    void criarInterface() {
        // Janela principal
        janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(janela), "🚀 Sistema Distribuído - Versão Otimizada");
        gtk_window_set_default_size(GTK_WINDOW(janela), 850, 600);
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
            "🚀 Sistema Distribuído Master-Slave"
            "</span>");
        gtk_widget_set_halign(titulo, GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(headerBox), titulo, FALSE, FALSE, 0);

        // Subtítulo
        GtkWidget *subtitulo = gtk_label_new(nullptr);
        gtk_label_set_markup(GTK_LABEL(subtitulo), 
            "<span size='11000' color='#ecf0f1'>"
            "Processamento paralelo com threads distribuídas"
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
        btnProcessar = gtk_button_new_with_label("⚡ PROCESSAR ARQUIVO SELECIONADO");
        gtk_widget_set_name(btnProcessar, "btn-principal");
        gtk_widget_set_size_request(btnProcessar, -1, 45);
        gtk_widget_set_sensitive(btnProcessar, FALSE);
        gtk_widget_set_halign(btnProcessar, GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(selecaoBox), btnProcessar, FALSE, FALSE, 10);

        // Status e progresso
        GtkWidget *statusFrame = gtk_frame_new("📊 Status do Sistema");
        gtk_widget_set_name(statusFrame, "status-frame");
        gtk_box_pack_start(GTK_BOX(contentContainer), statusFrame, FALSE, FALSE, 0);
        
        GtkWidget *statusBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_set_border_width(GTK_CONTAINER(statusBox), 15);
        gtk_container_add(GTK_CONTAINER(statusFrame), statusBox);

        // Status atual
        GtkWidget *statusRow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        iconStatus = gtk_label_new("⭕");
        gtk_widget_set_name(iconStatus, "status-icon");
        statusLabel = gtk_label_new("Sistema pronto para processamento");
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
        GtkWidget *resultFrame = gtk_frame_new("📋 Resultados da Análise Distribuída");
        gtk_widget_set_name(resultFrame, "result-frame");
        gtk_box_pack_start(GTK_BOX(contentContainer), resultFrame, TRUE, TRUE, 0);

        GtkWidget *resultBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_set_border_width(GTK_CONTAINER(resultBox), 20);
        gtk_container_add(GTK_CONTAINER(resultFrame), resultBox);

        // Área de texto
        GtkWidget *scrolled = gtk_scrolled_window_new(nullptr, nullptr);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                     GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_size_request(scrolled, -1, 180);

        textResultados = gtk_text_view_new();
        gtk_widget_set_name(textResultados, "text-area");
        gtk_text_view_set_editable(GTK_TEXT_VIEW(textResultados), FALSE);
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textResultados), GTK_WRAP_WORD);
        gtk_container_add(GTK_CONTAINER(scrolled), textResultados);
        gtk_box_pack_start(GTK_BOX(resultBox), scrolled, TRUE, TRUE, 0);

        // Texto inicial
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textResultados));
        gtk_text_buffer_set_text(buffer, 
            "🎯 SISTEMA DISTRIBUÍDO INICIALIZADO!\n\n"
            "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
            "  🏗️  ARQUITETURA MASTER-SLAVE ATIVA\n"
            "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
            "  🖥️  Servidor Mestre:     localhost:8080\n"
            "  📝  Escravo Letras:      localhost:8081\n"
            "  🔢  Escravo Números:     localhost:8082\n"
            "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n"
            "⚡ RECURSOS DISPONÍVEIS:\n"
            "   ✓ Processamento paralelo com múltiplas threads\n"
            "   ✓ Comunicação HTTP/REST entre containers Docker\n"
            "   ✓ Análise distribuída de caracteres e números\n"
            "   ✓ Interface gráfica GTK3 otimizada\n\n"
            "📋 COMO USAR:\n"
            "   1️⃣ Selecione um arquivo no dropdown acima\n"
            "   2️⃣ Clique no botão 'PROCESSAR ARQUIVO'\n"
            "   3️⃣ Acompanhe o progresso em tempo real\n"
            "   4️⃣ Visualize os resultados detalhados\n\n"
            "🔄 Aguardando seleção de arquivo para iniciar...", -1);

        // Conectar sinais
        g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), nullptr);
        g_signal_connect(comboArquivos, "changed", G_CALLBACK(onArquivoSelecionado), this);
        g_signal_connect(btnProcessar, "clicked", G_CALLBACK(onProcessarArquivo), this);
    }
};