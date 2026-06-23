#!/usr/bin/perl
# perl/v2-soap-traduccion/server.pl
# Ejecutar: perl server.pl
# Acceder:  http://localhost:8080/numero?n=10
#
# Instalar dependencias:
#   cpan SOAP::Lite HTTP::Server::Simple::CGI LWP::UserAgent JSON URI::Escape

use strict;
use warnings;
use SOAP::Lite;
use HTTP::Server::Simple::CGI;
use LWP::UserAgent;
use JSON;
use URI::Escape;

my $wsdl = 'https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL';

sub traducir_al_espanol {
    my ($texto) = @_;
    my $texto_encoded = uri_escape($texto);
    my $url = "https://translate.googleapis.com/translate_a/single?client=gtx&sl=en&tl=es&dt=t&q=$texto_encoded";

    my $ua = LWP::UserAgent->new;
    my $resp = $ua->get($url);
    if ($resp->is_success) {
        my $data = decode_json($resp->decoded_content);
        return $data->[0][0][0];
    }
    return $texto;
}

{
    package MiServidor;
    use parent 'HTTP::Server::Simple::CGI';

    sub handle_request {
        my ($self, $cgi) = @_;
        my $numero = $cgi->param('n') || 0;

        my $soap = SOAP::Lite->service($wsdl);
        my $en_ingles = $soap->NumberToWords($numero);
        my $en_espanol = main::traducir_al_espanol($en_ingles);

        print "HTTP/1.0 200 OK\r\n";
        print "Content-Type: text/plain; charset=utf-8\r\n\r\n";
        print "$numero => $en_espanol\n";
    }
}

my $servidor = MiServidor->new(8080);
$servidor->run();
