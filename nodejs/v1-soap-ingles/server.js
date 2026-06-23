// nodejs/v1-soap-ingles/server.js
// Ejecutar: node server.js
// Acceder:  http://localhost:3000/numero?n=10
//
// Instalar dependencias:
//   npm install

const http = require('http');
const url = require('url');
const soap = require('soap');

const WSDL = 'https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL';
const PORT = 3000;

const server = http.createServer(async (req, res) => {
  const parsedUrl = url.parse(req.url, true);

  if (parsedUrl.pathname === '/numero') {
    const numero = parseInt(parsedUrl.query.n) || 0;

    try {
      const cliente = await soap.createClientAsync(WSDL);
      const [resultado] = await cliente.NumberToWordsAsync({ ubiNum: numero });
      const palabras = resultado.NumberToWordsResult;

      res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
      res.end(`${numero} => ${palabras}`);
    } catch (err) {
      res.writeHead(500, { 'Content-Type': 'text/plain' });
      res.end(`Error: ${err.message}`);
    }
  } else {
    res.writeHead(404);
    res.end('Usa: /numero?n=10');
  }
});

server.listen(PORT, () => {
  console.log(`Servidor corriendo en http://localhost:${PORT}/numero?n=10`);
});
