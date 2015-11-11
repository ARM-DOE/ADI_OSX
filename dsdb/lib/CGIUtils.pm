################################################################################
#
#  COPYRIGHT (C) 2010 Battelle Memorial Institute.  All Rights Reserved.
#
################################################################################
#
#  Author:
#     name:  Sherman Beus
#     phone: (509) 375-2662
#     email: sherman.beus@pnl.gov
#
################################################################################
#
#  RCS INFORMATION:
#    $RCSfile: DODDB.perl,v $
#    $Revision: 18176 $
#    $Locker:  $
#    $Date: 2013-07-12 22:27:52 +0000 (Fri, 12 Jul 2013) $
#    $State: Exp $
#
################################################################################

package CGIUtils;

use strict;
use warnings;

use Exporter qw(import);
use vars qw(@EXPORT_OK);
@EXPORT_OK = qw(
    trim
    get_CGI
    get_session
    print_error_for_flash
    json_encode
    json_decode
    print_json
    print_text
    print_html
    session_init
    authenticate
);

use DBI;
use CGI;
use CGI::Session;
use JSON::XS;

use Net::LDAP;

use DBPasswords;

my @Error = ( );

my $_printed_type;

my ($CGI, $Session);

################################################################################

sub _set_error {
    my ($line, @err) = @_;

    push(@Error, ":$line >> " . join("\n", grep { defined($_) } @err ));
}

sub get_error {
    return join("\n\n", @Error);
}

sub trim {
    my ($str) = @_;
    
    $str =~ s/^\s*//;
    $str =~ s/\s*$//;
    
    return $str;
}

sub get_CGI {
    unless (defined($CGI)) {
        $CGI = CGI->new();
    }
    return $CGI;
}

sub get_session {
    unless (defined($Session)) {
        session_init();
    }
    return $Session;
}

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

################################################################################

sub print_error_for_flash {
    my ($msg, @details) = @_;
	
    # Because Flex does not support retrieval of response headers (last time I checked)...
    # print $CGI->header(-status => "400 $msg -> " . join(", ", @details));

	print_text("ERROR:\n$msg\nDETAIL:\n" . join("\n", @details));
}

sub print_json {
    my ($data) = @_;
    
    unless (defined($_printed_type)) {
        $_printed_type = 'text/x-json';
        print $CGI->header(-type => $_printed_type);
    }
    print json_encode($data) if (defined($data));
}

sub print_text {
    my ($text) = @_;
    
    unless (defined($_printed_type)) {
        $_printed_type = 'text/plain';
        print $CGI->header(-type => $_printed_type);
    }
    print $text if defined($text);
}

sub print_html {
    my ($text) = @_;
    
    unless (defined($_printed_type)) {
        $_printed_type = 'text/html';
        print $CGI->header(-type => $_printed_type);
    }
    print $text if (defined($text));
}

sub session_init {
    my $cgi = get_CGI();
    
    my $sid = $cgi->param('CGISESSID');
    if (defined($sid)) {
    	$Session = CGI::Session->new(undef,
    		$cgi->param('CGISESSID'),
    		{ Directory => '/tmp' }
    	);
    	unless ($Session->id() eq $sid) {
    		$Session->delete();
    		$Session = undef;
    	}
	}
}

sub authenticate {
    my ($user, $passwd) = @_;
    
    if (defined($ENV{'DODDB_BYPASS_AUTH'}) && $ENV{'DODDB_BYPASS_AUTH'}) {
        $Session = CGI::Session->new(undef, undef, { Directory => '/tmp' });
        $Session->flush();
        return undef;
    }

    my $ldap_auth = 0;
    my $ldap_info = ldap__get_user_by_id($user);
    
    return(undef) unless (
        defined($ldap_info) &&
        defined($ldap_info->{'userPassword'}) &&
        DBPasswords::verify_pass($passwd, $ldap_info->{'userPassword'})
    );

    $Session = CGI::Session->new(undef, undef, { Directory => '/tmp' });
    $Session->flush();
    
    return $ldap_info;
}

sub ldap__get_user_by_id {
    my ($uid) = @_;
    
    my $ldap = Net::LDAP->new('ldap1.dmf.arm.gov');
    my $res = $ldap->search(
        base   => 'ou=people, dc=arm, dc=gov',
        filter => '(&(uid=' . $uid . '))',
    );
    
    return(undef) unless (defined($res) && defined($res->{entries}->[0]));

    my $atts = $res->{entries}->[0]->asn->{attributes};

    my %info = ( );
    for my $a (@{ $atts }) {
        $info{ $a->{type} } = ($#{ $a->{vals} } > 0) ? $a->{vals} : $a->{vals}->[0];
    }
    
    return \%info;
}

sub wiki__authenticate {
    my ($user, $passwd) = @_;
    
    return(0) unless (
        $user   =~ /^[A-Za-z0-9_]+$/ &&
        $passwd =~ /^[^\s'";]+$/
    );
    
    $passwd = crypt($passwd, $passwd);

    # Connect to Wiki DB.

    my $wdb = DBI->connect(
        'DBI:mysql:host=mydb.dmf.arm.gov;database=http_auth;port=3306',
        'http_authpw', 'op9fg27a'
    );

    return(0) unless (defined($wdb));

    # Request user info based on credentials.

    my $stmt = $wdb->prepare("
        SELECT * FROM user_info
        WHERE user_name = '$user'
          AND user_passwd = '$passwd'
    ");
    $stmt->execute();

    my $ref = $stmt->fetchrow_hashref();
    $stmt->finish();
    return(0) unless (defined($ref));
    
    return 1;
}

1;
