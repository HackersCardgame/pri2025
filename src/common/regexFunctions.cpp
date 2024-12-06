#include "../lib/jpcre2-10.31.03/src/jpcre2.hpp"

#include <sstream>
#include <iostream>
//#include <regex>  the standard regex libaray is  very slow, using pcre2
#include <vector>

using namespace std;

#ifndef REGEX_FUNCTIONS
#define REGEX_FUNCTIONS

typedef jpcre2::select<char> jp;

/*******************************************************************************
* does what it says: here are some useful regex functions to
* - match() something
* - to find occurances() of something
* - to replace() things
* - to trim() something
* - to split() somethings
* - to findAll() things
*******************************************************************************/
class RegexFunctions
{

  inline static const bool debug = false;


  public:

/* 50x slower than PCRE2
    static bool match(string search, string text)
    {
      regex b(search);
      return std::regex_match(text, b);
    }*/


    static bool match(string search, string text)
    {
      return jp::Regex(search,"i").match(text,"g");  //i menas caseInsensitiv, g means global match
    }

    static int occurances(string search, string text)
    {
      return jp::Regex(search,"i").match(text,"g");  //i menas caseInsensitiv, g means global match
    }
    
    static string replace(string toReplace, string replacementString, string text)
    {
      jp::RegexReplace rr;
//TODO: crashes if string to replace does not exist
      jp::Regex re(toReplace);

      return rr.setSubject(text)
      .setRegexObject(&re)
      .setReplaceWith(replacementString)
      .addModifier("g")
      .replace();
    }
    
    static string ltrim(string text)
    {
      jp::RegexReplace rr;

      jp::Regex re("^[ ]*");

      return rr.setSubject(text)
      .setRegexObject(&re)
      .setReplaceWith("")
      .addModifier("g")
      .replace();
    }
    
    static string rtrim(string text)
    {
      jp::RegexReplace rr;

      jp::Regex re("[ ]*$");

      return rr.setSubject(text)
      .setRegexObject(&re)
      .setReplaceWith("")
      .addModifier("g")
      .replace();
    }
    
    static string trim(string text)
    {
      return ltrim(rtrim(text));
    }    

    static vector<std::string> split(char delimiter, string text)
    {
      stringstream textStream;
      textStream << trim(text);
      string segment;
      vector<std::string> seglist;

      while(getline(textStream, segment, delimiter))  //XXX seems to remove trailing and leading spaces 
      {
         seglist.push_back(segment);
      }
      if (RegexFunctions::debug) cout << seglist.size() << endl << endl ;
      return seglist;
    }

//TODO: Check if regex compiled successfully, as a function but costs time

    static string unify(string text)
    {
      string result;
      //result = replace("-", ".", text);
      result = replace("_", ".", text);
      result = replace(" ", ".", result);
      result = replace("'", ".", result);
      result = replace(":", ".", result);
      result = replace("\"", ".", result);
      result = replace("\'", ".", result);
      result = replace("\\$", ".", result);
      result = replace(",", ".", result);
      return result;
    }
    
    static vector<string> findAll(string what, string text)
    {
      jp::Regex re(what);
      re.addModifier("i").compile();  //TODO: maybe optional as parameter
      
      jp::VecNum vec_num;    ///Vector to store numbered substring vector.
//      jp::VecNas vec_nas;   ///Vector to store named substring Map.
//      jp::VecNtN vec_ntn;   ///Vector to store Named substring to Number Map.
      std::string ac_mod="g";   // g is for global match. Equivalent to using setFindAll() or FIND_ALL in addJpcre2Option()
      jp::RegexMatch rm;
      rm.setRegexObject(&re)
        .setSubject(text)                        //set subject string
        .addModifier(ac_mod)                         //add modifier
        .setNumberedSubstringVector(&vec_num)        //pass pointer to vector of numbered substring vectors
//        .setNamedSubstringVector(&vec_nas)           //pass pointer to vector of named substring maps
//        .setNameToNumberMapVector(&vec_ntn)          //pass pointer to vector of name to number maps
        .match();                                    //Finally perform the match()

      vector<string> result;

      for(auto val : vec_num)
      {
        if (RegexFunctions::debug) cout << val[0] << endl;
        result.push_back(val[0]);
      }
      return result;
    }
};
#endif
