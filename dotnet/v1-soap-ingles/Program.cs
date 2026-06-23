// dotnet/v1-soap-ingles/Program.cs
// Ejecutar: dotnet run
// Acceder:  http://localhost:5000/numero?n=10
//
// Crear proyecto:
//   dotnet new web -n v1-soap-ingles
//   dotnet add package System.ServiceModel.Http

using System.ServiceModel;

var builder = WebApplication.CreateBuilder(args);
builder.WebHost.UseUrls("http://localhost:5000");
var app = builder.Build();

app.MapGet("/numero", async (int n) =>
{
    const string endpoint = "https://www.dataaccess.com/webservicesserver/NumberConversion.wso";

    // Configurar cliente WCF (SOAP)
    var binding = new BasicHttpsBinding();
    var endpointAddress = new EndpointAddress(endpoint);
    var channel = new ChannelFactory<INumberConversion>(binding, endpointAddress).CreateChannel();

    var resultado = await channel.NumberToWordsAsync(n);
    return $"{n} => {resultado}";
});

app.Run();

// Contrato del servicio SOAP
[ServiceContract(Namespace = "http://www.dataaccess.com/webservicesserver/")]
public interface INumberConversion
{
    [OperationContract(Action = "NumberToWords")]
    Task<string> NumberToWordsAsync(long ubiNum);
}
