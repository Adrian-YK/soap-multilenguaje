// java/v3-nativo-espanol/src/main/java/NativoEspanolApplication.java
// Ejecutar: mvn spring-boot:run
// Acceder:  http://localhost:8080/numero?n=10

import com.ibm.icu.text.RuleBasedNumberFormat;
import com.ibm.icu.util.ULocale;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.web.bind.annotation.*;

@SpringBootApplication
@RestController
public class NativoEspanolApplication {

    // Equivalente a NumberFormatter::SPELLOUT de PHP pero con ICU4J
    private static final RuleBasedNumberFormat formateador =
        new RuleBasedNumberFormat(new ULocale("es"), RuleBasedNumberFormat.SPELLOUT);

    public static void main(String[] args) {
        SpringApplication.run(NativoEspanolApplication.class, args);
    }

    @GetMapping("/numero")
    public String numeroALetras(@RequestParam long n) {
        String enEspanol = formateador.format(n);
        return n + " => " + enEspanol;
    }
}
