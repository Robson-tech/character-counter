// servidor_mestre.cpp
#include <iostream>
#include <string>
#include <thread>
#include <future>
#include <chrono>
#include <sstream>
#include <httplib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ServidorMestre {
private:
    httplib::Server servidor;
    std::string host_letras = "servidor-letras";
    std::string host_numeros = "servidor-numeros";
    int porta_letras = 8081;
    int porta_numeros = 8082;

    // Função para verificar saúde dos escravos
    bool verificarSaude(const std::string& host, int porta) {
        httplib::Client client(host, porta);
        client.set_connection_timeout(2, 0);
        client.set_read_timeout(5, 0);
        
        auto res = client.Get("/health");
        return res && res->status == 200;
    }

    // Função assíncrona para chamar escravo
    std::future<json> chamarEscravo(const std::string& host, int porta, 
                                   const std::string& endpoint, const std::string& texto) {
        return std::async(std::launch::async, [=]() -> json {
            try {
                httplib::Client client(host, porta);
                client.set_connection_timeout(5, 0);
                client.set_read_timeout(30, 0);
                
                json requisicao;
                requisicao["texto"] = texto;
                
                auto res = client.Post(endpoint.c_str(), requisicao.dump(), "application/json");
                
                if (res && res->status == 200) {
                    return json::parse(res->body);
                } else {
                    json erro;
                    erro["erro"] = true;
                    erro["mensagem"] = "Falha na comunicação";
                    return erro;
                }
            } catch (const std::exception& e) {
                json erro;
                erro["erro"] = true;
                erro["mensagem"] = e.what();
                return erro;
            }
        });
    }

public:
    void iniciar() {
        // Health check
        servidor.Get("/health", [](const httplib::Request&, httplib::Response& res) {
            json resposta;
            resposta["status"] = "saudavel";
            resposta["servico"] = "mestre";
            res.set_content(resposta.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        });

        // Status dos escravos
        servidor.Get("/status", [this](const httplib::Request&, httplib::Response& res) {
            json resposta;
            resposta["mestre"] = "ativo";
            resposta["escravos"]["letras"] = verificarSaude(host_letras, porta_letras);
            resposta["escravos"]["numeros"] = verificarSaude(host_numeros, porta_numeros);
            
            res.set_content(resposta.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        });

        // Endpoint principal para processar arquivo
        servidor.Post("/processar", [this](const httplib::Request& req, httplib::Response& res) {
            try {
                json requisicao = json::parse(req.body);
                std::string nome_arquivo = requisicao["nome_arquivo"];
                std::string conteudo = requisicao["conteudo"];

                std::cout << "📁 Processando: " << nome_arquivo 
                         << " (" << conteudo.length() << " caracteres)" << std::endl;

                // Verificar se escravos estão ativos
                if (!verificarSaude(host_letras, porta_letras)) {
                    json erro;
                    erro["erro"] = "Servidor de letras indisponível";
                    res.status = 503;
                    res.set_content(erro.dump(), "application/json");
                    res.set_header("Access-Control-Allow-Origin", "*");
                    return;
                }

                if (!verificarSaude(host_numeros, porta_numeros)) {
                    json erro;
                    erro["erro"] = "Servidor de números indisponível";
                    res.status = 503;
                    res.set_content(erro.dump(), "application/json");
                    res.set_header("Access-Control-Allow-Origin", "*");
                    return;
                }

                auto inicio = std::chrono::high_resolution_clock::now();

                // Disparar threads paralelas
                std::cout << "🚀 Iniciando threads paralelas..." << std::endl;
                auto future_letras = chamarEscravo(host_letras, porta_letras, "/letras", conteudo);
                auto future_numeros = chamarEscravo(host_numeros, porta_numeros, "/numeros", conteudo);

                // Aguardar resultados
                json resultado_letras = future_letras.get();
                json resultado_numeros = future_numeros.get();

                auto fim = std::chrono::high_resolution_clock::now();
                auto duracao = std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio);

                // Montar resposta final
                json resposta;
                resposta["sucesso"] = true;
                resposta["arquivo"] = nome_arquivo;
                resposta["letras"] = resultado_letras.contains("quantidade") ? resultado_letras["quantidade"].get<int>() : 0;
                resposta["numeros"] = resultado_numeros.contains("quantidade") ? resultado_numeros["quantidade"].get<int>() : 0;
                resposta["tempo_ms"] = duracao.count();

                std::cout << "✅ Resultado: " << resposta["letras"] << " letras, " 
                         << resposta["numeros"] << " números (" << duracao.count() << "ms)" << std::endl;

                res.set_content(resposta.dump(), "application/json");
                res.set_header("Access-Control-Allow-Origin", "*");

            } catch (const std::exception& e) {
                json erro;
                erro["erro"] = e.what();
                res.status = 500;
                res.set_content(erro.dump(), "application/json");
                res.set_header("Access-Control-Allow-Origin", "*");
            }
        });

        // CORS
        servidor.Options(".*", [](const httplib::Request&, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
        });

        std::cout << "🚀 Servidor Mestre iniciado na porta 8080" << std::endl;
        std::cout << "🔗 Escravos: Letras(8081), Números(8082)" << std::endl;
        
        servidor.listen("0.0.0.0", 8080);
    }
};

int main() {
    ServidorMestre mestre;
    mestre.iniciar();
    return 0;
}