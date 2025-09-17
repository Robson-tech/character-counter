// servidor_numeros.cpp
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <httplib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ServidorNumeros {
private:
    httplib::Server servidor;

public:
    void iniciar() {
        // Health check
        servidor.Get("/health", [](const httplib::Request&, httplib::Response& res) {
            json resposta;
            resposta["status"] = "saudavel";
            resposta["servico"] = "contador-numeros";
            resposta["endpoint"] = "/numeros";
            res.set_content(resposta.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        });

        // Endpoint para contar números
        servidor.Post("/numeros", [](const httplib::Request& req, httplib::Response& res) {
            try {
                json requisicao = json::parse(req.body);
                std::string texto = requisicao["texto"];
                
                auto inicio = std::chrono::high_resolution_clock::now();
                
                // Contar apenas dígitos numéricos
                int quantidade = std::count_if(texto.begin(), texto.end(), 
                    [](unsigned char c) { return std::isdigit(c); });
                
                auto fim = std::chrono::high_resolution_clock::now();
                auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);
                
                std::cout << "🔢 Contei " << quantidade << " números em " 
                         << texto.length() << " caracteres (" << duracao.count() << "μs)" << std::endl;

                json resposta;
                resposta["quantidade"] = quantidade;
                resposta["servico"] = "numeros";
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

        std::cout << "🔢 Servidor de Números iniciado na porta 8082" << std::endl;
        servidor.listen("0.0.0.0", 8082);
    }
};

int main() {
    ServidorNumeros servidor;
    servidor.iniciar();
    return 0;
}