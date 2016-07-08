// List of includes
#include <netinet/in.h> 
#include <netdb.h> 
#include <arpa/inet.h> 
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/time.h> 
#include <string.h> 
#include <stdlib.h> 
#include <queue> 
#include <unistd.h> 
#include <fcntl.h> 
#include <iostream> 
#include <string> 
#include  <list> 
#include <errno.h> 
#include <vector> 
#include <fstream> 
#include <time.h> 
#include <ctime> 
#include <cstdio> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/sendfile.h>

using namespace std;

ofstream file;
FILE * file2;
void Log(int number, std::string s);

int main(int argc, char * argv[]) {

	unsigned int PORT = 4003;
	int port2 = 0;
	int sfd, cfd, readNum, writeNum, len, charlen;
	char join[] = "#JOIN";
	char leave[] = "#LEAVE";
	char log[] = "#LOG";
	char list[] = "#LIST";
	char recievedJoin[] = "Recieved a #JOIN action from agent\0";
	char buffer[125];
	int count = 100;
	char hostname[128];
	bool keepGoing = true;
	char Str[32];
	bool clientAddedAlready = false;
	std::string tempString;
	std::vector < std::string > clientListVector;
	std::vector < long int > clientListOfTimes;
	bool notMember = true;
	int tempIndex = 0;
	long int arry[25];
	int tempValue = 0;

	gethostname(hostname, sizeof hostname);

	struct sockaddr_in my_addr, peer_addr;
	socklen_t peer_addr_size;
	port2 = atoi(argv[1]);

	memset( & my_addr, 0, sizeof(struct sockaddr_in));

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port2);
	my_addr.sin_addr.s_addr = INADDR_ANY;

	sfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sfd == -1) return 0;


	if (bind(sfd, (struct sockaddr * ) & my_addr, sizeof(struct sockaddr_in)) == -1) return 0;


	do {

		notMember = true;
		tempIndex = 0;
		if (listen(sfd, 10) == -1) return 0;
		peer_addr_size = sizeof(struct sockaddr_in);
		cfd = accept(sfd, (struct sockaddr * ) & peer_addr, & peer_addr_size);
		if (cfd == -1) return 0;
		memset( & buffer[0], 0, sizeof(buffer));
		readNum = read(cfd, buffer, count);
		if (readNum == -1) write(2, "an read error has occured\n", 27);
		char * ip = inet_ntoa(peer_addr.sin_addr);
		std::string tempAddress(ip);
		
		if (strcmp(join, buffer) == 0) {
			
			struct timeval tv;
			long int curtimeS;
			gettimeofday( & tv, NULL);
			curtimeS = tv.tv_sec;
			Log(1, tempAddress);

			if (clientListVector.size() == 0) {
				
				writeNum = write(cfd, "$OK", 3);
				Log(2, tempAddress);
				clientListVector.push_back(tempAddress);
				clientListOfTimes.push_back(curtimeS);
				clientAddedAlready = true;

			} else {
				for (std::vector <std::string> ::iterator i = clientListVector.begin(); i != clientListVector.end(); ++i) //iterator to traverse through the list
				{
					if (strcmp(i->c_str(), tempAddress.c_str()) == 0) //compare the clients ip addresss to the ones on the list.. if on the list then dont add
					{
						writeNum = write(cfd, "$ALREADY MEMBER", 15);
						Log(7, tempAddress);
						clientAddedAlready = true;
					}


				}
			}


			if (!clientAddedAlready) {
				writeNum = write(cfd, "$OK", 3);
				Log(2, tempAddress);
				clientListVector.push_back(tempAddress);
				tempValue = tempValue + 1;
				clientListOfTimes.push_back(curtimeS);
			} else {

				clientAddedAlready = false;
			}

			if (writeNum == -1) write(2, "an write error has occured\n", 27);
		}
		if (strcmp(leave, buffer) == 0) {
			int deleteIndex = 0;
			tempIndex = 0;
			Log(5, tempAddress);
			for (std::vector <std::string> ::iterator i = clientListVector.begin(); i != clientListVector.end(); ++i) //iterator to traverse through the list
			{

				if (strcmp(i->c_str(), tempAddress.c_str()) == 0) //compare the clients ip addresss to the ones on the list.. if on the list then dont add
				{
					Log(2, tempAddress);
					writeNum = write(cfd, "$OK", 3);
					notMember = false;
					deleteIndex = tempIndex;
				}
				tempIndex++;

			}
			if (notMember) { //compare the clients ip addresss to the ones on the list.. if on the list then dont add
				writeNum = write(cfd, "$NOT MEMBER", 11);
				Log(8, tempAddress);
			} else {
				clientListVector.erase(clientListVector.begin() + deleteIndex);
				clientListOfTimes.erase(clientListOfTimes.begin() + deleteIndex);
			}
			if (writeNum == -1) write(2, "an write error has occured\n", 27);
		}
		if (strcmp(list, buffer) == 0) {
			int tempValueTwo = 0;
			struct timeval tv;
			long int diftimeS;
			long int curtimeS;
			gettimeofday( & tv, NULL);
			curtimeS = tv.tv_sec;
			bool isInList = false;
			Log(3, tempAddress);
			std::string tempStringofAddresses;
			
			for (std::vector <std::string> ::iterator i = clientListVector.begin(); i != clientListVector.end(); ++i) //iterator to traverse through the list
			{
				if (strcmp(i->c_str(), tempAddress.c_str()) == 0) //compare the clients ip addresss to the ones on the list.. if on the list then dont add
				{
					isInList = true;
				}
			}
			for (std::vector <std::string> ::iterator i = clientListVector.begin(); i != clientListVector.end(); ++i) //iterator to traverse through the list
			{
				if (isInList) {
					diftimeS = (curtimeS - clientListOfTimes[tempValueTwo]);
					tempStringofAddresses = tempStringofAddresses + " " + "<" + i->c_str() + "," + std::to_string(diftimeS) + ">";
					tempValueTwo = tempValueTwo + 1;

				}
			}

			if (isInList) {
				writeNum = write(cfd, tempStringofAddresses.c_str(), strlen(tempStringofAddresses.c_str()));
				Log(12, tempStringofAddresses);
				isInList = false;
			} else {
				Log(13, tempAddress);
				isInList = false;

			}



		}
		if (strcmp(log, buffer) == 0) {
			bool isInList = false;
			Log(9, tempAddress);
			for (std::vector <std::string> ::iterator i = clientListVector.begin(); i != clientListVector.end(); ++i) //iterator to traverse through the list
			{
				if (strcmp(i->c_str(), tempAddress.c_str()) == 0) //compare the clients ip addresss to the ones on the list.. if on the list then dont add
				{
					isInList = true;
				}
			}
			if (isInList) {
				int fd = open("log.txt", O_RDONLY);
				if (fd != -1) {
					int sent_bytes = 0;
					off_t offset = 0;
					int remain_data;
					struct stat file_stat;

					if (fstat(fd, & file_stat) < 0) {
						fprintf(stderr, "Error fstat --> %s", strerror(errno));

						exit(EXIT_FAILURE);
					}
					/* Sending file data */
					remain_data = file_stat.st_size;
					while (((sent_bytes = sendfile(cfd, fd, & offset, 1024)) > 0) && (remain_data > 0)) {
						remain_data -= sent_bytes;

					}

				} else write(2, "an read error has occured\n", 26);

				close(fd);

			} else {
				Log(13, tempAddress);
				isInList = false;

			}

		}
		close(cfd);

	} while (1);

	return 0;
}

