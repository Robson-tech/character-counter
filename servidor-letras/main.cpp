// servidor_letras.cpp
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <httplib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ServidorLetras {
private:
    httplib::Server servidor;

public:
    void iniciar() {
        // Health check
        servidor.Get("/health", [](const httplib::Request&, httplib::Response& res) {
            json resposta;
            resposta["status"] = "saudavel";
            resposta["servico"] = "contador-letras";
            resposta["endpoint"] = "/letras";
            res.set_content(resposta.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        });

        // Endpoint para contar letras
        servidor.Post("/letras", [](const httplib::Request& req, httplib::Response& res) {
            try {
                json requisicao = json::parse(req.body);
                std::string texto = requisicao["texto"];
                
                auto inicio = std::chrono::high_resolution_clock::now();
                
                // Contar apenas letras alfabéticas
                int quantidade = std::count_if(texto.begin(), texto.end(), 
                    [](unsigned char c) { return std::isalpha(c); });
                
                auto fim = std::chrono::high_resolution_clock::now();
                auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);
                
                std::cout << "📝 Contei " << quantidade << " letras em " 
                         << texto.length() << " caracteres (" << duracao.count() << "μs)" << std::endl;

                json resposta;
                resposta["quantidade"] = quantidade;
                resposta["servico"] = "letras";
                resposta["tempo_us"] = duracao.count();
                
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

        std::cout << "📝 Servidor de Letras iniciado na porta 8081" << std::endl;
        servidor.listen("0.0.0.0", 8081);
    }
};

int main() {
    ServidorLetras servidor;
    servidor.iniciar();
    return 0;
}