var async = require("async");
var isolate = require("isolate");
var sbc = require("sbc-hook");
var logger = require("logger");
var system = require("system");
var zmq = require("zmq");
var SIPMessage = require("sip-parser").SIPMessage;
var resip_ua = require("resip_ua");

var sip_ip = system.NET_IPV4;
var api_ep_ip = system.NET_IPV4;
var api_ep_port = 9061;

sbc.on_initialize_transport(function(){
    return {
        interfaces : [
            {
                primary : true,                 // Only relevant if there are multiple interfaces.  Indicates default interface to be used if not explicitly set by route script
                ip_address : sip_ip,   // The Host IP Address
                external_address : "",          // (Optional) External IP address if the virtual IP is behind a port mapped firewall
                tcp_enabled : true,             // Enable TCP transport
                udp_enabled : true,       
                // Enable UDP Transport
                ws_enabled : false,             // Enable WebSocket Transport
                wss_enabled : false,            // Enable Secure WebSocket Transport
                tls_enabled : false,            // Enable TLS Transport
                sip_port : 5060,                // Port for TCP and UDP
                tls_port : 5061,                // Port for TLS
                ws_port : 5062,                 // Port for WebSocket
                wss_port : 5063,                // Port for Secure WebSocket
                subnets : "0.0.0.0/0"           // (Optional) Comma delimited subnets reachable by this transport
            }
        ],
        tls_ca_file : "/etc/karoo.conf.d/config/ssl/ca.crt", 
        tls_ca_path : "/etc/karoo.conf.d/config/ssl/ca",
        tls_certificate_file : "/etc/karoo.conf.d/config/ssl/karoo.crt",
        tls_private_key_file : "/etc/karoo.conf.d/config/ssl/karoo.key", 
        tls_cert_password : "demo",
        tls_verify_peer : false,
        /****************************************************************************
        * The packet rate ratio allows the transport to detect a potential DoS     *
        * attack.  It works by detecting the packet read rate per second as        *
        * designated by the upper limit.  If the value of packet rate is 50/100,   *
        * the maximum packet rate before the SBC raises the alert level if a       *
        * potential denial of service attack is 100 packets per second.            *
        * When this happens, the transport layer checks if there is a particular   *
        * IP that is sending more than its allowable rate of 50 packets per second.*
        * If the sender is violating the threshold, it will be banned for 1 hour   *
        * which is the third parameter of 3600 seconds.                            *
        ****************************************************************************/
        packet_rate_ratio : "50/100/3600",
        /*****************************************************************************
         * One may statically define a list of known IP addresses or networks so that*
         * they get immunity against the packet-rate-ratio algorithm.  This would    *
         * normally contain the ip addresses of known traffic sources or destinations*
         * like the local iPBX or trunk gateways.  For Call Centers with predictive  *
         * or progressive dialers in the network, it would be wise to also white-list*
         * those applications.                                                       *
         *****************************************************************************/
         packet_rate_white_list : [
            { source_ip : "192.168.1.10" },
            { source_network : "192.168.1.1/24" }
         ],
        /*****************************************************************************
         * If a source network or address is banned, it can execute a script with    *
         * the newly blocked address as the sole argument.  This would allow         *
         * any arbitrary action to be executed like raising an alarm or permanently  *
         * ban the address in iptables.  Absolute path must be used.                 *
         *****************************************************************************/
         execute_on_ban : "",
        /*****************************************************************************
         * If a source network or address is banned, automatically add a null route  *
         * in the routing table.                                                     *
         *****************************************************************************/
         auto_null_route_on_ban : false,
        /****************************************************************************
         * The low value marker for the TCP client port range                        *
         ****************************************************************************/
         sip_tcp_port_base : 10000,
        /****************************************************************************
         * The high value marker for the TCP client port range                      *
         ****************************************************************************/
         sip_tcp_port_max : 15000,
        /****************************************************************************
         * The low value marker for the RTP proxy port range                        *
         ****************************************************************************/
         rtp_proxy_port_base : 30000,
        /****************************************************************************
         * The high value marker for the RTP proxy port range                       *
         ****************************************************************************/
         rtp_proxy_port_max : 40000,
        /****************************************************************************
         * The read timeout for RTP Proxy before it decides to automatically        *
         * tear down the RTP channel.  This is expressed in seconds.                *
         ****************************************************************************/
         rtp_proxy_read_timeout : 300,
        /****************************************************************************
         * The number of worker threads for the RTP proxy.  Take note that Karoo    *
         * Bridge uses async IO and the number here does not limit the number of RTP*
         * channels that can be supported.  This thread pool will be shared equally *
         * by all RTP channels.  10 threads has been tested to be enough to cater   *
         * to 1000 RTP proxy sessions                                               *
         ****************************************************************************/
         rtp_proxy_transport_thread_count : 10,
         homer_enabled : false,      // Enable the SIP Capture service
         homer_version : 3,          // HEP version (2 or 3)
         homer_host : "",            // IP Address of the SIP Capture server
         homer_port : 0,             // Port for the SIP Capture server
         homer_password : "",        // SIP Capture Password
         homer_compression : false,  // Compress packets before sending
         homer_id : 0               // Unique Identifier
    };
});

