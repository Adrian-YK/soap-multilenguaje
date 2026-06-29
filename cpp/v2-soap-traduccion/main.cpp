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

string httpGet(const string& host, const string& path) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    struct hostent* he = gethostbyname(host.c_str());
    if (!he) return "";
    addr.sin_addr = *((struct in_addr*)he->h_addr);
    connect(sock, (sockaddr*)&addr, sizeof(addr));
    string req = "GET " + path + " HTTP/1.0\r\nHost: " + host + "\r\nUser-Agent: Mozilla/5.0\r\n\r\n";
    send(sock, req.c_str(), req.size(), 0);
    string resp = "";
    char buf[4096];
    int n;
    while ((n = recv(sock, buf, sizeof(buf)-1, 0)) > 0) {
        buf[n] = 0;
        resp += buf;
    }
    closesocket(sock);
    return resp;
}

string traducir(const string& texto) {
    string encoded = "";
    for (char c : texto) {
        if (isalnum(c) || c=='-' || c=='_' || c=='.' || c=='~') encoded += c;
        else if (c == ' ') encoded += "%20";
        else { char buf[4]; sprintf(buf, "%%%02X", (unsigned char)c); encoded += buf; }
    }
    string path = "/translate_a/single?client=gtx&sl=en&tl=es&dt=t&q=" + encoded;
    string resp = httpGet("translate.googleapis.com", path);
    size_t pos1 = resp.find("[[[\"");
    if (pos1 == string::npos) return texto;
    pos1 += 4;
    size_t pos2 = resp.find("\"", pos1);
    if (pos2 == string::npos) return texto;
    return resp.substr(pos1, pos2-pos1);
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
    string enIngles = numeroEnIngles(numero);
    string enEspanol = traducir(enIngles);
    string resultado = enIngles + " => " + enEspanol;
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