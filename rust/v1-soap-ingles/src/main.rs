use std::collections::HashMap;
use axum::{extract::Query, routing::get, Router};

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

async fn numero_handler(Query(params): Query<HashMap<String, String>>) -> String {
    let n: i64 = params.get("n").and_then(|s| s.parse().ok()).unwrap_or(0);
    format!("{} => {}", n, numero_en_ingles(n))
}

#[tokio::main]
async fn main() {
    let app = Router::new().route("/numero", get(numero_handler));
    let listener = tokio::net::TcpListener::bind("0.0.0.0:8080").await.unwrap();
    println!("Servidor corriendo en http://localhost:8080/numero?n=10");
    axum::serve(listener, app).await.unwrap();
}