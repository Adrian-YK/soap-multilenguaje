import com.sun.net.httpserver.HttpServer;
import java.net.*;
import java.net.http.*;
import java.net.InetSocketAddress;

public class SoapTraduccionApplication {

    static String[] unidades = {"", "one", "two", "three", "four", "five",
        "six", "seven", "eight", "nine", "ten", "eleven", "twelve",
        "thirteen", "fourteen", "fifteen", "sixteen", "seventeen",
        "eighteen", "nineteen"};
    static String[] decenas = {"", "", "twenty", "thirty", "forty", "fifty",
        "sixty", "seventy", "eighty", "ninety"};

    static String numeroEnIngles(long n) {
        if (n == 0) return "zero";
        if (n < 20) return unidades[(int)n];
        if (n < 100) return n % 10 == 0 ? decenas[(int)(n/10)] : decenas[(int)(n/10)] + " " + unidades[(int)(n%10)];
        if (n < 1000) return n % 100 == 0 ? unidades[(int)(n/100)] + " hundred" : unidades[(int)(n/100)] + " hundred " + numeroEnIngles(n%100);
        if (n < 1000000) {
            long miles = n / 1000, resto = n % 1000;
            return resto == 0 ? numeroEnIngles(miles) + " thousand" : numeroEnIngles(miles) + " thousand " + numeroEnIngles(resto);
        }
        return "very large number";
    }

    static String traducir(String texto) throws Exception {
        String url = "https://translate.googleapis.com/translate_a/single?client=gtx&sl=en&tl=es&dt=t&q=" + URLEncoder.encode(texto, "UTF-8");
        HttpClient client = HttpClient.newHttpClient();
        HttpRequest request = HttpRequest.newBuilder()
            .uri(URI.create(url))
            .header("User-Agent", "Mozilla/5.0")
            .GET()
            .build();
        HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());
        String body = response.body();
        // Parsear JSON simple: [[["traduccion",...
        int i = body.indexOf("[[[\"") + 4;
        int f = body.indexOf("\"", i);
        return body.substring(i, f);
    }

    public static void main(String[] args) throws Exception {
        HttpServer server = HttpServer.create(new InetSocketAddress(8080), 0);
        server.createContext("/numero", exchange -> {
            String query = exchange.getRequestURI().getQuery();
            int n = 0;
            if (query != null && query.startsWith("n=")) {
                try { n = Integer.parseInt(query.substring(2)); } catch (Exception e) {}
            }
            String enIngles = numeroEnIngles(n);
            String enEspanol;
            try { enEspanol = traducir(enIngles); }
            catch (Exception e) { enEspanol = enIngles; }
            String resultado = enIngles + " => " + enEspanol;
            byte[] bytes = resultado.getBytes();
            exchange.sendResponseHeaders(200, bytes.length);
            exchange.getResponseBody().write(bytes);
            exchange.getResponseBody().close();
        });
        server.start();
        System.out.println("Servidor en http://localhost:8080/numero?n=10");
    }
}