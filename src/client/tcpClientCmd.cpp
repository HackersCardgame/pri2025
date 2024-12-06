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

#include <signal.h>

#include "../lib/colorize/color.hpp"

#include "../common/regexFunctions.cpp"

#ifndef TCP_CLIENT_CMD
#define TCP_CLIENT_CMD

    extern int gSockfd;
    extern SSL * gSsl;
    extern bool gConnected;
    extern bool answered;

using namespace std;

//forward declaration
char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);

//TODO: maybe have this list in a separate config file or load from database?

vector<string> command_names = {};



/*******************************************************************************
" Command interpreter with history and [TAB]-completion, uses GNU Readline
*******************************************************************************/
class TcpClientCmd
{
  private:
    inline static const bool debug2 = false;

  public:
    inline static const bool debug = false;
    inline static string script = "";
//    inline static const char* historyFile = ".pri-history";
    inline static const char* historyFile = NULL;
    
    TcpClientCmd(string script)
    {
      if ( TcpClientCmd::debug ) cout << "TcpClientCmd()" << endl;    
      TcpClientCmd::script = script;
    }
    
    ~TcpClientCmd()
    {
    
    }


    static int cmd()
    {
      if ( TcpClientCmd::debug ) cout << "cmd()" << endl;
    
      rl_attempted_completion_function = character_name_completion;

      read_history(TcpClientCmd::historyFile);
      rl_completer_word_break_characters = (char*)"";

      this_thread::sleep_for(std::chrono::milliseconds(1000));

      cout << color::rize("pHASE Interface, to get help write ", "Blue") << color::rize("help", "Yellow") << endl;

      cout << color::rize("use ", "Blue") << color::rize("UP/DOWN arrows", "Yellow") << color::rize(" for history or ", "Blue") << color::rize("[CTRL-r] + word", "Yellow") << color::rize(" to search history ", "Blue") << endl;

      cout << color::rize("use ", "Blue") << color::rize("[TAB]", "Yellow") << color::rize(" for available commands", "Blue") << endl;

      if ( TcpClientCmd::script != "" )
      {
        //TODO: check answer and return EXIT_FAULURE oder EXIT_SUCCESS
        while ( !gConnected )
          this_thread::sleep_for(std::chrono::milliseconds(1000));
        
        vector<string> scriptCommands = RegexFunctions::split(';', TcpClientCmd::script);
        
        for ( string scriptCommand : scriptCommands )
        {
          this_thread::sleep_for(std::chrono::milliseconds(1000));
          SSL_write(gSsl, scriptCommand.c_str(), scriptCommand.length());
        }
        exit(0);
      }
      while (true)
      {
        string command = "autocomplete";
		    //cout << "load: " << command << endl;
		    SSL_write(gSsl, command.c_str(), command.length());
        if ( TcpClientCmd::debug ) cout << "cmd() in while" << endl;
        char *buffer = readline("> ");  //TODO show cursor after async write
        if ( TcpClientCmd::debug ) cout << "cmd() in while got message" << endl;

        if (buffer)
        {
          write_history(TcpClientCmd::historyFile);
          string command(buffer);

          if ( RegexFunctions::trim(command) == "exit" )
          {
            cout << "EXITING..." << endl;
            exit(0);
          }
          add_history(buffer);
          if ( TcpClientCmd::debug ) cout << "cmd() pre SSL_write" << endl;

          if ( gConnected )
            SSL_write(gSsl, command.c_str(), command.length());
          free(buffer);
        }
        answered = false;
        int count = 0;
        while ( !answered )
        {
          this_thread::sleep_for(std::chrono::milliseconds(100));
          count++;
          if ( count > 5 ) answered = true;
        }
      }
      return 0;
    }
};


char ** character_name_completion(const char *text, int start, int end)
{
  rl_attempted_completion_over = 1;
  char** matches = rl_completion_matches(text, character_name_generator);         //complete word, eg "debug"

  return matches;
}

char * character_name_generator(const char *text, int state)
{
  static int list_index, len;
  char *name = (char*)"srqyvns";  //TODO: Was ist das?

  if (!state) {
    list_index = 0;
    len = strlen(text);

    string command = string(text);
    command = "autocomplete " + command;
    SSL_write(gSsl, command.c_str(), command.length());

  }

  int s = command_names.size();
  while (list_index<s) 
  {
    name = (char*)command_names.at(list_index++).c_str();
    if (strncmp(name, text, len) == 0) {
      return strdup(name);
    }
  }
  return NULL;
}


#endif

