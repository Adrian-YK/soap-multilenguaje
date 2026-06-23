// rust/v2-soap-traduccion/src/main.rs
// Ejecutar: cargo run
// Acceder:  http://localhost:8080/numero?n=10

use std::collections::HashMap;
use reqwest::Client;
use axum::{extract::Query, routing::get, Router};
use serde::Deserialize;

const SOAP_ENDPOINT: &str = "https://www.dataaccess.com/webservicesserver/NumberConversion.wso";

async fn llamar_soap(n: &str) -> String {
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

    if let Ok(r) = resp {
        let xml = r.text().await.unwrap_or_default();
        if let Some(inicio) = xml.find("<NumberToWordsResult>") {
            let resto = &xml[inicio + "<NumberToWordsResult>".len()..];
            if let Some(fin) = resto.find("</NumberToWordsResult>") {
                return resto[..fin].to_string();
            }
        }
    }
    String::from("error")
}

async fn traducir(texto: &str) -> String {
    let url = format!(
        "https://translate.googleapis.com/translate_a/single?client=gtx&sl=en&tl=es&dt=t&q={}",
        urlencoding::encode(texto)
    );

    let client = Client::new();
    if let Ok(resp) = client.get(&url).send().await {
        if let Ok(body) = resp.text().await {
            // Parsear JSON: [[["traduccion",...
            if let Some(inicio) = body.find("[[[\"") {
                let resto = &body[inicio + 4..];
                if let Some(fin) = resto.find('"') {
                    return resto[..fin].to_string();
                }
            }
        }
    }
    texto.to_string()
}

async fn numero_handler(Query(params): Query<HashMap<String, String>>) -> String {
    let n = params.get("n").map(|s| s.as_str()).unwrap_or("0");

    // 1. SOAP → inglés
    let en_ingles = llamar_soap(n).await;

    // 2. Traducir → español
    let en_espanol = traducir(&en_ingles).await;

    format!("{} => {}", n, en_espanol)
}

#[tokio::main]
async fn main() {
    let app = Router::new().route("/numero", get(numero_handler));
    let listener = tokio::net::TcpListener::bind("0.0.0.0:8080").await.unwrap();
    println!("Servidor corriendo en http://localhost:8080/numero?n=10");
    axum::serve(listener, app).await.unwrap();
}
