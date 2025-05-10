#include <stdio.h>
#include <pcap.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include <time.h>
#include <string.h>

struct packet_stats {
    int total_packets;
    int tcp_packets;
    int udp_packets;
    int icmp_packets;
    int other_packets;
    int total_bytes;
};

// Print MAC address
void print_mac_address(const u_char *mac) {
    printf("%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

// Print packet payload preview
void print_payload_preview(const u_char *payload, int len) {
    int i;
    printf("Payload preview: ");
    for(i = 0; i < len && i < 32; i++) {
        if(isprint(payload[i]))
            printf("%c", payload[i]);
        else
            printf(".");
    }
    printf("\n");
}

void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    struct ether_header *eth_header;
    struct ip *ip_header;
    struct tcphdr *tcp_header;
    struct udphdr *udp_header;
    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];
    char timestamp[32];
    time_t now;
    struct tm *timeinfo;
    struct packet_stats *stats = (struct packet_stats *)user_data;
    const u_char *payload;
    int payload_len;

    // Update
    stats->total_packets++;
    stats->total_bytes += pkthdr->len;

    // Get timestamp
    now = time(NULL);
    timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Get Ethernet header
    eth_header = (struct ether_header *)packet;

    // Print MAC addresses
    printf("\n[%s] Packet captured\n", timestamp);
    printf("Source MAC: ");
    print_mac_address(eth_header->ether_shost);
    printf("\nDestination MAC: ");
    print_mac_address(eth_header->ether_dhost);
    printf("\n");

    // IP packet check
    if (ntohs(eth_header->ether_type) == ETHERTYPE_IP) {
        ip_header = (struct ip *)(packet + sizeof(struct ether_header));
        
        // Convert IP addresses to strings
        inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);

        printf("Source IP: %s\n", src_ip);
        printf("Destination IP: %s\n", dst_ip);
        printf("Protocol: ");
        
        switch(ip_header->ip_p) {
            case IPPROTO_TCP:
                printf("TCP\n");
                stats->tcp_packets++;
                tcp_header = (struct tcphdr *)((u_char *)ip_header + sizeof(struct ip));
                printf("Source Port: %d\n", ntohs(tcp_header->th_sport));
                printf("Destination Port: %d\n", ntohs(tcp_header->th_dport));
                payload = (u_char *)tcp_header + sizeof(struct tcphdr);
                payload_len = pkthdr->len - (sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr));
                if(payload_len > 0) {
                    print_payload_preview(payload, payload_len);
                }
                break;
            case IPPROTO_UDP:
                printf("UDP\n");
                stats->udp_packets++;
                udp_header = (struct udphdr *)((u_char *)ip_header + sizeof(struct ip));
                printf("Source Port: %d\n", ntohs(udp_header->uh_sport));
                printf("Destination Port: %d\n", ntohs(udp_header->uh_dport));
                payload = (u_char *)udp_header + sizeof(struct udphdr);
                payload_len = pkthdr->len - (sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct udphdr));
                if(payload_len > 0) {
                    print_payload_preview(payload, payload_len);
                }
                break;
            case IPPROTO_ICMP:
                printf("ICMP\n");
                stats->icmp_packets++;
                break;
            default:
                printf("Other\n");
                stats->other_packets++;
        }
        printf("Packet size: %d bytes\n", pkthdr->len);
        printf("----------------------------------------\n");
    }
}

void print_stats(struct packet_stats *stats) {
    printf("\n=== Packet Statistics ===\n");
    printf("Total packets captured: %d\n", stats->total_packets);
    printf("TCP packets: %d\n", stats->tcp_packets);
    printf("UDP packets: %d\n", stats->udp_packets);
    printf("ICMP packets: %d\n", stats->icmp_packets);
    printf("Other packets: %d\n", stats->other_packets);
    printf("Total bytes captured: %d\n", stats->total_bytes);
    printf("=======================\n");
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    char *dev;
    struct bpf_program fp;
    bpf_u_int32 net;
    struct packet_stats stats = {0};
    
    // Find the default network interface
    dev = pcap_lookupdev(errbuf);
    if (dev == NULL) {
        fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
        return 2;
    }
    
    printf("Using device: %s\n", dev);
    
    // Open the device for live capture
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        return 2;
    }
    
    // Set filter to capture all packets
    if (pcap_compile(handle, &fp, "ip", 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter: %s\n", pcap_geterr(handle));
        return 2;
    }
    
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter: %s\n", pcap_geterr(handle));
        return 2;
    }
    
    printf("Starting packet capture... Press Ctrl+C to stop\n");
    
    // Man the harpoons
    pcap_loop(handle, 0, packet_handler, (u_char *)&stats);
    
    // Print final statistics
    print_stats(&stats);
    
    // Clean n close
    pcap_freecode(&fp);
    pcap_close(handle);
    
    return 0;
} 