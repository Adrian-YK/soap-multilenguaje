package main

import (
	"fmt"
	"log"
	"net/http"
	"strconv"
)

var unidades = []string{"", "one", "two", "three", "four", "five",
	"six", "seven", "eight", "nine", "ten", "eleven", "twelve",
	"thirteen", "fourteen", "fifteen", "sixteen", "seventeen",
	"eighteen", "nineteen"}

var decenas = []string{"", "", "twenty", "thirty", "forty", "fifty",
	"sixty", "seventy", "eighty", "ninety"}

func numeroEnIngles(n int64) string {
	if n == 0 {
		return "zero"
	}
	if n < 20 {
		return unidades[n]
	}
	if n < 100 {
		if n%10 == 0 {
			return decenas[n/10]
		}
		return decenas[n/10] + " " + unidades[n%10]
	}
	if n < 1000 {
		if n%100 == 0 {
			return unidades[n/100] + " hundred"
		}
		return unidades[n/100] + " hundred " + numeroEnIngles(n%100)
	}
	if n < 1000000 {
		miles := n / 1000
		resto := n % 1000
		if resto == 0 {
			return numeroEnIngles(miles) + " thousand"
		}
		return numeroEnIngles(miles) + " thousand " + numeroEnIngles(resto)
	}
	return "very large number"
}

func numeroHandler(w http.ResponseWriter, r *http.Request) {
	nStr := r.URL.Query().Get("n")
	n, err := strconv.ParseInt(nStr, 10, 64)
	if err != nil {
		n = 0
	}
	resultado := numeroEnIngles(n)
	w.Header().Set("Content-Type", "text/plain; charset=utf-8")
	fmt.Fprintf(w, "%d => %s", n, resultado)
}

func main() {
	http.HandleFunc("/numero", numeroHandler)
	log.Println("Servidor corriendo en http://localhost:8080/numero?n=10")
	log.Fatal(http.ListenAndServe(":8080", nil))
}