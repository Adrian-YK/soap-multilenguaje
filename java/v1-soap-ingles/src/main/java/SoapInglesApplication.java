// java/v1-soap-ingles/src/main/java/SoapInglesApplication.java
// Ejecutar: mvn spring-boot:run
// Acceder:  http://localhost:8080/numero?n=10

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.client.RestTemplate;
import org.springframework.http.*;

@SpringBootApplication
@RestController
public class SoapInglesApplication {

    public static void main(String[] args) {
        SpringApplication.run(SoapInglesApplication.class, args);
    }

    @GetMapping("/numero")
    public String numeroALetras(@RequestParam int n) throws Exception {
        String endpoint = "https://www.dataaccess.com/webservicesserver/NumberConversion.wso";

        // Construir mensaje SOAP manualmente
        String soapBody = """
            <?xml version="1.0" encoding="utf-8"?>
            <soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
              <soap:Body>
                <NumberToWords xmlns="http://www.dataaccess.com/webservicesserver/">
                  <ubiNum>%d</ubiNum>
                </NumberToWords>
              </soap:Body>
            </soap:Envelope>
            """.formatted(n);

        HttpHeaders headers = new HttpHeaders();
        headers.setContentType(MediaType.TEXT_XML);
        headers.set("SOAPAction", "NumberToWords");

        RestTemplate restTemplate = new RestTemplate();
        HttpEntity<String> request = new HttpEntity<>(soapBody, headers);
        ResponseEntity<String> response = restTemplate.postForEntity(endpoint, request, String.class);

        // Extraer resultado del XML de respuesta
        String xml = response.getBody();
        String resultado = xml.replaceAll(".*<NumberToWordsResult>(.*?)</NumberToWordsResult>.*", "$1");

        return n + " => " + resultado;
    }
}
