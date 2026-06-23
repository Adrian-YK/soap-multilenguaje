# ruby/v3-nativo-espanol/server.rb
# Ejecutar: ruby server.rb
# Acceder:  http://localhost:4567/numero?n=10
#
# Instalar dependencias:
#   gem install sinatra humanize

require 'sinatra'
require 'humanize'

# Configurar humanize en español
Humanize.configure do |config|
  config.default_locale = :es
end

get '/numero' do
  numero = params['n'].to_i
  en_espanol = numero.humanize(locale: :es)
  "#{numero} => #{en_espanol}"
end
