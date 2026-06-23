// rust/v1-soap-ingles/src/main.rs
// Ejecutar: cargo run
// Acceder:  http://localhost:8080/numero?n=10

use std::collections::HashMap;
use reqwest::Client;
use axum::{extract::Query, response::Html, routing::get, Router};

const SOAP_ENDPOINT: &str = "https://www.dataaccess.com/webservicesserver/NumberConversion.wso";

async fn numero_a_letras(Query(params): Query<HashMap<String, String>>) -> String {
    let n = params.get("n").map(|s| s.as_str()).unwrap_or("0");

    let soap_body = format!(
        r#"<?xml version="1.0" encoding="utf-8"?>
<soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
  <soap:Body>
    <NumberToWords xmlns="http://www.dataaccess.com/webservicesserver/">
      <ubiNum>{}</ubiNum>
    </NumberToWords>
  </soap:Body>
</soap:Envelope>"#,
        n
    );

    let client = Client::new();
    let resp = client
        .post(SOAP_ENDPOINT)
        .header("Content-Type", "text/xml; charset=utf-8")
        .header("SOAPAction", "NumberToWords")
        .body(soap_body)
        .send()
        .await;

    match resp {
        Ok(r) => {
            let xml = r.text().await.unwrap_or_default();
            // Extraer resultado del XML
            if let Some(inicio) = xml.find("<NumberToWordsResult>") {
                let resto = &xml[inicio + "<NumberToWordsResult>".len()..];
                if let Some(fin) = resto.find("</NumberToWordsResult>") {
                    let resultado = &resto[..fin];
                    return format!("{} => {}", n, resultado);
                }
            }
            format!("{} => Error al parsear respuesta", n)
        }
        Err(e) => format!("Error: {}", e),
    }
}

#[tokio::main]
async fn main() {
    let app = Router::new().route("/numero", get(numero_a_letras));
    let listener = tokio::net::TcpListener::bind("0.0.0.0:8080").await.unwrap();
    println!("Servidor corriendo en http://localhost:8080/numero?n=10");
    axum::serve(listener, app).await.unwrap();
}
