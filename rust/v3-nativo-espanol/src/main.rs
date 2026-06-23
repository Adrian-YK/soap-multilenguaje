// rust/v3-nativo-espanol/src/main.rs
// Ejecutar: cargo run
// Acceder:  http://localhost:8080/numero?n=10
//
// Implementación nativa del código base de Rust sin dependencias externas
// de conversión (solo axum para el servidor web)

use std::collections::HashMap;
use axum::{extract::Query, routing::get, Router};

fn numero_a_letras(n: i64) -> String {
    if n == 0 {
        return "cero".to_string();
    }
    if n < 0 {
        return format!("menos {}", numero_a_letras(-n));
    }

    let unidades = ["", "uno", "dos", "tres", "cuatro", "cinco",
                    "seis", "siete", "ocho", "nueve", "diez",
                    "once", "doce", "trece", "catorce", "quince",
                    "dieciséis", "diecisiete", "dieciocho", "diecinueve"];

    let decenas = ["", "", "veinte", "treinta", "cuarenta", "cincuenta",
                   "sesenta", "setenta", "ochenta", "noventa"];

    let centenas = ["", "ciento", "doscientos", "trescientos", "cuatrocientos",
                    "quinientos", "seiscientos", "setecientos", "ochocientos", "novecientos"];

    if n < 20 {
        return unidades[n as usize].to_string();
    }
    if n < 30 {
        if n == 20 { return "veinte".to_string(); }
        return format!("veinti{}", unidades[(n - 20) as usize]);
    }
    if n < 100 {
        if n % 10 == 0 { return decenas[(n / 10) as usize].to_string(); }
        return format!("{} y {}", decenas[(n / 10) as usize], unidades[(n % 10) as usize]);
    }
    if n == 100 { return "cien".to_string(); }
    if n < 1_000 {
        if n % 100 == 0 { return centenas[(n / 100) as usize].to_string(); }
        return format!("{} {}", centenas[(n / 100) as usize], numero_a_letras(n % 100));
    }
    if n < 2_000 {
        let resto = n % 1_000;
        if resto == 0 { return "mil".to_string(); }
        return format!("mil {}", numero_a_letras(resto));
    }
    if n < 1_000_000 {
        let miles = n / 1_000;
        let resto = n % 1_000;
        let base = format!("{} mil", numero_a_letras(miles));
        if resto == 0 { return base; }
        return format!("{} {}", base, numero_a_letras(resto));
    }
    if n < 2_000_000 {
        let resto = n % 1_000_000;
        if resto == 0 { return "un millón".to_string(); }
        return format!("un millón {}", numero_a_letras(resto));
    }
    let millones = n / 1_000_000;
    let resto = n % 1_000_000;
    let base = format!("{} millones", numero_a_letras(millones));
    if resto == 0 { return base; }
    format!("{} {}", base, numero_a_letras(resto))
}

async fn numero_handler(Query(params): Query<HashMap<String, String>>) -> String {
    let n: i64 = params
        .get("n")
        .and_then(|s| s.parse().ok())
        .unwrap_or(0);

    let resultado = numero_a_letras(n);
    format!("{} => {}", n, resultado)
}

#[tokio::main]
async fn main() {
    let app = Router::new().route("/numero", get(numero_handler));
    let listener = tokio::net::TcpListener::bind("0.0.0.0:8080").await.unwrap();
    println!("Servidor corriendo en http://localhost:8080/numero?n=10");
    axum::serve(listener, app).await.unwrap();
}
