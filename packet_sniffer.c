#include <stdio.h>
#include <pcap.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


#define MAX_PATH_LENGTH 100

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {

	//Add packet to file
	pcap_dump(args, header, packet);
	
	//Print its length
	//printf("--- Jacked a packet with length of [%d]\n", header->len);
}


int main(void) {
	
	pcap_t *handle;			/* Session handle */
	char *dev;			/* The device to sniff on */
	char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */
	//struct bpf_program fp;		/* The compiled filter */
	//char filter_exp[] = "port 23";	/* The filter expression */
	bpf_u_int32 mask;		/* Our netmask */
	bpf_u_int32 net;		/* Our IP */
	//struct pcap_pkthdr header;	/* The header that pcap gives us */
	
	// set the device we wish to monitor
	// wlan1 is the interface of my usb dongle
	dev = "wlan1";
	printf("Device: %s\n", dev);
	
	
	//Find the properties for the device
	if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
		fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
		net = 0;
		mask = 0;
	}
	
	//Open the session in non-promiscuous mode
	handle = pcap_open_live(dev, BUFSIZ, 0, 1000, errbuf);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
		return(2);
	}
	
	//Create path variable
	char path[MAX_PATH_LENGTH] = "packet_dumps/";
	
	//Create the path directory we have made
	if (1 == mkdir(path, 0777)) {
		printf("Error in creation of directory: %s", path);
		return(2);
	}

	//Todo - make this program run until an interupt
	while (1 == 1) {
		
		//Create a local path variable to point towards directory
		char local_path[MAX_PATH_LENGTH];
		strcpy(local_path, path);
		
		//Get system time
		time_t ltime;
		time(&ltime);
		char *curr_time = ctime(&ltime);
		
		//Concat time to local path
		strcat(local_path, curr_time);
		
		//Remove newline that is added by ctime
		local_path[strlen(local_path)-1] = '\0';
		
		printf("Dumping sniffed packets to : %s\n", local_path);
		
		//Open file for dumping
		pcap_dumper_t *dump_file = pcap_dump_open(handle, local_path);
		
		//Start capture loop and pass dump_file
		pcap_loop(handle, 1000, got_packet, (u_char *)dump_file);
		
		//Close dump_file and save stream
		pcap_dump_close(dump_file);
		
	}
	
	
	// Close session
	pcap_close(handle);
	
	
	//While loop that uploads files from directory and then deletes them
	
	
	return(0);
}

