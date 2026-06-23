// cpp/v3-nativo-espanol/main.cpp
// Compilar: make
// Ejecutar: ./servidor
// Acceder:  http://localhost:8080/numero?n=10
//
// Sin dependencias externas - implementación nativa en español

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Arrays de palabras en español
const std::string unidades[] = {
    "", "uno", "dos", "tres", "cuatro", "cinco",
    "seis", "siete", "ocho", "nueve", "diez",
    "once", "doce", "trece", "catorce", "quince",
    "dieciséis", "diecisiete", "dieciocho", "diecinueve"
};

const std::string decenas[] = {
    "", "", "veinte", "treinta", "cuarenta", "cincuenta",
    "sesenta", "setenta", "ochenta", "noventa"
};

const std::string centenas[] = {
    "", "ciento", "doscientos", "trescientos", "cuatrocientos", "quinientos",
    "seiscientos", "setecientos", "ochocientos", "novecientos"
};

std::string numeroALetras(long long n) {
    if (n == 0) return "cero";
    if (n < 0) return "menos " + numeroALetras(-n);
    if (n < 20) return unidades[n];
    if (n < 30) {
        if (n == 20) return "veinte";
        return "veinti" + unidades[n - 20];
    }
    if (n < 100) {
        if (n % 10 == 0) return decenas[n / 10];
        return decenas[n / 10] + " y " + unidades[n % 10];
    }
    if (n == 100) return "cien";
    if (n < 1000) {
        if (n % 100 == 0) return centenas[n / 100];
        return centenas[n / 100] + " " + numeroALetras(n % 100);
    }
    if (n < 2000) {
        long long resto = n % 1000;
        if (resto == 0) return "mil";
        return "mil " + numeroALetras(resto);
    }
    if (n < 1000000LL) {
        long long miles = n / 1000;
        long long resto = n % 1000;
        std::string resultado = numeroALetras(miles) + " mil";
        if (resto > 0) resultado += " " + numeroALetras(resto);
        return resultado;
    }
    if (n < 2000000LL) {
        long long resto = n % 1000000;
        if (resto == 0) return "un millón";
        return "un millón " + numeroALetras(resto);
    }
    long long millones = n / 1000000;
    long long resto = n % 1000000;
    std::string resultado = numeroALetras(millones) + " millones";
    if (resto > 0) resultado += " " + numeroALetras(resto);
    return resultado;
}

void manejarConexion(int cliente_fd) {
    char buffer[4096] = {0};
    read(cliente_fd, buffer, sizeof(buffer) - 1);

    std::string peticion(buffer);
    long long numero = 0;

    size_t pos = peticion.find("?n=");
    if (pos != std::string::npos) {
        size_t fin = peticion.find(' ', pos);
        std::string nStr = peticion.substr(pos + 3, fin - pos - 3);
        try { numero = std::stoll(nStr); } catch (...) { numero = 0; }
    }

    std::string resultado = numeroALetras(numero);
    std::string cuerpo = std::to_string(numero) + " => " + resultado;

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
