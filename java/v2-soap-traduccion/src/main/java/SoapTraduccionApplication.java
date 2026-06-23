// java/v2-soap-traduccion/src/main/java/SoapTraduccionApplication.java
// Ejecutar: mvn spring-boot:run
// Acceder:  http://localhost:8080/numero?n=10

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.http.*;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.client.RestTemplate;
import org.springframework.web.util.UriComponentsBuilder;

@SpringBootApplication
@RestController
public class SoapTraduccionApplication {

    public static void main(String[] args) {
        SpringApplication.run(SoapTraduccionApplication.class, args);
    }

    @GetMapping("/numero")
    public String numeroALetras(@RequestParam int n) throws Exception {
        // 1. Consumir SOAP
        String endpoint = "https://www.dataaccess.com/webservicesserver/NumberConversion.wso";
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

        String xml = response.getBody();
        String enIngles = xml.replaceAll(".*<NumberToWordsResult>(.*?)</NumberToWordsResult>.*", "$1").trim();

        // 2. Traducir usando Google Translate API gratuita
        String urlTrad = UriComponentsBuilder
            .fromHttpUrl("https://translate.googleapis.com/translate_a/single")
            .queryParam("client", "gtx")
            .queryParam("sl", "en")
            .queryParam("tl", "es")
            .queryParam("dt", "t")
            .queryParam("q", enIngles)
            .toUriString();

        ResponseEntity<String> tradResp = restTemplate.getForEntity(urlTrad, String.class);
        ObjectMapper mapper = new ObjectMapper();
        JsonNode json = mapper.readTree(tradResp.getBody());
        String enEspanol = json.get(0).get(0).get(0).asText();

        return n + " => " + enEspanol;
    }
}
