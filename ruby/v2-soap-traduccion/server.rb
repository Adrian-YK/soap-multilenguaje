require 'sinatra'
require 'net/http'
require 'uri'
require 'json'

PALABRAS_EN = ['', 'one', 'two', 'three', 'four', 'five',
  'six', 'seven', 'eight', 'nine', 'ten', 'eleven', 'twelve',
  'thirteen', 'fourteen', 'fifteen', 'sixteen', 'seventeen',
  'eighteen', 'nineteen']
DECENAS_EN = ['', '', 'twenty', 'thirty', 'forty', 'fifty',
  'sixty', 'seventy', 'eighty', 'ninety']

def numero_en_ingles(n)
  return 'zero' if n == 0
  return PALABRAS_EN[n] if n < 20
  if n < 100
    return n % 10 == 0 ? DECENAS_EN[n/10] : DECENAS_EN[n/10] + ' ' + PALABRAS_EN[n%10]
  end
  if n < 1000
    return n % 100 == 0 ? PALABRAS_EN[n/100] + ' hundred' : PALABRAS_EN[n/100] + ' hundred ' + numero_en_ingles(n%100)
  end
  miles = n / 1000
  resto = n % 1000
  return resto == 0 ? numero_en_ingles(miles) + ' thousand' : numero_en_ingles(miles) + ' thousand ' + numero_en_ingles(resto)
end

def traducir(texto)
  url = URI("https://translate.googleapis.com/translate_a/single?client=gtx&sl=en&tl=es&dt=t&q=#{URI.encode_www_form_component(texto)}")
  response = Net::HTTP.get(url)
  JSON.parse(response)[0][0][0]
rescue
  texto
end

get '/numero' do
  numero = params['n'].to_i
  en_ingles = numero_en_ingles(numero)
  en_espanol = traducir(en_ingles)
  "#{en_ingles} => #{en_espanol}"
end