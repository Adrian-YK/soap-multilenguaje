// nodejs/v3-nativo-espanol/server.js
// Ejecutar: node server.js
// Acceder:  http://localhost:3000/numero?n=10
//
// Instalar dependencias:
//   npm install
//
// Usa la librería 'numero-a-letras' que convierte directamente al español
// sin necesidad del servicio SOAP externo.

const http = require('http');
const url = require('url');
const { numeroALetras } = require('numero-a-letras');

const PORT = 3000;

const server = http.createServer((req, res) => {
  const parsedUrl = url.parse(req.url, true);

  if (parsedUrl.pathname === '/numero') {
    const numero = parseInt(parsedUrl.query.n) || 0;
    const enEspanol = numeroALetras(numero);

    res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
    res.end(`${numero} => ${enEspanol}`);
  } else {
    res.writeHead(404);
    res.end('Usa: /numero?n=10');
  }
});

server.listen(PORT, () => {
  console.log(`Servidor corriendo en http://localhost:${PORT}/numero?n=10`);
});
