#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

string unidades[] = {"", "one", "two", "three", "four", "five",
    "six", "seven", "eight", "nine", "ten", "eleven", "twelve",
    "thirteen", "fourteen", "fifteen", "sixteen", "seventeen",
    "eighteen", "nineteen"};
string decenas[] = {"", "", "twenty", "thirty", "forty", "fifty",
    "sixty", "seventy", "eighty", "ninety"};

string numeroEnIngles(long long n) {
    if (n == 0) return "zero";
    if (n < 20) return unidades[n];
    if (n < 100) return n%10==0 ? decenas[n/10] : decenas[n/10]+" "+unidades[n%10];
    if (n < 1000) return n%100==0 ? unidades[n/100]+" hundred" : unidades[n/100]+" hundred "+numeroEnIngles(n%100);
    if (n < 1000000) {
        long long miles=n/1000, resto=n%1000;
        return resto==0 ? numeroEnIngles(miles)+" thousand" : numeroEnIngles(miles)+" thousand "+numeroEnIngles(resto);
    }
    return "very large number";
}

void manejarConexion(SOCKET cliente) {
    char buffer[4096] = {0};
    recv(cliente, buffer, sizeof(buffer)-1, 0);
    string peticion(buffer);
    long long numero = 0;
    size_t pos = peticion.find("?n=");
    if (pos != string::npos) {
        size_t fin = peticion.find(' ', pos);
        try { numero = stoll(peticion.substr(pos+3, fin-pos-3)); } catch(...) {}
    }
    string resultado = to_string(numero) + " => " + numeroEnIngles(numero);
    string respuesta = "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\nContent-Length: " + to_string(resultado.size()) + "\r\n\r\n" + resultado;
    send(cliente, respuesta.c_str(), respuesta.size(), 0);
    closesocket(cliente);
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);
    bind(server, (sockaddr*)&addr, sizeof(addr));
    listen(server, 10);
    cout << "Servidor corriendo en http://localhost:8080/numero?n=10" << endl;
    while (true) {
        SOCKET cliente = accept(server, nullptr, nullptr);
        manejarConexion(cliente);
    }
    WSACleanup();
    return 0;
}