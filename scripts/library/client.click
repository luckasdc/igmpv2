// Output configuration: 
//
// Packets for the network are put on output 0
// Packets for the host are put on output 1

elementclass Client {
	$address, $gateway |

	igmp::IGMPGroupMember()

	ip :: Strip(14)
		-> CheckIPHeader()
		-> rt :: StaticIPLookup(
					$address:ip/32 0,
					$address:ipnet 0,
					224.0.0.0/4 2,
					0.0.0.0/0.0.0.0 $gateway 1)

		-> [1]output;


	rt[2]
	    -> IPClass::IPClassifier(ip proto IGMP, -)
    	-> [0]igmp

    IPClass[1] // Filter data
        -> CheckUDPHeader()
        -> [1]igmp;

	rt[1]
		-> DropBroadcasts
		-> ipgw :: IPGWOptions($address)
		-> FixIPSrc($address)
		-> ttl :: DecIPTTL
		-> frag :: IPFragmenter(1500)
		-> arpq :: ARPQuerier($address)
		-> output;

	igmp[0] // Deliver data
	    -> [1]output;

    igmp[1] // Send Reports
        -> IPEncapDeluxe(2, $address, 224.0.0.22, TTL 1, TOS 192)
        -> CheckIPHeader()
        -> arpq;


	ipgw[1] -> ICMPError($address, parameterproblem) -> output;
	ttl[1]  -> ICMPError($address, timeexceeded) -> output;
	frag[1] -> ICMPError($address, unreachable, needfrag) -> output;
	
	// Incoming Packets
	input
		-> HostEtherFilter($address)
		-> in_cl :: Classifier(12/0806 20/0001, 12/0806 20/0002, 12/0800)
		-> arp_res :: ARPResponder($address)
		-> output;

	in_cl[1] -> [1]arpq;
	in_cl[2] -> ip;
}
