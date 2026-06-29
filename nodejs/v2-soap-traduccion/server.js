const http = require('http');
const url = require('url');
const soap = require('soap');
const { translate } = require('google-translate-api-x');

const WSDL = 'https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL';
const PORT = 3000;

const server = http.createServer(async (req, res) => {
  const parsedUrl = url.parse(req.url, true);
  if (parsedUrl.pathname === '/numero') {
    const numero = parseInt(parsedUrl.query.n) || 0;
    try {
      const cliente = await soap.createClientAsync(WSDL);
      const [resultado] = await cliente.NumberToWordsAsync({ ubiNum: numero });
      const enIngles = resultado.NumberToWordsResult;

      const tradResult = await translate(enIngles, { from: 'en', to: 'es' });
      const enEspanol = tradResult.text;

      res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
      res.end(`${numero} => ${enEspanol}`);
    } catch (err) {
      res.writeHead(500);
      res.end(`Error: ${err.message}`);
    }
  } else {
    res.writeHead(404);
    res.end('Usa: /numero?n=10');
  }
});

server.listen(PORT, () => {
  console.log(`Servidor en http://localhost:${PORT}/numero?n=10`);
});