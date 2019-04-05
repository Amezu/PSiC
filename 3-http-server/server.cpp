#include "HTTPServer.h"

int main() {
  TCPSocket socket;
  socket.makeNonBlocking();
  socket.listenOnAddress(65280, "127.0.0.1");

  HTTPServer server(socket);
  server.serve();
}