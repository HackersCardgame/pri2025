#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstdio>
#include <iostream>

#include <vector>


#include <termios.h>
#include <unistd.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <resolv.h>
#include <netdb.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <thread>

#include <readline/readline.h>
#include <readline/history.h>

#include "../lib/colorize/color.hpp"

#include "../server/hashPassword.cpp"

#include "../common/regexFunctions.cpp"


#ifndef TCP_CLIENT_NET
#define TCP_CLIENT_NET

#define MAXTCP 65000
#define SA struct sockaddr

using namespace std;

    extern int gSockfd;
    extern SSL * gSsl;
    extern bool gConnected;
    extern bool answered;

//    extern char *command_names[];
    extern vector<string> command_names;


/*******************************************************************************
* does all the network related stuff for the pHase-client (also SSL)
*******************************************************************************/
class TcpClientNet
{
  private:
    inline static int reconnect = 0;
    
    inline static string user = "";
    inline static string password = "";
    
    inline static bool started = false;

  public:
    inline static const bool debug = false;

    inline static string TCPHOST = "localhost";
    inline static int TCPPORT = 8000;

    inline static const int ERROR_STATUS = -1;

    TcpClientNet(string host, string user, string password, int retries)
    {
      if ( TcpClientNet::debug ) cout << "TcpClientNet()" << endl;

      TcpClientNet::TCPHOST=host;
      TcpClientNet::user=user;
      TcpClientNet::password=sha256(password);

      while (true)
      {
        thread t2 = thread(initSocket);    
        if ( TcpClientNet::debug ) cout << "socket started" << endl;

        t2.join();
        gConnected = false;

        if ( TcpClientNet::debug ) cout << "socket died" << endl;

        cout << color::rize("lost connection (" + to_string(reconnect++) + " Seconds ago)", "White", "Red") << " [trying to reconnect every 2 Seconds]      \r" << flush;        
        this_thread::sleep_for(std::chrono::milliseconds(1000));
        if ( retries < reconnect ) exit(1);
      }
    }
    
    ~TcpClientNet()
    {
    
    }

    static int OpenConnection(const char *hostname)
    {
      if ( TcpClientNet::debug ) cout << "OpenConnection()" << endl;
      struct hostent *host;
      if ((host = gethostbyname(hostname)) == nullptr)
      {
        perror(hostname);
        exit(EXIT_FAILURE);
      }

      const char* port = to_string(TcpClientNet::TCPPORT).c_str();

      if ( TcpClientNet::debug ) cout << "hostname: " << hostname << ":" << port << endl;

      struct addrinfo hints = {0}, *addrs;
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = IPPROTO_TCP;

      const int status = getaddrinfo(hostname, port, &hints, &addrs);
      if (status != 0)
      {
        fprintf(stderr, "%s: %s\n", hostname, gai_strerror(status));
        //exit(EXIT_FAILURE);
        return 1;
      }

      if ( TcpClientNet::debug ) cout << "got address info" << endl;


      int err = -1;  //TODO ugly change
      int sockfd = -1;
      for (struct addrinfo *addr = addrs; addr != nullptr; addr = addr->ai_next)
      {
        if ( TcpClientNet::debug ) cout << "in for loop" << endl;

        sockfd = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);

        int opt = 1;

        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

        if ( TcpClientNet::debug ) cout << "sockfd " << sockfd << endl;
        if (sockfd == ERROR_STATUS)
        {
          if ( TcpClientNet::debug ) cout << "errno" << endl;

          err = errno;
          continue;
        }

        if (connect(sockfd, addr->ai_addr, addr->ai_addrlen) == 0)
        {
          if ( TcpClientNet::debug ) cout << "break" << endl;
          break;
        }

        err = errno;
        sockfd = ERROR_STATUS;
        close(sockfd);
      }

      if ( TcpClientNet::debug ) cout << "post for loop" << endl;

      freeaddrinfo(addrs);

      if ( TcpClientNet::debug ) cout << "errorstatus: " << sockfd << endl;
      if ( TcpClientNet::debug ) cout << "errorstatus: " << ERROR_STATUS << endl;


