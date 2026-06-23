// cpp/v1-soap-ingles/main.cpp
// Compilar: make  (ver Makefile)
// Ejecutar: ./servidor
// Acceder:  http://localhost:8080/numero?n=10
//
// Dependencias:
//   sudo apt install gsoap libgsoap-dev libcurl4-openssl-dev
//   También requiere generar stubs con wsdl2h y soapcpp2 del WSDL

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <curl/curl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Callback para recibir la respuesta de CURL
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Función que hace la llamada SOAP usando libcurl
std::string llamarSoap(const std::string& numero) {
    CURL* curl = curl_easy_init();
    std::string respuesta;

    if (!curl) return "Error: no se pudo inicializar CURL";

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

    // Extraer resultado del XML
    std::string inicio = "<NumberToWordsResult>";
    std::string fin = "</NumberToWordsResult>";
    size_t posI = respuesta.find(inicio);
    size_t posF = respuesta.find(fin);
    if (posI != std::string::npos && posF != std::string::npos) {
        return respuesta.substr(posI + inicio.size(), posF - posI - inicio.size());
    }
    return "Error al parsear respuesta";
}

// Mini servidor HTTP
void manejarConexion(int cliente_fd) {
    char buffer[4096] = {0};
    read(cliente_fd, buffer, sizeof(buffer) - 1);

    std::string peticion(buffer);
    std::string numero = "0";

    // Extraer parámetro n de la URL (/numero?n=10)
    size_t pos = peticion.find("?n=");
    if (pos != std::string::npos) {
        size_t fin = peticion.find(' ', pos);
        numero = peticion.substr(pos + 3, fin - pos - 3);
    }

    std::string resultado = llamarSoap(numero);
    std::string cuerpo = numero + " => " + resultado;

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
