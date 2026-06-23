# ruby/v2-soap-traduccion/server.rb
# Ejecutar: ruby server.rb
# Acceder:  http://localhost:4567/numero?n=10
#
# Instalar dependencias:
#   gem install sinatra savon google_translate_api_x

require 'sinatra'
require 'savon'
require 'google_translate_api_x'

WSDL = 'https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL'

get '/numero' do
  numero = params['n'].to_i

  # 1. Consumir servicio SOAP
  cliente = Savon.client(wsdl: WSDL)
  respuesta = cliente.call(:number_to_words, message: { ubi_num: numero })
  en_ingles = respuesta.body[:number_to_words_response][:number_to_words_result]

  # 2. Traducir de inglés a español usando Google Translate
  traductor = GoogleTranslateApiX::Translator.new
  en_espanol = traductor.translate(en_ingles, from: 'en', to: 'es')

  "#{numero} => #{en_espanol}"
end
