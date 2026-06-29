package main

import (
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
	"net/url"
	"strconv"
)

var unidades = []string{"", "one", "two", "three", "four", "five",
	"six", "seven", "eight", "nine", "ten", "eleven", "twelve",
	"thirteen", "fourteen", "fifteen", "sixteen", "seventeen",
	"eighteen", "nineteen"}

var decenas = []string{"", "", "twenty", "thirty", "forty", "fifty",
	"sixty", "seventy", "eighty", "ninety"}

func numeroEnIngles(n int64) string {
	if n == 0 { return "zero" }
	if n < 20 { return unidades[n] }
	if n < 100 {
		if n%10 == 0 { return decenas[n/10] }
		return decenas[n/10] + " " + unidades[n%10]
	}
	if n < 1000 {
		if n%100 == 0 { return unidades[n/100] + " hundred" }
		return unidades[n/100] + " hundred " + numeroEnIngles(n%100)
	}
	miles := n / 1000; resto := n % 1000
	if resto == 0 { return numeroEnIngles(miles) + " thousand" }
	return numeroEnIngles(miles) + " thousand " + numeroEnIngles(resto)
}

func traducir(texto string) string {
	apiURL := fmt.Sprintf("https://translate.googleapis.com/translate_a/single?client=gtx&sl=en&tl=es&dt=t&q=%s", url.QueryEscape(texto))
	client := &http.Client{}
	req, _ := http.NewRequest("GET", apiURL, nil)
	req.Header.Set("User-Agent", "Mozilla/5.0")
	resp, err := client.Do(req)
	if err != nil { return texto }
	defer resp.Body.Close()
	body, _ := io.ReadAll(resp.Body)
	var result []interface{}
	if err := json.Unmarshal(body, &result); err != nil { return texto }
	arr := result[0].([]interface{})
	traduccion := ""
	for _, item := range arr {
		par := item.([]interface{})
		if len(par) > 0 && par[0] != nil {
			traduccion += par[0].(string)
		}
	}
	return traduccion
}

func numeroHandler(w http.ResponseWriter, r *http.Request) {
	nStr := r.URL.Query().Get("n")
	n, err := strconv.ParseInt(nStr, 10, 64)
	if err != nil { n = 0 }
	enIngles := numeroEnIngles(n)
	enEspanol := traducir(enIngles)
	w.Header().Set("Content-Type", "text/plain; charset=utf-8")
	fmt.Fprintf(w, "%s => %s", enIngles, enEspanol)
}

func main() {
	http.HandleFunc("/numero", numeroHandler)
	log.Println("Servidor corriendo en http://localhost:8080/numero?n=10")
	log.Fatal(http.ListenAndServe(":8080", nil))
}