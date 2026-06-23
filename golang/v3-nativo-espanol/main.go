// golang/v3-nativo-espanol/main.go
// Ejecutar: go run main.go
// Acceder:  http://localhost:8080/numero?n=10
//
// Instalar dependencias:
//   go mod init nativo-espanol
//   go get github.com/nicholasgasior/gsfmt

package main

import (
	"fmt"
	"log"
	"net/http"
	"strconv"
)

// Conversión nativa en español implementada desde el código base del lenguaje
var unidades = []string{
	"", "uno", "dos", "tres", "cuatro", "cinco",
	"seis", "siete", "ocho", "nueve", "diez",
	"once", "doce", "trece", "catorce", "quince",
	"dieciséis", "diecisiete", "dieciocho", "diecinueve",
}

var decenas = []string{
	"", "", "veinte", "treinta", "cuarenta", "cincuenta",
	"sesenta", "setenta", "ochenta", "noventa",
}

var centenas = []string{
	"", "ciento", "doscientos", "trescientos", "cuatrocientos", "quinientos",
	"seiscientos", "setecientos", "ochocientos", "novecientos",
}

func numeroALetras(n int64) string {
	if n == 0 {
		return "cero"
	}
	if n < 0 {
		return "menos " + numeroALetras(-n)
	}
	if n < 20 {
		return unidades[n]
	}
	if n < 30 {
		if n == 20 {
			return "veinte"
		}
		return "veinti" + unidades[n-20]
	}
	if n < 100 {
		if n%10 == 0 {
			return decenas[n/10]
		}
		return decenas[n/10] + " y " + unidades[n%10]
	}
	if n == 100 {
		return "cien"
	}
	if n < 1000 {
		if n%100 == 0 {
			return centenas[n/100]
		}
		return centenas[n/100] + " " + numeroALetras(n%100)
	}
	if n < 2000 {
		resto := n % 1000
		if resto == 0 {
			return "mil"
		}
		return "mil " + numeroALetras(resto)
	}
	if n < 1_000_000 {
		miles := n / 1000
		resto := n % 1000
		milTexto := numeroALetras(miles) + " mil"
		if resto == 0 {
			return milTexto
		}
		return milTexto + " " + numeroALetras(resto)
	}
	if n < 2_000_000 {
		resto := n % 1_000_000
		if resto == 0 {
			return "un millón"
		}
		return "un millón " + numeroALetras(resto)
	}
	millones := n / 1_000_000
	resto := n % 1_000_000
	millonesTexto := numeroALetras(millones) + " millones"
	if resto == 0 {
		return millonesTexto
	}
	return millonesTexto + " " + numeroALetras(resto)
}

func numeroHandler(w http.ResponseWriter, r *http.Request) {
	nStr := r.URL.Query().Get("n")
	n, err := strconv.ParseInt(nStr, 10, 64)
	if err != nil {
		n = 0
	}
	resultado := numeroALetras(n)
	w.Header().Set("Content-Type", "text/plain; charset=utf-8")
	fmt.Fprintf(w, "%d => %s", n, resultado)
}

func main() {
	http.HandleFunc("/numero", numeroHandler)
	log.Println("Servidor corriendo en http://localhost:8080/numero?n=10")
	log.Fatal(http.ListenAndServe(":8080", nil))
}
