// dotnet/v3-nativo-espanol/Program.cs
// Ejecutar: dotnet run
// Acceder:  http://localhost:5000/numero?n=10
//
// Crear proyecto:
//   dotnet new web -n v3-nativo-espanol
//
// Usa Humanizer con cultura española para convertir números a letras
//   dotnet add package Humanizer.Core.es

using System.Globalization;
using Humanizer;

var builder = WebApplication.CreateBuilder(args);
builder.WebHost.UseUrls("http://localhost:5000");
var app = builder.Build();

app.MapGet("/numero", (int n) =>
{
    // Configurar cultura española
    var culturaEspanol = new CultureInfo("es-MX");
    var enEspanol = ((long)n).ToWords(culturaEspanol);
    return $"{n} => {enEspanol}";
});

app.Run();
