/************************
 * Title: Socket server
 * Author: Milan Hrabovský
 * Login: xhrabo15
 * Subject: IPK
 * Sources:
 * server: https://www.geeksforgeeks.org/socket-programming-cc/
 * cpu load: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
************************/

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "fstream"
#include <sstream>
using namespace std;

string getHostname(){
    ifstream f;
    string hostname;
    f.open("/proc/sys/kernel/hostname", ios::in);
    getline(f, hostname); //get hostname from file, from first line
    f.close();
    return hostname;
}

string getCPUName(){
    ifstream f;
    string cpuName;
    //finds cpu name in file
    f.open("/proc/cpuinfo", ios::in);
    while(!getline(f, cpuName).bad()){
        if(!cpuName.compare(0, 10, "model name")) //finds correct line
            break;
    }
    auto pos = cpuName.find(": ");
    cpuName = cpuName.substr(pos + 2); //remove unecessery strin and keep just model name
    f.close();
    return cpuName;
}

void calcLoad(unsigned long long int *total, unsigned long long int *idle){
    ifstream f;
    string line;
    *idle = 0;
    *total = 0;
    //loads cpu stats from file
    f.open("/proc/stat", ios::in);
    getline(f, line);
    //splite line by values into array cpuStat
    istringstream iss(line);
    unsigned long long int cpuStat[8];
    for (int n=0; n<9; n++){
        string val;
        iss >> val;
        if(n == 0) //to get rid of string on 0 because we don't need it
            continue;
        cpuStat[n] = atoi(val.c_str());
    }
    *idle = cpuStat[3] + cpuStat[4];
    *total = cpuStat[0] + cpuStat[1] + cpuStat[2] + cpuStat[5] + cpuStat[6] + cpuStat[7] + (*idle);
    f.close();
}

string getCPULoad(){
    string cpuLoad;
    unsigned long long int total, prevTotal, idle, prevIdle;
    calcLoad(&prevTotal, &prevIdle);
    sleep(1);
    calcLoad(&total, &idle);
    //calc cpu pecentage CPU_Percentage = (totald - idled)/totald
    unsigned long long int cpuLoadVal = 100 * ((total - prevTotal) - (idle - prevIdle))/(total - prevTotal);
    cpuLoad = to_string(cpuLoadVal);
    return "cpu load " + cpuLoad + "%";
}

int main(int argc, char *argv[]){
    //check if the command have enough arguments
    if(argc != 2) {
        cerr << "Invalid number of parameters" << endl;
        return 1;
    }

    char *p;
    long port = strtol(argv[1], &p, 10);
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // accept only unsigned int as port number
    if(port <= 0) {
        cerr << "Invalid Port number" << endl;
        return 1;
    }

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port from stdin
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(true){
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        string msg = "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n";
        read(new_socket, buffer, 1024);
        //calling functions for user requests
        if(!strncmp(buffer,"GET /load", 9))
            msg += getCPULoad() + "\n";
        else if(!strncmp(buffer,"GET /hostname", 13))
            msg += getHostname() + "\n";
        else if(!strncmp(buffer,"GET /cpu-name", 13))
            msg += getCPUName() + "\n";
        else
            msg += "400 Bad Request\n";
        write(new_socket, msg.c_str(), msg.length()); //message from server
        close(new_socket);
    }
    return 0;
}