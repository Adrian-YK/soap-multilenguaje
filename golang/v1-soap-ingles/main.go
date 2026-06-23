// golang/v1-soap-ingles/main.go
// Ejecutar: go run main.go
// Acceder:  http://localhost:8080/numero?n=10
//
// Instalar dependencias:
//   go mod init soap-ingles
//   go get github.com/tiaguinho/gosoap

package main

import (
	"fmt"
	"log"
	"net/http"

	"github.com/tiaguinho/gosoap"
)

const wsdl = "https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL"

func numeroHandler(w http.ResponseWriter, r *http.Request) {
	numero := r.URL.Query().Get("n")
	if numero == "" {
		numero = "0"
	}

	// Crear cliente SOAP
	soap, err := gosoap.SoapClient(wsdl, nil)
	if err != nil {
		http.Error(w, fmt.Sprintf("Error cliente SOAP: %v", err), http.StatusInternalServerError)
		return
	}

	params := gosoap.Params{"ubiNum": numero}
	resp, err := soap.Call("NumberToWords", params)
	if err != nil {
		http.Error(w, fmt.Sprintf("Error llamada SOAP: %v", err), http.StatusInternalServerError)
		return
	}

	var resultado string
	resp.Unmarshal(&resultado)

	w.Header().Set("Content-Type", "text/plain; charset=utf-8")
	fmt.Fprintf(w, "%s => %s", numero, resultado)
}

func main() {
	http.HandleFunc("/numero", numeroHandler)
	log.Println("Servidor corriendo en http://localhost:8080/numero?n=10")
	log.Fatal(http.ListenAndServe(":8080", nil))
}
