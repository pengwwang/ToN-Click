#! /usr/bin/perl -w

# make-ip-conf.pl -- make a Click IP router configuration
# Robert Morris, Eddie Kohler, David Scott Page
#
# Copyright (c) 1999-2000 Massachusetts Institute of Technology
# Copyright (c) 2002 International Computer Science Institute
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, subject to the conditions
# listed in the Click LICENSE file. These conditions include: you must
# preserve this copyright notice, and you cannot mention the copyright
# holders in advertising related to the Software without their permission.
# The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
# notice is a summary of the Click LICENSE file; the license in that file is
# legally binding.

# Make a Click IP router configuration.  This script generates a
# configuration using PollDevices. You can change it to use
# FromDevices; see the comment above the $ifs array, below.  The
# output is intended for the Linux kernel module; however, by making
# the change from PollDevices to FromDevices, and setting $local_host
# appropriately, the configuration will also work at userlevel.

# --------------------------- Begin configuration ----------------------------

# Change this array to suit your router.
# One line per network interface, containing:
#  The interface name;
#  Whether the interface can use polling (1 = polling, 0 = no polling);
#  The router's IP address on that interface;
#  The netmask on that interface; and
#  The router's Ethernet address on that interface.
# This setup for blackisle -> plebic -> darkstar.

# Router interface information
my @iip = ("10.10.2.1", "10.10.1.2"); #addresses for interfaces from interface_0 to interface_1

# Routing table entry
my @RtTable = ("10.10.2.1/24 1", "10.10.1.2/24 2", "0.0.0.0/0 0");

my $niip = @iip;

# Get interface information from "/var/emulab/boot/ifmap" file on Emulab
my $ifs = [];
my $i;
for($i = 0; $i < $niip; $i++){
    open( FH, "/var/emulab/boot/ifmap" ) || die ("Could not open file"); #open file
    foreach $line ( <FH> ){ #read each line
        @data = split(" ",$line); #split content using space
    
        # convert mac address to the right format
        $macaddress = substr($data[2], 0, 2);
        for($count=0; $count<=4; $count++){
            $macaddress .= ":";
            $macaddress .= substr($data[2], $count*2 + 2, 2);
        }
        
        if($iip[$i] eq $data[1]){
            push @$ifs, [ $data[0], 0, $data[1], "255.255.255.0", $macaddress ];
        }
        close(FH);
    }
}

# Static routes to hosts/networks beyond adjacent networks specified in $ifs.
# One line per route, http://blog.csdn.net/jerry_1126/article/details/24501105containing:
#   The destination address (host or network);
#   The mask;
#   The gateway IP address (next hop);
#   The output network interface name.
# A default route (mask 0.0.0.0) can be specified as the last entry.
#my $srts = [ [ "0.0.0.0", "0.0.0.0", "18.26.4.1", "eth0" ]
#	   ];

# Set to, e.g., "Print(toh) -> Discard" for user-level.
#my $local_host = "ToHost";

# Set to 1 if you want the configuration to handle ICMP echo requests itself.
my $handle_pings = 0;

# --------------------------- End of configuration ---------------------------

my $nifs = $#$ifs + 1;
#my $nsrts = $#$srts + 1;

print "// Generate spine0 router configuration file\n";

for($i = 0; $i < $nifs; $i++){
    printf("// %s %s %s\n",
           $ifs->[$i]->[0],
           $ifs->[$i]->[2],
           $ifs->[$i]->[4]);
}

# Set up the routing table.
my(@routes, @interfaces);

for($i = 0; $i < @RtTable; $i++){
    push @routes, sprintf($RtTable[$i]);
}

for($i = 0; $i < $nifs; $i++){
    push @interfaces, $ifs->[$i]->[2] . '/' . $ifs->[$i]->[3];
}

print "\n// Shared IP input path and routing table\n";
print "ip :: Strip(14)
    -> CheckIPHeader(INTERFACES ", join(' ', @interfaces), ")
    -> rt :: StaticIPLookup(\n\t", join(",\n\t", @routes), ");\n";

# Link-level devices, classification, and ARP
print "\n// ARP responses are copied to each ARPQuerier and the host.\n";
for($i = 0; $i < $nifs; $i++){
    my $devname = $ifs->[$i]->[0];
    my $ip = $ifs->[$i]->[2];
    my $ena = $ifs->[$i]->[4];
    my $paint = $i + 1;
    my $fromdevice = ($ifs->[$i]->[1] ? "PollDevice" : "FromDevice");
    print <<"EOD;";

// Input and output paths for $devname
c$i :: Classifier(12/0806 20/0001, 12/0806 20/0002, 12/0800, -);
$fromdevice($devname) -> c$i;
out$i :: Queue(200) -> todevice$i :: ToDevice($devname);
c$i\[0] -> ar$i :: ARPResponder($ip $ena, 0.0.0.0/0 $ena) -> out$i;
arpq$i :: ARPQuerier($ip, $ena) -> out$i;
c$i\[1] -> [1]arpq$i;
c$i\[2] -> Paint($paint) -> ip;
c$i\[3] -> Print("$devname non-IP") -> Discard;
EOD;
}

# Local delivery path.
#print "\n// Local delivery\n";
#print "toh :: $local_host;\n";
#print "arpt[$nifs] -> toh;\n";
if ($handle_pings) {
    print <<"EOD;";
rt[0] -> IPReassembler -> ping_ipc :: IPClassifier(icmp type echo, -);
ping_ipc[0] -> ICMPPingResponder -> [0]rt;
ping_ipc[1] -> EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2) -> toh;
EOD;
} else {
    print "rt[0] -> EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2) -> Discard;\n";
}

# Forwarding path.
for($i = 0; $i < $nifs; $i++){
    my $i1 = $i + 1;
    my $ipa = $ifs->[$i]->[2];
    my $devname = $ifs->[$i]->[0];
    print <<"EOD;";

// Forwarding path for $devname
rt[$i1] -> DropBroadcasts
    -> cp$i :: PaintTee($i1)
    -> gio$i :: IPGWOptions($ipa)
    -> FixIPSrc($ipa)
    -> dt$i :: DecIPTTL
    -> fr$i :: IPFragmenter(1500)
    -> [0]arpq$i;
dt$i\[1] -> ICMPError($ipa, timeexceeded) -> rt;
fr$i\[1] -> ICMPError($ipa, unreachable, needfrag) -> rt;
gio$i\[1] -> ICMPError($ipa, parameterproblem) -> rt;
cp$i\[1] -> ICMPError($ipa, redirect, host) -> rt;
EOD;
}

sub ip2i {
    my($ip) = @_;
    my @a = split(/\./, $ip);
    my $i = ($a[0] << 24) + ($a[1] << 16) + ($a[2] << 8) + $a[3];
    return($i);
}
sub i2ip {
    my($i) = @_;
    my $a = ($i >> 24) & 0xff;
    my $b = ($i >> 16) & 0xff;
    my $c = ($i >> 8) & 0xff;
    my $d = $i & 0xff;
    return sprintf("%d.%d.%d.%d", $a, $b, $c, $d);
}
