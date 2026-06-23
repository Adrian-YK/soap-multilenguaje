# SOAP NumberConversion — Clientes Multilenguaje

**Universidad Tecnológica de Puebla**  
Materia: Desarrollo para Dispositivos Inteligentes  
Docente: Mather Xochitl Mendoza Piscil  
Alumno: Carlos Adrián Ortiz Ruiz

---

## Descripción

Repositorio que implementa un cliente del servicio web SOAP público [NumberConversion](https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL) en **8 lenguajes de programación del lado del servidor**, cada uno con **3 versiones**:

| Versión | Descripción |
|---------|-------------|
| `v1-soap-ingles` | Consume el servicio SOAP y devuelve el resultado en inglés |
| `v2-soap-traduccion` | Consume SOAP y traduce el resultado al español mediante una librería |
| `v3-nativo-espanol` | Convierte el número a letras en español directamente (sin SOAP) |

El número se proporciona como parámetro en la URL: `http://localhost:<puerto>/numero?n=10`

---

## Lenguajes incluidos

| Lenguaje | Puerto | Ejecutar |
|----------|--------|---------|
| Ruby | 4567 | `ruby server.rb` |
| Perl | 8080 | `perl server.pl` |
| Node.js | 3000 | `node server.js` |
| .NET 10 | 5000 | `dotnet run` |
| Go | 8080 | `go run main.go` |
| Java | 8080 | `mvn spring-boot:run` |
| C++ | 8080 | `make && ./servidor` |
| Rust | 8080 | `cargo run` |

---

## Estructura del repositorio

```
soap-multilenguaje/
├── ruby/
│   ├── v1-soap-ingles/      # Savon (cliente SOAP)
│   ├── v2-soap-traduccion/  # Savon + google_translate_api_x
│   └── v3-nativo-espanol/   # humanize gem (locale :es)
├── perl/
│   ├── v1-soap-ingles/      # SOAP::Lite
│   ├── v2-soap-traduccion/  # SOAP::Lite + Google Translate API
│   └── v3-nativo-espanol/   # Lingua::ES::Numeros
├── nodejs/
│   ├── v1-soap-ingles/      # soap npm
│   ├── v2-soap-traduccion/  # soap + @vitalets/google-translate-api
│   └── v3-nativo-espanol/   # numero-a-letras npm
├── dotnet/
│   ├── v1-soap-ingles/      # WCF BasicHttpsBinding (.NET 10)
│   ├── v2-soap-traduccion/  # WCF + GoogleTranslateFreeApi
│   └── v3-nativo-espanol/   # Humanizer (es-MX)
├── golang/
│   ├── v1-soap-ingles/      # tiaguinho/gosoap
│   ├── v2-soap-traduccion/  # gosoap + bregydoc/gtranslate
│   └── v3-nativo-espanol/   # Implementación nativa del lenguaje
├── java/
│   ├── v1-soap-ingles/      # Spring Boot + RestTemplate SOAP
│   ├── v2-soap-traduccion/  # Spring Boot + Google Translate API
│   └── v3-nativo-espanol/   # Spring Boot + ICU4J (RuleBasedNumberFormat)
├── cpp/
│   ├── v1-soap-ingles/      # libcurl (HTTP/SOAP manual)
│   ├── v2-soap-traduccion/  # libcurl + Google Translate API
│   └── v3-nativo-espanol/   # Implementación nativa del lenguaje
└── rust/
    ├── v1-soap-ingles/      # axum + reqwest (SOAP manual)
    ├── v2-soap-traduccion/  # axum + reqwest + traducción API
    └── v3-nativo-espanol/   # axum + implementación nativa
```

---

## Flujo de trabajo Git (GitHub Flow)

```
main  ←─────────────────────────────────────────────────
        ↑ PR merge                                       |
feature/ruby-v1        feature/nodejs-v3   feature/rust-v2
```

Cada lenguaje/versión se desarrolla en su propia rama `feature/<lenguaje>-<version>` y se integra a `main` mediante Pull Request.

---

## Referencia (clase base en PHP)

```php
<?php
// v1: Cliente SOAP directo
$cliente = new SoapClient("https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL");
$respuesta = $cliente->NumberToWords(array("ubiNum" => $_GET['n']));
echo $respuesta->NumberToWordsResult;

// v2: SOAP + traducción
require_once 'vendor/autoload.php';
use Stichoza\GoogleTranslate\GoogleTranslate;
$traductor = new GoogleTranslate('es', 'en');
echo $traductor->translate($respuesta->NumberToWordsResult);

// v3: Nativo con intl
$formateador = new NumberFormatter("es", NumberFormatter::SPELLOUT);
echo $formateador->format($_GET['n']);
?>
```
