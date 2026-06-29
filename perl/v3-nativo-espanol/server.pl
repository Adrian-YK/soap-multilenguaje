#!/usr/bin/perl
use strict;
use warnings;
use IO::Socket::INET;

my @unidades = ('', 'uno', 'dos', 'tres', 'cuatro', 'cinco',
    'seis', 'siete', 'ocho', 'nueve', 'diez', 'once', 'doce',
    'trece', 'catorce', 'quince', "diecis\x{e9}is", 'diecisiete',
    'dieciocho', 'diecinueve');
my @decenas = ('', '', 'veinte', 'treinta', 'cuarenta',
    'cincuenta', 'sesenta', 'setenta', 'ochenta', 'noventa');
my @centenas = ('', 'ciento', 'doscientos', 'trescientos',
    'cuatrocientos', 'quinientos', 'seiscientos', 'setecientos',
    'ochocientos', 'novecientos');

sub numero_a_letras {
    my ($n) = @_;
    return 'cero' if $n == 0;
    return $unidades[$n] if $n < 20;
    if ($n < 30) {
        return 'veinte' if $n == 20;
        return 'veinti' . $unidades[$n-20];
    }
    if ($n < 100) {
        return $decenas[int($n/10)] if $n % 10 == 0;
        return $decenas[int($n/10)] . ' y ' . $unidades[$n%10];
    }
    return 'cien' if $n == 100;
    if ($n < 1000) {
        return $centenas[int($n/100)] if $n % 100 == 0;
        return $centenas[int($n/100)] . ' ' . numero_a_letras($n%100);
    }
    if ($n < 2000) {
        return 'mil' if $n % 1000 == 0;
        return 'mil ' . numero_a_letras($n%1000);
    }
    if ($n < 1000000) {
        my $miles = int($n/1000);
        my $resto = $n % 1000;
        return numero_a_letras($miles) . ' mil' if $resto == 0;
        return numero_a_letras($miles) . ' mil ' . numero_a_letras($resto);
    }
    return 'numero muy grande';
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
    my $resultado = "$n => " . numero_a_letras($n);
    my $response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\nContent-Length: " . length($resultado) . "\r\n\r\n$resultado";
    print $client $response;
    close $client;
}