// dotnet/v2-soap-traduccion/Program.cs
// Ejecutar: dotnet run
// Acceder:  http://localhost:5000/numero?n=10
//
// Crear proyecto:
//   dotnet new web -n v2-soap-traduccion
//   dotnet add package System.ServiceModel.Http
//   dotnet add package GoogleTranslateFreeApi

using System.ServiceModel;
using GoogleTranslateFreeApi;

var builder = WebApplication.CreateBuilder(args);
builder.WebHost.UseUrls("http://localhost:5000");
builder.Services.AddSingleton<GoogleTranslator>();
var app = builder.Build();

app.MapGet("/numero", async (int n, GoogleTranslator traductor) =>
{
    const string endpoint = "https://www.dataaccess.com/webservicesserver/NumberConversion.wso";

    // 1. Consumir SOAP
    var binding = new BasicHttpsBinding();
    var endpointAddress = new EndpointAddress(endpoint);
    var channel = new ChannelFactory<INumberConversion>(binding, endpointAddress).CreateChannel();
    var enIngles = await channel.NumberToWordsAsync(n);

    // 2. Traducir al español
    var resultado = await traductor.TranslateLiteAsync(enIngles, Language.English, Language.Spanish);
    var enEspanol = resultado.MergedTranslation;

    return $"{n} => {enEspanol}";
});

app.Run();

[ServiceContract(Namespace = "http://www.dataaccess.com/webservicesserver/")]
public interface INumberConversion
{
    [OperationContract(Action = "NumberToWords")]
    Task<string> NumberToWordsAsync(long ubiNum);
}
