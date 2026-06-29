require 'sinatra'

UNIDADES = ['', 'uno', 'dos', 'tres', 'cuatro', 'cinco',
  'seis', 'siete', 'ocho', 'nueve', 'diez', 'once', 'doce',
  'trece', 'catorce', 'quince', 'dieciséis', 'diecisiete',
  'dieciocho', 'diecinueve']
DECENAS = ['', '', 'veinte', 'treinta', 'cuarenta',
  'cincuenta', 'sesenta', 'setenta', 'ochenta', 'noventa']
CENTENAS = ['', 'ciento', 'doscientos', 'trescientos',
  'cuatrocientos', 'quinientos', 'seiscientos', 'setecientos',
  'ochocientos', 'novecientos']

def numero_a_letras(n)
  return 'cero' if n == 0
  return UNIDADES[n] if n < 20
  return n == 20 ? 'veinte' : 'veinti' + UNIDADES[n - 20] if n < 30
  return n % 10 == 0 ? DECENAS[n / 10] : DECENAS[n / 10] + ' y ' + UNIDADES[n % 10] if n < 100
  return 'cien' if n == 100
  return n % 100 == 0 ? CENTENAS[n / 100] : CENTENAS[n / 100] + ' ' + numero_a_letras(n % 100) if n < 1000
  return n % 1000 == 0 ? 'mil' : 'mil ' + numero_a_letras(n % 1000) if n < 2000
  if n < 1_000_000
    miles = n / 1000
    resto = n % 1000
    return resto == 0 ? numero_a_letras(miles) + ' mil' : numero_a_letras(miles) + ' mil ' + numero_a_letras(resto)
  end
  'número muy grande'
end

get '/numero' do
  numero = params['n'].to_i
  "#{numero} => #{numero_a_letras(numero)}"
end