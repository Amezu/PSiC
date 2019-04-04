#include "HTTPServer.h"

int main() {
  HTTPServer server;
  server.makeNonBlocking();
  server.listenOnAddress(65280, "127.0.0.1");
  server.serve();
}