#!/usr/bin/perl
use strict;
use warnings;
use IO::Socket::INET;
use LWP::UserAgent;
use URI::Escape;
use JSON;

my @unidades = ('', 'one', 'two', 'three', 'four', 'five',
    'six', 'seven', 'eight', 'nine', 'ten', 'eleven', 'twelve',
    'thirteen', 'fourteen', 'fifteen', 'sixteen', 'seventeen',
    'eighteen', 'nineteen');
my @decenas = ('', '', 'twenty', 'thirty', 'forty', 'fifty',
    'sixty', 'seventy', 'eighty', 'ninety');

sub numero_en_ingles {
    my ($n) = @_;
    return 'zero' if $n == 0;
    return $unidades[$n] if $n < 20;
    if ($n < 100) {
        return $decenas[int($n/10)] if $n % 10 == 0;
        return $decenas[int($n/10)] . ' ' . $unidades[$n%10];
    }
    if ($n < 1000) {
        return $unidades[int($n/100)] . ' hundred' if $n % 100 == 0;
        return $unidades[int($n/100)] . ' hundred ' . numero_en_ingles($n%100);
    }
    if ($n < 1000000) {
        my $miles = int($n/1000);
        my $resto = $n % 1000;
        return numero_en_ingles($miles) . ' thousand' if $resto == 0;
        return numero_en_ingles($miles) . ' thousand ' . numero_en_ingles($resto);
    }
    return 'very large number';
}

sub traducir {
    my ($texto) = @_;
    my $encoded = uri_escape($texto);
    my $url = "https://translate.googleapis.com/translate_a/single?client=gtx&sl=en&tl=es&dt=t&q=$encoded";
    my $ua = LWP::UserAgent->new;
    $ua->agent('Mozilla/5.0');
    my $resp = $ua->get($url);
    if ($resp->is_success) {
        my $data = decode_json($resp->decoded_content);
        return $data->[0][0][0];
    }
    return $texto;
}

my $server = IO::Socket::INET->new(
    LocalPort => 8080,
    Type => SOCK_STREAM,
    Reuse => 1,
    Listen => 10
) or die "No se pudo crear el servidor: $!";

print "Servidor corriendo en http://localhost:8080/numero?n=10\n";

while (my $client = $server->accept()) {
    my $request = '';
    while (my $line = <$client>) {
        $request .= $line;
        last if $line eq "\r\n";
    }
    my $n = 0;
    if ($request =~ /GET \/numero\?n=(\d+)/) {
        $n = $1;
    }
    my $en_ingles = numero_en_ingles($n);
    my $en_espanol = traducir($en_ingles);
    my $resultado = "$en_ingles => $en_espanol";
    my $response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\nContent-Length: " . length($resultado) . "\r\n\r\n$resultado";
    print $client $response;
    close $client;
}