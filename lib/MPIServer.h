//
// Created by Apoorv Gupta on 4/21/19.
//

#ifndef DOMP_MPISERVER_H
#define DOMP_MPISERVER_H

#include <iostream>
#include <list>
#include <mutex>
#include <utility>
#include <mpi.h>
#include <map>
#include <thread>
#include <condition_variable>
#include "domp.h"

using namespace std;

namespace domp {
#define DOMP_MIN_DATA_TAG (100)

  class MPIServer;
  class MPIMasterServer;
  enum MPIServerTag {MPI_MAP_REQ= 0, MPI_MAP_RESP, MPI_DATA_CMD, MPI_EXIT_CMD, MPI_EXIT_ACK};
  enum MPIAccessType {MPI_SHARED_FETCH= 0, MPI_EXCLUSIVE_FETCH, MPI_SHARED_FIRST, MPI_EXCLUSIVE_FIRST};

#define IS_EXCLUSIVE(e) ((e == MPI_EXCLUSIVE_FETCH) ||(e == MPI_EXCLUSIVE_FIRST))
#define IS_FETCH(e) ((e == MPI_SHARED_FETCH) || (e == MPI_EXCLUSIVE_FETCH))


  typedef struct DOMPMapCommand {
    std::string varName;
    int start;
    int size;
    MPIAccessType accessType;
    int nodeId;
  } DOMPMapCommand_t;

  typedef struct DOMPMapRequest {
    int count;
    std::list <DOMPMapCommand_t> commands;
  } DOMPMapRequest_t;

}



class domp::MPIServer {
 protected:
  int clusterSize;
  int rank;
  char clusterName[DOMP_MAX_CLUSTER_NAME];
  char port_name[MPI_MAX_PORT_NAME];
  std::thread serverThread;
  MPI_Comm *nodeConnections;
  void accept();
  void handleRequest(MPI_Status *status);
  DOMPMapRequest_t *mapRequest;
  std::map <int, DOMPMapCommand*> dataRequests;
  DOMP *dompObject;

  // Locking and waiting mechanism for slave nodes requesting data
  std::mutex dataMtx;
  std::condition_variable dataCV;
  std::condition_variable exitCondv;
  bool dataReceived;


 public:
  MPIServer(DOMP *dompObject, char* clusterName, int clusterSize, int rank) {
    this->dompObject = dompObject;
    strncpy(this->clusterName, clusterName, DOMP_MAX_CLUSTER_NAME);
    this->clusterSize = clusterSize;
    this->rank = rank;
    this->mapRequest = new DOMPMapRequest_t();
    nodeConnections = new MPI_Comm[this->rank];
  }
  ~MPIServer();
  void startServer();
  void stopServer();
  void requestData(std::string varName, int start, int size, MPIAccessType accessType);
  void triggerMap();
  void transferData(MPI_Status *status);
  void receiveData(MPI_Status *status);
  void handleMapResponse(MPI_Status *status);
  void handleMapRequest(MPI_Status *status){}
  bool serverRunning;
};


class domp::MPIMasterServer : public domp::MPIServer {
  int mapReceived;
  std::mutex mappingMtx;
  std::list<std::pair<int,DOMPMapCommand_t*>> commands_received;
  public:
  MPIMasterServer(DOMP *dompObject, char* clusterName, int size, int rank) :MPIServer(dompObject, clusterName, size, rank){
    mapReceived = 0;
  };
  void handleMapRequest(MPI_Status *status);
};
#endif //DOMP_MPISERVER_H