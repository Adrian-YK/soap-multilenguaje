// cpp/v2-soap-traduccion/main.cpp
// Compilar: make
// Ejecutar: ./servidor
// Acceder:  http://localhost:8080/numero?n=10
//
// Dependencias:
//   sudo apt install libcurl4-openssl-dev

#include <iostream>
#include <string>
#include <cstring>
#include <curl/curl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string httpGet(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string respuesta;
    if (!curl) return "";
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return respuesta;
}

std::string llamarSoap(const std::string& numero) {
    CURL* curl = curl_easy_init();
    std::string respuesta;
    if (!curl) return "";

    std::string soapBody =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
        "<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"
        "<soap:Body>"
        "<NumberToWords xmlns=\"http://www.dataaccess.com/webservicesserver/\">"
        "<ubiNum>" + numero + "</ubiNum>"
        "</NumberToWords>"
        "</soap:Body>"
        "</soap:Envelope>";

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: text/xml; charset=utf-8");
    headers = curl_slist_append(headers, "SOAPAction: NumberToWords");

    curl_easy_setopt(curl, CURLOPT_URL, "https://www.dataaccess.com/webservicesserver/NumberConversion.wso");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, soapBody.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

    curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    std::string inicio = "<NumberToWordsResult>";
    std::string fin = "</NumberToWordsResult>";
    size_t posI = respuesta.find(inicio);
    size_t posF = respuesta.find(fin);
    if (posI != std::string::npos && posF != std::string::npos) {
        return respuesta.substr(posI + inicio.size(), posF - posI - inicio.size());
    }
    return "";
}

// URL-encode básico para el texto a traducir
std::string urlEncode(const std::string& texto) {
    CURL* curl = curl_easy_init();
    char* enc = curl_easy_escape(curl, texto.c_str(), texto.size());
    std::string resultado(enc);
    curl_free(enc);
    curl_easy_cleanup(curl);
    return resultado;
}

std::string traducir(const std::string& texto) {
    std::string url = "https://translate.googleapis.com/translate_a/single?client=gtx&sl=en&tl=es&dt=t&q=" + urlEncode(texto);
    std::string resp = httpGet(url);
    // Parsear JSON simple: [[["traduccion","original",...],...],...]
    size_t pos1 = resp.find("[[[\"");
    if (pos1 == std::string::npos) return texto;
    pos1 += 4;
    size_t pos2 = resp.find("\"", pos1);
    if (pos2 == std::string::npos) return texto;
    return resp.substr(pos1, pos2 - pos1);
}

void manejarConexion(int cliente_fd) {
    char buffer[4096] = {0};
    read(cliente_fd, buffer, sizeof(buffer) - 1);

    std::string peticion(buffer);
    std::string numero = "0";

    size_t pos = peticion.find("?n=");
    if (pos != std::string::npos) {
        size_t fin = peticion.find(' ', pos);
        numero = peticion.substr(pos + 3, fin - pos - 3);
    }

    std::string enIngles = llamarSoap(numero);
    std::string enEspanol = traducir(enIngles);
    std::string cuerpo = numero + " => " + enEspanol;

    std::string respuesta =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Content-Length: " + std::to_string(cuerpo.size()) + "\r\n"
        "\r\n" + cuerpo;

    send(cliente_fd, respuesta.c_str(), respuesta.size(), 0);
    close(cliente_fd);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 10);

    std::cout << "Servidor corriendo en http://localhost:8080/numero?n=10" << std::endl;

    while (true) {
        int cliente_fd = accept(server_fd, nullptr, nullptr);
        manejarConexion(cliente_fd);
    }

    return 0;
}
