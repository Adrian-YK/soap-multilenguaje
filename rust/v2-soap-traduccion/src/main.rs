use std::collections::HashMap;
use axum::{extract::Query, routing::get, Router};
use reqwest::Client;

fn numero_en_ingles(n: i64) -> String {
    let unidades = ["", "one", "two", "three", "four", "five",
                    "six", "seven", "eight", "nine", "ten", "eleven", "twelve",
                    "thirteen", "fourteen", "fifteen", "sixteen", "seventeen",
                    "eighteen", "nineteen"];
    let decenas = ["", "", "twenty", "thirty", "forty", "fifty",
                   "sixty", "seventy", "eighty", "ninety"];

    if n == 0 { return "zero".to_string(); }
    if n < 20 { return unidades[n as usize].to_string(); }
    if n < 100 {
        if n % 10 == 0 { return decenas[(n/10) as usize].to_string(); }
        return format!("{} {}", decenas[(n/10) as usize], unidades[(n%10) as usize]);
    }
    if n < 1000 {
        if n % 100 == 0 { return format!("{} hundred", unidades[(n/100) as usize]); }
        return format!("{} hundred {}", unidades[(n/100) as usize], numero_en_ingles(n%100));
    }
    if n < 1_000_000 {
        let miles = n / 1000;
        let resto = n % 1000;
        if resto == 0 { return format!("{} thousand", numero_en_ingles(miles)); }
        return format!("{} thousand {}", numero_en_ingles(miles), numero_en_ingles(resto));
    }
    "very large number".to_string()
}

async fn traducir(texto: &str) -> String {
    let url = format!(
        "https://translate.googleapis.com/translate_a/single?client=gtx&sl=en&tl=es&dt=t&q={}",
        urlencoding::encode(texto)
    );
    let client = Client::new();
    if let Ok(resp) = client.get(&url).header("User-Agent", "Mozilla/5.0").send().await {
        if let Ok(body) = resp.text().await {
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
    let n: i64 = params.get("n").and_then(|s| s.parse().ok()).unwrap_or(0);
    let en_ingles = numero_en_ingles(n);
    let en_espanol = traducir(&en_ingles).await;
    format!("{} => {}", en_ingles, en_espanol)
}

#[tokio::main]
async fn main() {
    let app = Router::new().route("/numero", get(numero_handler));
    let listener = tokio::net::TcpListener::bind("0.0.0.0:8080").await.unwrap();
    println!("Servidor corriendo en http://localhost:8080/numero?n=10");
    axum::serve(listener, app).await.unwrap();
}