var builder = WebApplication.CreateBuilder(args);
builder.WebHost.UseUrls("http://localhost:5000");
var app = builder.Build();

string[] unidades = {"", "one", "two", "three", "four", "five",
    "six", "seven", "eight", "nine", "ten", "eleven", "twelve",
    "thirteen", "fourteen", "fifteen", "sixteen", "seventeen",
    "eighteen", "nineteen"};
string[] decenas = {"", "", "twenty", "thirty", "forty", "fifty",
    "sixty", "seventy", "eighty", "ninety"};

string NumeroEnIngles(long n) {
    if (n == 0) return "zero";
    if (n < 20) return unidades[n];
    if (n < 100) return n % 10 == 0 ? decenas[n/10] : decenas[n/10] + " " + unidades[n%10];
    if (n < 1000) return n % 100 == 0 ? unidades[n/100] + " hundred" : unidades[n/100] + " hundred " + NumeroEnIngles(n%100);
    if (n < 1000000) {
        long miles = n/1000, resto = n%1000;
        return resto == 0 ? NumeroEnIngles(miles) + " thousand" : NumeroEnIngles(miles) + " thousand " + NumeroEnIngles(resto);
    }
    return "very large number";
}

async Task<string> Traducir(string texto) {
    var url = $"https://translate.googleapis.com/translate_a/single?client=gtx&sl=en&tl=es&dt=t&q={Uri.EscapeDataString(texto)}";
    using var client = new HttpClient();
    client.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0");
    var response = await client.GetStringAsync(url);
    int i = response.IndexOf("[[[\"") + 4;
    int f = response.IndexOf("\"", i);
    return response.Substring(i, f - i);
}

app.MapGet("/numero", async (int n) => {
    var enIngles = NumeroEnIngles(n);
    var enEspanol = await Traducir(enIngles);
    return $"{enIngles} => {enEspanol}";
});

app.Run();