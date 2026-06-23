#!/usr/bin/perl
# perl/v1-soap-ingles/server.pl
# Ejecutar: perl server.pl
# Acceder:  http://localhost:8080/numero?n=10
#
# Instalar dependencias:
#   cpan SOAP::Lite HTTP::Server::Simple::CGI

use strict;
use warnings;
use SOAP::Lite;
use HTTP::Server::Simple::CGI;

my $wsdl = 'https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL';

{
    package MiServidor;
    use parent 'HTTP::Server::Simple::CGI';

    sub handle_request {
        my ($self, $cgi) = @_;
        my $numero = $cgi->param('n') || 0;

        my $soap = SOAP::Lite->service($wsdl);
        my $resultado = $soap->NumberToWords($numero);

        print "HTTP/1.0 200 OK\r\n";
        print "Content-Type: text/plain; charset=utf-8\r\n\r\n";
        print "$numero => $resultado\n";
    }
}

my $servidor = MiServidor->new(8080);
$servidor->run();
