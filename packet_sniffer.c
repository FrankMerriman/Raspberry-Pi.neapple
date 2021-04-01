#include <stdio.h>
#include <pcap.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


#define MAX_PATH_LENGTH 100

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {

	//add packet to file
	pcap_dump(args, header, packet);
	//print its length
	//printf("--- Jacked a packet with length of [%d]\n", header->len);
}


int main(int argc, char *argv[]) {
	
	pcap_t *handle;			/* Session handle */
	char *dev;			/* The device to sniff on */
	char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */
	//struct bpf_program fp;		/* The compiled filter */
	//char filter_exp[] = "port 23";	/* The filter expression */
	bpf_u_int32 mask;		/* Our netmask */
	bpf_u_int32 net;		/* Our IP */
	//struct pcap_pkthdr header;	/* The header that pcap gives us */
	//const u_char *packet;		/* The actual packet */
	
	// set the device we wish to monitor
	// wlan1 is the interface of my usb dongle
	
	dev = "wlan1";
	printf("Device: %s\n", dev);
	
	
	/* Find the properties for the device */
	if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
		fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
		net = 0;
		mask = 0;
	}
	
	// Open the session in non-promiscuous mode
	handle = pcap_open_live(dev, BUFSIZ, 0, 1000, errbuf);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
		return(2);
	}
	
	int i = 0;
	char path[MAX_PATH_LENGTH] = "packet_dumps/";
	//printf("%s\n", path);
	
	// Get system time
	time_t ltime;
	time(&ltime);
	char *curr_time = ctime(&ltime);
	
	//Remove last two char of array after append ("\n\0" so we have "\0\0")
	//Then path should be fine
	strcat(path, curr_time);
	path[strlen(path)-1] = '\0';
	//printf("%s\n", path);
	
	
	// There is probably a better way to make my path
	strcat(path, "/");
	//printf("%s\n", path);
	
	while (i < 25) {
		//Create the path directory we have made
		if (1 == mkdir(path, 0777)) {
			printf("Error in creation of directory: %s", path);
			break;
		}
		
		// cast? our int to a char so we can concat to path
		char str[21] = "";
		sprintf(str, "%d", i);
		//printf("%s\n", str);
		// concat our previous directory pathing with the file number
		strcat(path, str);
		
		
		printf("Dumping sniffed packets to : %s\n", path);
		
		//open file for dumping
		pcap_dumper_t *dump_file = pcap_dump_open(handle, path);
		//start capture loop and pass dump_file
		pcap_loop(handle, -1, got_packet, (u_char *)dump_file);
		//close dump_file and save stream
		pcap_dump_close(dump_file);
		
		
		i++;
	}
	
	
	// Close session
	pcap_close(handle);
	
	return(0);
}