sbc.on_initialize_user_agent(function(){
    return {
        /****************************************************************************
         * The User-Agent header value                                              *
         ****************************************************************************/
         user_agent_name : "occ_core Session Border Controller",
        /****************************************************************************
         * Karoo bridge uses the contact header to store some states about sip      *
         * session.  It can either do this by storing a unique id as the user info  *
         * of the contact-uri or by storing it as a contact parameter for REGISTER. *
         * Mid-dialog states are also stored using the contact parameters.  If the  *
         * value is set to false, Karoo will insert a record route header instead.  * 
         * If your switch/pbx supports parameters in contact, setting the two       *
         * parameters below to true is the recommended value.                       *
         ****************************************************************************/
         register_state_in_contact_params : true,
         dialog_state_in_contact_params : true,
        /****************************************************************************
         * OPTIONS request are normally used by edge devices to maintain NAT.       *
         * Starting 1.4, Karoo bridge will automatically send an ok response to     *
         * OPTIONS to avoid the over head of these requests reaching the routing    *
         * module.  If you want to have a handle on options routing, you must set   *
         * the parameter to true                                                    *
         ****************************************************************************/
         enable_options_routing : false,
         /***************************************************************************
          * By default, Karoo Bridge will send OPTIONS requests to all registered   *
          * endpoints every 5 seconds.  Set this value to true to disable it        *
          ***************************************************************************/
         disable_options_keep_alive : false,
        /****************************************************************************
         * If set to true, all calls to registered user agents will require an rtp  *
         * proxy.  The current default value is true because there are a lot of user*
         * agents that send the public ip even if it is behind a symmetric NAT      *
         * which will not work without rtp proxy.  Unless you know that all         *
         * user agents will send a private contact when inside a symmetric NAT,     *
         * leave the value of this parameter to true.                               *
         ****************************************************************************/
         require_rtp_for_registrations : true,
        /****************************************************************************
         * Rate Limites per type of request                                         *
         ****************************************************************************/
         max_invites_per_second : 100,
         max_registers_per_second : 100,
         max_subscribes_per_second : 100,
        /****************************************************************************
         * Starting version 2.0.2, channel limits can now be enforced using         *
         * call prefixes.  For example, if you want to limit international calls to *
         * just 30 simultaneous calls, you can add an entry for the IDD prefix      *
         ****************************************************************************/
         channel_limits : [
             {
                 enabled : false,
                 prefix : "011",
                max_channels : 30
             },
             {
                 enabled : false,
                 prefix : "000",
                 max_channels : 30
             }
         ],
         domain_limits : [
             {
                 enabled : true,
                 domain : "demo.webrtc.local",
                 max_channels : 30
             }
         ]
    }
});

sbc.on_route_request(function(context, sipMessage) {
    sbc.set_target_address(context, "udp", "192.168.1.250", "35060");
    //sbc.reject_request(request, "499", "I am a teapot");
});

sbc.on_inbound_request(function(sipMessage) {
    logger.log_info("Incoming request URI=" + sipMessage.getRequestUri());
});

sbc.on_critical_state(function(event){
   log.log_error(event.error_message);
   system.exit(-1);
});

sbc.run();

var api_ep = new zmq.ZMQSocket(zmq.REP);
var api_ep_url = "tcp://" + api_ep_ip + ":" + api_ep_port;
if (!api_ep.bind(api_ep_url)) {
    logger.log_error("Unable to bind API endpoint via " + api_ep_url);
    system.exit(-1);
} else {
    logger.log_info("API endpoint started receiving messages via " + api_ep_url);
}

api_ep.start(function() {
    var msg = new Buffer(1024);
    api_ep.receive(msg);
    logger.log_info(msg.toString());
    var response = new Buffer("Bye ZeroMQ!");
    api_ep.send(response);
});


