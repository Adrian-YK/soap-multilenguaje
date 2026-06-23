// nodejs/v2-soap-traduccion/server.js
// Ejecutar: node server.js
// Acceder:  http://localhost:3000/numero?n=10
//
// Instalar dependencias:
//   npm install

const http = require('http');
const url = require('url');
const soap = require('soap');
const translate = require('@vitalets/google-translate-api');

const WSDL = 'https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL';
const PORT = 3000;

const server = http.createServer(async (req, res) => {
  const parsedUrl = url.parse(req.url, true);

  if (parsedUrl.pathname === '/numero') {
    const numero = parseInt(parsedUrl.query.n) || 0;

    try {
      // 1. Consumir servicio SOAP (resultado en inglés)
      const cliente = await soap.createClientAsync(WSDL);
      const [resultado] = await cliente.NumberToWordsAsync({ ubiNum: numero });
      const enIngles = resultado.NumberToWordsResult;

      // 2. Traducir al español
      const tradResult = await translate(enIngles, { from: 'en', to: 'es' });
      const enEspanol = tradResult.text;

      res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
      res.end(`${numero} => ${enEspanol}`);
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
