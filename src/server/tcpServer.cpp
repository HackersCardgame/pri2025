#include <cstdio>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <unistd.h>

#include "../lib/colorize/color.hpp"

#include "../processRelease/parseRelease.cpp"
#include "../results/release.cpp"

#include "../common/dateTimeFunctions.cpp"

#include "../server/tcpServerListener.cpp"



#define MAXTCP 65000
#define SA struct sockaddr

using namespace std;

#ifndef TCP_SERVER
#define TCP_SERVER

//Not sure what headers are needed or not
//This code (theoretically) writes "Hello World, 123" to a socket over a secure TLS connection
//compiled with g++ -Wall -o client.out client.cpp -L/usr/lib -lssl -lcrypto
//Based off of: https://www.cs.utah.edu/~swalton/listings/articles/ssl_client.c
//Some of the code was taken from this post: https://stackoverflow.com/questions/52727565/client-in-c-use-gethostbyname-or-getaddrinfo

//source: https://gist.github.com/vedantroy/d2b99d774484cf4ea5165b200888e414

/*******************************************************************************
* is what it says: 
* the TCP Server that listens to the TCP Clients (phaseClient)
* this file is for initializing and opening the ports the file
* tcpServerListener.cpp is for listening and work off the things that comes
* from the tcp Cleints
*******************************************************************************/
class TcpServer
{

  private:
    inline static const bool debug = false;
    
    inline static string TCPHOST = "localhost";
    inline static int TCPPORT = 8000;
    
    TcpServer()
    {
      init();
    }
    
    ~TcpServer()
    {
    
    }

  public:

    
    static int create_socket(int port)
    {
      int sockfd;
      struct sockaddr_in addr;

      string ip = resolveHostname(TCPHOST);


      addr.sin_family = AF_INET;
      addr.sin_port = htons(port);
      addr.sin_addr.s_addr = htonl(INADDR_ANY);
      

      sockfd = socket(AF_INET, SOCK_STREAM, 0);

      int opt = 1;

      setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
      setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));


      if (sockfd < 0) 
      {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
      }

      if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
      {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
      }

      if (listen(sockfd, 1) < 0)
      {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
      }
      
        return sockfd;
    }
    
    
    static void DisplayCerts(SSL *ssl)
    {
      if ( TcpServer::debug ) cout << "DisplayCerts()" << endl;

      X509 *cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
      if (cert != nullptr)
      {
        printf("Server certificates:\n");
        char *line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        delete line;
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        delete line;
        X509_free(cert);
      }
      else
      {
        printf("Info: No client certificates configured.\n");
      }
    }



    static void configure_context(SSL_CTX *ctx)
    {
      SSL_CTX_set_ecdh_auto(ctx, 1);

      /* Set the key and cert */
      if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0)
      {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
      }

      if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0 )
      {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
      }
    }

    static SSL_CTX *create_context()
    {
      const SSL_METHOD *method;
      SSL_CTX *ctx;

      method = SSLv23_server_method();

      ctx = SSL_CTX_new(method);
      if (!ctx)
      {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
      }
      return ctx;
    }
  
    static void init_openssl()
    {
      SSL_library_init();
      SSL_load_error_strings();  
      OpenSSL_add_ssl_algorithms();
    }

    static void cleanup_openssl()
    {
      EVP_cleanup();
    }

    static string resolveHostname(string host)
    {
      struct hostent *he;
      struct in_addr **addr_list;

      char* hostname = const_cast<char *>(host.c_str());
      
      if ( ( he = gethostbyname( hostname ) ) == NULL )
      {
        cout << "["+DateTimeFunctions::getTimeStamp() +"] " + "HOST NAME RESOLUTION E-R-R-O-R: " << TCPHOST << endl;
        return "127.0.0.1";
      }
      
      addr_list = (struct in_addr **) he->h_addr_list;
      
      string ip;
      
      for ( int i = 0; addr_list[i] != NULL; i++ )
      {
//        strcpy(ip, inet_ntoa(*addr_list[i]) );
        ip = inet_ntoa(*addr_list[i]);
        if ( TcpServer::debug) cout << ip << endl;  //TODO: CHECK IF IPv6 works too
      }
      
      cout << "["+DateTimeFunctions::getTimeStamp() +"] " + "TCP Server Listening on " << color::rize(ip, "Blue") << ":" << color::rize(to_string(TCPPORT), "Green") << endl;
      return ip;
    }

    // Driver function
    static int init()
    {

      cout << "["+DateTimeFunctions::getTimeStamp() +"] " + color::rize("STARTING TCP SERVER (" + TcpServerListener::getMachineInfo() + ")", "Yellow") << endl;

      int sock;
      SSL_CTX *ctx;

      init_openssl();
      ctx = create_context();

      configure_context(ctx);

      sock = create_socket(TCPPORT);

      thread threads[50000];
      int i = 0;

      while (true)
      {
        struct sockaddr_in addr;
        uint len = sizeof(addr);
        SSL *ssl;

        // Accept the first data packet from client and verification
        int client = accept(sock, (struct sockaddr*)&addr, &len);

        if (client < 0) {
            cout << "["+DateTimeFunctions::getTimeStamp() +"] " + color::rize("server failed to accept client ...", "White", "Red") << endl;
            //exit(EXIT_FAILURE);
        }
        else
          cout << "["+DateTimeFunctions::getTimeStamp() +"] " + color::rize("server acccept the client", "Green") << " " << to_string(client) << endl;

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);
        DisplayCerts(ssl);
        threads[i++] = thread(TcpServerListener::clientListener, ssl, client);
        if ( i > 49997 )
          exit(0);  //RESTART SERVER IF ALL SLOTS USED
      }

      close(sock);
      SSL_CTX_free(ctx);
      cleanup_openssl();
    }
    
};

#endif
