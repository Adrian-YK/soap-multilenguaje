#!/usr/bin/perl
# perl/v3-nativo-espanol/server.pl
# Ejecutar: perl server.pl
# Acceder:  http://localhost:8080/numero?n=10
#
# Instalar dependencias:
#   cpan HTTP::Server::Simple::CGI Lingua::ES::Numeros

use strict;
use warnings;
use HTTP::Server::Simple::CGI;
use Lingua::ES::Numeros;

{
    package MiServidor;
    use parent 'HTTP::Server::Simple::CGI';

    sub handle_request {
        my ($self, $cgi) = @_;
        my $numero = $cgi->param('n') || 0;

        my $conv = new Lingua::ES::Numeros('MASCULINO' => 1);
        my $en_espanol = $conv->cardinal($numero);

        print "HTTP/1.0 200 OK\r\n";
        print "Content-Type: text/plain; charset=utf-8\r\n\r\n";
        print "$numero => $en_espanol\n";
    }
}

my $servidor = MiServidor->new(8080);
$servidor->run();
