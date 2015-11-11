################################################################################
#
#  COPYRIGHT (C) 2007 Battelle Memorial Institute.  All Rights Reserved.
#
################################################################################
#
#  Author:
#     name:  Sherman Beus
#     phone: (509) 375-2662
#     email: sherman@pnl.gov
#
################################################################################
#
#  REPOSITORY INFORMATION:
#    $Revision: 12937 $
#    $Author: sbeus $
#    $Date: 2012-02-23 17:33:32 +0000 (Thu, 23 Feb 2012) $
#    $State: Exp $
#
################################################################################

package ExternalDB;


use Exporter 'import';
@EXPORT_OK = qw(json_encode json_decode);

use LWP;
use JSON::XS;

use strict;

# Wrapper around JSON::XS JSON encode.
sub json_encode {
    my ($var) = @_;
    
    if (JSON::XS->can('encode_json')) {
        return JSON::XS::encode_json($var);
    }
    return JSON::XS::to_json($var);
}

# Wrapper around JSON::XS JSON decode.
sub json_decode {
    my ($var) = @_;
    
    if (JSON::XS->can('decode_json')) {
        return JSON::XS::decode_json($var);
    }
    return JSON::XS::from_json($var);
}

# Queries a BNL-hosted ARM database given an exposed database function - one
# defined in /shared/include/db.php.
#
# INPUTS:
#
#  db     => Name of database (e.g. int, people, pi).
#  func   => Name of the function to call.
#  params => Array of parameters to send to the function.

sub query_armdb {
    my ($db, $func, @params) = @_;
    
    my $www = new LWP::UserAgent;
    
    my $req = new HTTP::Request( POST => 'http://www.arm.gov/lib/services/db.php' );
    $req->content_type('application/x-www-form-urlencoded');
    $req->content('name=' . $db . '&func=' . $func . '&params=' . json_encode(\@params));

    my $res = $www->request($req);
    return(undef) unless ($res->is_success);
    
    return json_decode($res->content);
}

sub query_armint {
    my ($func, @params) = @_;
    return query_armdb('int', $func, @params);
}

1;
