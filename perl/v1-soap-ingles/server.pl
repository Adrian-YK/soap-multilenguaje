#!/usr/bin/perl
use strict;
use warnings;
use IO::Socket::INET;

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

    my $resultado = "$n => " . numero_en_ingles($n);
    my $response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\nContent-Length: " . length($resultado) . "\r\n\r\n$resultado";
    print $client $response;
    close $client;
}