void Log(int number, std::string s) {
	std::string tempString;
	std::ofstream file("log.txt", std::ofstream::out | std::ofstream::app);
	file2 = fopen("log.txt", "a");
	std::time_t rawtime;
	std::tm * timeinfo;
	std::time( & rawtime);
	std::string timeString(ctime( & rawtime));
	timeString.erase(timeString.size() - 6);
	struct timeval tv;
	long int curtime;
	long int diftime;
	long int diftimetemp;
	long int diftimeS;
	long int curtimeS;
	float micros = 0.000;
	gettimeofday( & tv, NULL);
	curtime = tv.tv_usec;
	curtimeS = tv.tv_sec;
	curtime = curtime / 1000; //needs to be padded with zeros
	timeString = timeString + ":" + std::to_string(curtime) + " 2015";
	switch (number) {

		case 1:
			tempString = timeString + ": Recieved a "
			"#JOIN"
			" action from agent " + s + "\n";
			fprintf(file2, "%s", tempString.c_str());
			break;
		case 2:
			tempString = timeString + ": Responded to agent " + s + " with "
			"$OK"
			"" + "\n";
			fprintf(file2, "%s", tempString.c_str());
			break;
		case 3:
			tempString = timeString + ": Recieved a "
			"#LIST"
			" action from agent " + s + "\n";
			fprintf(file2, "%s", tempString.c_str());
			break;
		case 4:
			tempString = timeString + ": No reponse is supplied to the agent " + s + "\n";
			fprintf(file2, "%s", tempString.c_str());
			break;
		case 5:
			tempString = timeString + ": Recieved a "
			"#LEAVE"
			" action from agent " + s + "\n";
			fprintf(file2, "%s", tempString.c_str());;
			break;
		case 7:
			tempString = timeString + ": Responded to agent " + s + " with "
			"$ALREADY MEMEBER"
			"" + "\n";
			fprintf(file2, "%s", tempString.c_str());
			break;
		case 8:
			tempString = timeString + ": Responded to agent " + s + " with "
			"$NOT MEMEBER"
			"" + "\n";
			fprintf(file2, "%s", tempString.c_str());
			break;
		case 9:
			tempString = timeString + ": Recieved a "
			"#LOG"
			" action from agent " + s + "\n";
			fprintf(file2, "%s", tempString.c_str());
			break;
		case 10:
			tempString = timeString + ": Responded to agent " + s + " with "
			"$NOT AUTHORIZED"
			"" + "\n";
			fprintf(file2, "%s", tempString.c_str());
			break;
		case 11:
			tempString = timeString + ": Responded to agent " + s + " with "
			"$NOT AUTHORIZED"
			"" + "\n";
			fprintf(file2, "%s", tempString.c_str());
			break;
		case 12:
			tempString = timeString + ": Responded to agent with" + s + "\n";
			fprintf(file2, "%s", tempString.c_str());
			break;
		case 13:
			tempString = timeString + ": No response is supplied to agent " + s + " (agent not active) " + "\n";
			fprintf(file2, "%s", tempString.c_str());
			break;

	}
	file.close();
	fclose(file2);
}
