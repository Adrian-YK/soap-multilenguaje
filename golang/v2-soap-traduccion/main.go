// golang/v2-soap-traduccion/main.go
// Ejecutar: go run main.go
// Acceder:  http://localhost:8080/numero?n=10
//
// Instalar dependencias:
//   go mod init soap-traduccion
//   go get github.com/tiaguinho/gosoap
//   go get github.com/bregydoc/gtranslate

package main

import (
	"fmt"
	"log"
	"net/http"

	"github.com/bregydoc/gtranslate"
	"github.com/tiaguinho/gosoap"
)

const wsdl = "https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL"

func numeroHandler(w http.ResponseWriter, r *http.Request) {
	numero := r.URL.Query().Get("n")
	if numero == "" {
		numero = "0"
	}

	// 1. Consumir SOAP
	soap, err := gosoap.SoapClient(wsdl, nil)
	if err != nil {
		http.Error(w, fmt.Sprintf("Error SOAP: %v", err), http.StatusInternalServerError)
		return
	}
	params := gosoap.Params{"ubiNum": numero}
	resp, err := soap.Call("NumberToWords", params)
	if err != nil {
		http.Error(w, fmt.Sprintf("Error llamada: %v", err), http.StatusInternalServerError)
		return
	}
	var enIngles string
	resp.Unmarshal(&enIngles)

	// 2. Traducir al español
	enEspanol, err := gtranslate.TranslateWithParams(enIngles, gtranslate.TranslationParams{
		From: "en",
		To:   "es",
	})
	if err != nil {
		enEspanol = enIngles // fallback sin traducción
	}

	w.Header().Set("Content-Type", "text/plain; charset=utf-8")
	fmt.Fprintf(w, "%s => %s", numero, enEspanol)
}

func main() {
	http.HandleFunc("/numero", numeroHandler)
	log.Println("Servidor corriendo en http://localhost:8080/numero?n=10")
	log.Fatal(http.ListenAndServe(":8080", nil))
}
