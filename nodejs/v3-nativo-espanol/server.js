const http = require('http');
const url = require('url');

const PORT = 3000;

const unidades = ['', 'uno', 'dos', 'tres', 'cuatro', 'cinco',
  'seis', 'siete', 'ocho', 'nueve', 'diez', 'once', 'doce',
  'trece', 'catorce', 'quince', 'dieciséis', 'diecisiete',
  'dieciocho', 'diecinueve'];
const decenas = ['', '', 'veinte', 'treinta', 'cuarenta',
  'cincuenta', 'sesenta', 'setenta', 'ochenta', 'noventa'];
const centenas = ['', 'ciento', 'doscientos', 'trescientos',
  'cuatrocientos', 'quinientos', 'seiscientos', 'setecientos',
  'ochocientos', 'novecientos'];

function numeroALetras(n) {
  if (n === 0) return 'cero';
  if (n < 20) return unidades[n];
  if (n < 30) return n === 20 ? 'veinte' : 'veinti' + unidades[n - 20];
  if (n < 100) return n % 10 === 0 ? decenas[Math.floor(n/10)] : decenas[Math.floor(n/10)] + ' y ' + unidades[n % 10];
  if (n === 100) return 'cien';
  if (n < 1000) return n % 100 === 0 ? centenas[Math.floor(n/100)] : centenas[Math.floor(n/100)] + ' ' + numeroALetras(n % 100);
  if (n < 2000) return n % 1000 === 0 ? 'mil' : 'mil ' + numeroALetras(n % 1000);
  if (n < 1000000) {
    const miles = Math.floor(n / 1000);
    const resto = n % 1000;
    return resto === 0 ? numeroALetras(miles) + ' mil' : numeroALetras(miles) + ' mil ' + numeroALetras(resto);
  }
  return 'número muy grande';
}

const server = http.createServer((req, res) => {
  const parsedUrl = url.parse(req.url, true);
  if (parsedUrl.pathname === '/numero') {
    const numero = parseInt(parsedUrl.query.n) || 0;
    res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
    res.end(`${numero} => ${numeroALetras(numero)}`);
  } else {
    res.writeHead(404);
    res.end('Usa: /numero?n=10');
  }
});

server.listen(PORT, () => {
  console.log(`Servidor corriendo en http://localhost:${PORT}/numero?n=10`);
});