      if (sockfd == ERROR_STATUS)
      {
        if ( !TcpClientNet::started) fprintf(stderr, "%s: %s\n", hostname, strerror(err));
        //cout << "exit failure" << EXIT_FAILURE << endl;
        //exit(EXIT_FAILURE);
        return -1;
      }
      return sockfd;
    }

    static void DisplayCerts(SSL *ssl)
    {
      if ( TcpClientNet::debug ) cout << "DisplayCerts()" << endl;

      X509 *cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
      if (cert != nullptr)
      {
        printf("Server certificates:\n");
        char *line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        //delete line;
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        //delete line;
        X509_free(cert);
      }
      else
      {
        printf("Info: No client certificates configured.\n");
      }
    }


    static SSL_CTX *InitSSL_CTX(void)
    {
      if ( TcpClientNet::debug ) cout << "InitSSL_CTX()" << endl;
      
      SSL_library_init();
      
      const SSL_METHOD *method = SSLv23_client_method(); /* Create new client-method instance */
      SSL_CTX *ctx = SSL_CTX_new(method);

      if (ctx == nullptr)
      {
          ERR_print_errors_fp(stderr);
          exit(EXIT_FAILURE);
      }
      return ctx;
    }


    static string resolveHostname(string host)
    {
      if ( TcpClientNet::debug ) cout << "resolveHostname()" << endl;

      struct hostent *he;
      struct in_addr **addr_list;

      char* hostname = const_cast<char *>(host.c_str());
      
      if ( ( he = gethostbyname( hostname ) ) == NULL )
      {
        cout << "HOST NAME RESOLUTION E-R-R-O-R: " << TCPHOST << endl;
        return "127.0.0.1";
      }
      
      addr_list = (struct in_addr **) he->h_addr_list;
      
      string ip;
      
      for ( int i = 0; addr_list[i] != NULL; i++ )
      {
        ip = inet_ntoa(*addr_list[i]);
        if ( TcpClientNet::debug) cout << ip << endl;  //TODO: CHECK IF IPv6 works too
      }
      
      return ip;
    }

    static void initSocket()
    {
      if ( TcpClientNet::debug ) cout << "initSocket()" << endl;

      int sockfd = -1;

      // socket create and varification
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd == -1)
      {
        if ( TcpClientNet::debug ) cout << "socket creation failed..." << endl;
        //exit(0);
        return;
      }
      else
      {
        //printf("Socket successfully created..\n");
      }
      
      gSockfd = sockfd;
    
      serverListener(sockfd);
    }


    static void serverListener(int sockfd)
    {
      if ( TcpClientNet::debug ) cout << "serverListener()" << endl;

      SSL_CTX *ctx = InitSSL_CTX();
      SSL *ssl = SSL_new(ctx);
      
      char buff[MAXTCP];

      if (ssl == nullptr)
      {
          fprintf(stderr, "SSL_new() failed\n");
          exit(EXIT_FAILURE);
      }
      else
      {
        if ( !TcpClientNet::started) cout << color::rize("SSL initialized", "White", "Yellow") << endl;
      }

      //Host is hardcoded to localhost for testing purposes
      if ( !TcpClientNet::started) cout << color::rize("open connection to "+TcpClientNet::TCPHOST, "White", "Yellow") << endl;

      const int clientSockfd = OpenConnection(resolveHostname(TCPHOST).c_str());
      
      if ( clientSockfd == -1 )
      {
        if ( TcpClientNet::debug ) cout << "exiting serverListener" << endl;
        return;
      }
      cout << color::rize("TCP Connection openend", "White", "Yellow") << endl;

      SSL_set_fd(ssl, clientSockfd);

      const int status = SSL_connect(ssl);
      if (status != 1)
      {
          SSL_get_error(ssl, status);
          ERR_print_errors_fp(stderr); //High probability this doesn't do anything
          fprintf(stderr, "SSL_connect failed with SSL_get_error code %d\n", status);
          //exit(EXIT_FAILURE);
          return;
      }
      else
      {
        cout << color::rize("SSL connected", "White", "Green") << endl;
      }

      cout << color::rize("Connected to ", "White", "Green") << color::rize(TCPHOST, "Black", "Green") << color::rize(" with ", "White", "Green") << color::rize(SSL_get_cipher(ssl), "Red", "Green") << color::rize(" encryption", "White", "Green") << endl;
      cout << "(Starting Serverlistener)" << endl;
      DisplayCerts(ssl);
      gSsl = ssl;
      gConnected = true;
      if ( !(TcpClientNet::user == "") && !(TcpClientNet::password == "") )
      {
        string login = TcpClientNet::user + " " + TcpClientNet::password;
        if ( gConnected )
          SSL_write(gSsl, login.c_str(), login.length());
      }

      TcpClientNet::started = true;
      TcpClientNet::reconnect = 0;
      for (;;)
      {
        bzero(buff, sizeof(buff));
        if ( ! SSL_read(ssl, buff, sizeof(buff)) )
          break;

        string allLines = string(buff);
        vector<string> lines = RegexFunctions::split('\n', allLines);

	      //cout << "\033[999D\n";

        for ( auto l : lines )
        {
			    if ( !RegexFunctions::match("autocomplete:", l) )
          {
            if ( l != "" ) cout << l << endl;
          }
          
			    if ( RegexFunctions::match("autocomplete:", l) )
			    {
			      //cout << "****** autocomplete"<< endl;
			      vector<string> ac = RegexFunctions::split(':', l);
			      vector<string> autocomplete = RegexFunctions::split(',', ac[1]);
			      command_names = {};
			      for ( auto ac : autocomplete )
			        command_names.push_back(ac);  //TODO: dont add if duplicated
			    } 
        }
          
        //cout << "\033[999D\n" << buff << endl;
        answered = true;
        if ((strncmp(buff, "exit", 4)) == 0) {
          printf("Client Exit...\n");
          break;
        }
      }
    }
};




#endif

