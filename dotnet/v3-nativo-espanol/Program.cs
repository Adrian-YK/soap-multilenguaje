var builder = WebApplication.CreateBuilder(args);
builder.WebHost.UseUrls("http://localhost:5000");
var app = builder.Build();

string[] unidades = {"", "uno", "dos", "tres", "cuatro", "cinco",
    "seis", "siete", "ocho", "nueve", "diez", "once", "doce",
    "trece", "catorce", "quince", "dieciséis", "diecisiete",
    "dieciocho", "diecinueve"};
string[] decenas = {"", "", "veinte", "treinta", "cuarenta",
    "cincuenta", "sesenta", "setenta", "ochenta", "noventa"};
string[] centenas = {"", "ciento", "doscientos", "trescientos",
    "cuatrocientos", "quinientos", "seiscientos", "setecientos",
    "ochocientos", "novecientos"};

string NumeroALetras(long n) {
    if (n == 0) return "cero";
    if (n < 20) return unidades[n];
    if (n < 30) return n == 20 ? "veinte" : "veinti" + unidades[n-20];
    if (n < 100) return n % 10 == 0 ? decenas[n/10] : decenas[n/10] + " y " + unidades[n%10];
    if (n == 100) return "cien";
    if (n < 1000) return n % 100 == 0 ? centenas[n/100] : centenas[n/100] + " " + NumeroALetras(n%100);
    if (n < 2000) return n % 1000 == 0 ? "mil" : "mil " + NumeroALetras(n%1000);
    if (n < 1000000) {
        long miles = n/1000, resto = n%1000;
        return resto == 0 ? NumeroALetras(miles) + " mil" : NumeroALetras(miles) + " mil " + NumeroALetras(resto);
    }
    return "número muy grande";
}

app.MapGet("/numero", (int n) => $"{n} => {NumeroALetras(n)}");

app.Run();