import com.sun.net.httpserver.HttpServer;
import java.net.InetSocketAddress;

public class NativoEspanolApplication {

    static String[] unidades = {"", "uno", "dos", "tres", "cuatro", "cinco",
        "seis", "siete", "ocho", "nueve", "diez", "once", "doce",
        "trece", "catorce", "quince", "dieciséis", "diecisiete",
        "dieciocho", "diecinueve"};
    static String[] decenas = {"", "", "veinte", "treinta", "cuarenta",
        "cincuenta", "sesenta", "setenta", "ochenta", "noventa"};
    static String[] centenas = {"", "ciento", "doscientos", "trescientos",
        "cuatrocientos", "quinientos", "seiscientos", "setecientos",
        "ochocientos", "novecientos"};

    static String numeroALetras(long n) {
        if (n == 0) return "cero";
        if (n < 20) return unidades[(int)n];
        if (n < 30) return n == 20 ? "veinte" : "veinti" + unidades[(int)(n-20)];
        if (n < 100) return n % 10 == 0 ? decenas[(int)(n/10)] : decenas[(int)(n/10)] + " y " + unidades[(int)(n%10)];
        if (n == 100) return "cien";
        if (n < 1000) return n % 100 == 0 ? centenas[(int)(n/100)] : centenas[(int)(n/100)] + " " + numeroALetras(n%100);
        if (n < 2000) return n % 1000 == 0 ? "mil" : "mil " + numeroALetras(n%1000);
        if (n < 1000000) {
            long miles = n / 1000, resto = n % 1000;
            return resto == 0 ? numeroALetras(miles) + " mil" : numeroALetras(miles) + " mil " + numeroALetras(resto);
        }
        return "número muy grande";
    }

    public static void main(String[] args) throws Exception {
        HttpServer server = HttpServer.create(new InetSocketAddress(8080), 0);
        server.createContext("/numero", exchange -> {
            String query = exchange.getRequestURI().getQuery();
            int n = 0;
            if (query != null && query.startsWith("n=")) {
                try { n = Integer.parseInt(query.substring(2)); } catch (Exception e) {}
            }
            String resultado = n + " => " + numeroALetras(n);
            byte[] bytes = resultado.getBytes("UTF-8");
            exchange.getResponseHeaders().set("Content-Type", "text/plain; charset=utf-8");
            exchange.sendResponseHeaders(200, bytes.length);
            exchange.getResponseBody().write(bytes);
            exchange.getResponseBody().close();
        });
        server.start();
        System.out.println("Servidor en http://localhost:8080/numero?n=10");
    }
}