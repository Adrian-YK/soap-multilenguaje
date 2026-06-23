# ruby/v1-soap-ingles/server.rb
# Ejecutar: ruby server.rb
# Acceder:  http://localhost:4567/numero?n=10
#
# Instalar dependencias:
#   gem install sinatra savon

require 'sinatra'
require 'savon'

WSDL = 'https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL'

get '/numero' do
  numero = params['n'].to_i
  cliente = Savon.client(wsdl: WSDL)
  respuesta = cliente.call(:number_to_words, message: { ubi_num: numero })
  resultado = respuesta.body[:number_to_words_response][:number_to_words_result]
  "#{numero} => #{resultado}"
end
