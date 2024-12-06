
#include <time.h>
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include "../results/release.cpp"
#include "../common/configFileReader.cpp"
#include "../common/regexParser.cpp"
#include "../common/regexFunctions.cpp"

using namespace std;

#ifndef FEATURE_EXTRACTOR
#define FEATURE_EXTRACTOR

/*******************************************************************************
* this module extracts the features configured in general.cfg (that is stored
* in the database)
*
* the order defines what features are used for this section
* eg. order TV = SECTION CODEC VIDEOSOURCE RESOLUTION LANGUAGE *TVGENRE *TVTYPE *TVCOUNTRY *DUPE INTERNAL *DUPECHECKEROVERRIDE
*
* the feature, eg. SECTION or CODEC ... consists of topics
* 
* some features have NO topics because they can be found directly release name
* feature CODEC = xvid h264 h265 x264 x265 DivX
*
* some features have topics because they can't be found directly release name
* feature SECTION = $GAMES $SPORTS XXX $MOVIES $TV PS4 $DOX $APPS WIIU NSW XBOX360 3DS $BOOKWARE $DAILYSHOWS NONE
*         =======                      ^------
* some features have topics, that describes more detailed what to look for
* in the release name, that have a $-sign
*
* the feature SECTION has Following topics
*             =======
* topic MOVIES = !$sport !$TVEP !$DATE $progressiveScan|$source|$codec
*       ------
* topic TV = !$sport $TVEP $progressiveScan|$source|$codec|$interlacedScan
* topic DAILYSHOWS = $dailyshow $DATE $progressiveScan|$source|$codec|$interlacedScan
* topic SPORTS = $sport $codec
* topic GAMES = $GAME-GROUPS !$DOX
* topic DOX = $DOX-GROUPS $DOX !Multi\d
* topic APPS = $UTIL-GROUPS !GAME
* topic BOOKWARE = $BOOKIE-GROUPS|$BOOKIE-COMPANY
*
* topic MOVIES = 
* !$sport
* !$TVEP
* !$DATE
* $progressiveScan|$source|$codec
*                 ^       ^
*                 OR      OR
*
* the topic MOVIES 
* must not be $sports AND
* must not be $TVEP (S01E03) AND
* must not have a date (like 12.03.2019, this would be for example a daily show)
* and must have progressive scan (1080p...) OR source (WEB, HDTV...) OR codec (x264, x265....)
*
* $TVEP refers to a keyword (TVEP means TV Episode)
* keyword TVEP = S\d+E\d+ E\d\d
* and uses regex, \d+ means one or more dIGIT so we have S\d+ woudl meand
* S01 or S001 or S1 and E\d+ would meand E01 or E001. and there is a second
* possible item E\d\d, because some releases have E01, E02, E03 but no Season
*
* WARNING: dont confuse $variable from features with
*                       $variable from topics whth
*                       $variable from keywords
*
* $variable from a features line refers to a topic so         TODO: maybe use here a @ instead of a $-sign
* $variable from a topic refers to a keyword
* $variable from a keyword refers another keyword
*******************************************************************************/
class FeatureExtractor
{

  private:


  public:
    inline static const bool debug = false;
    
    inline static ConfigFileReader* cfr = ConfigFileReader::getInstance();
    inline static RegexParser* rp = RegexParser::getInstance();


    static void getFeatures(Release &rls)
    {
      if ( FeatureExtractor::debug ) cout << "getFeatures()" << endl;
      rls.features["section"] = getTopic("section", cfr->generalCache.content["feature"]["section"], rls);
      

      string orderLine = cfr->generalCache.content["order"][rls.features["section"]];
      
      if ( FeatureExtractor::debug ) cout << "orderLine " << orderLine << endl;
      
      if ( FeatureExtractor::debug ) cout << "split features from orderline" << endl;
      vector<string> features = RegexFunctions::split(' ', orderLine);
      
      for ( auto feature : features )
      {
        if ( FeatureExtractor::debug ) cout << "for " << feature << endl;
        if ( feature.substr(0,1) == "*" )
        {
          if ( FeatureExtractor::debug ) cout << "CONTINUING SINCE FEATURE WILL BE SET ELSEWHERE" << endl;
          rls.features[feature] = "*";
          continue;
        }
        else
        {
          if ( FeatureExtractor::debug ) cout << feature.substr(0,1) << " " << feature << endl;
        }
        rls.features[feature] = getTopic(feature, cfr->generalCache.content["feature"][feature], rls);
        if ( FeatureExtractor::debug ) cout << "getTopic = " << rls.features[feature] << endl;
      }
    }      


/*
    static void getFeatures(Release &rls)
    {
      for ( auto keyValue : cfr->generalCache.content["feature"] )
      {
        rls.features[keyValue.first] = getTopic(keyValue.first, keyValue.second, rls);
      }
    }
*/

    static string getTopic(string key, string value, Release &rls)
    {
      if ( FeatureExtractor::debug ) cout << "getTopic() " << key << endl;
      istringstream iss(value);
      string topic;
      while ( getline( iss, topic, ' ' ) )
      {
        if ( FeatureExtractor::debug ) cout << "topic " << topic << endl;
        if ( topic.substr(0,1) == "$" )
        {
          if ( FeatureExtractor::debug ) cout << "$topic " << topic << endl;
          string term = cfr->generalCache.content["topic"][topic.substr(1)];
          if ( FeatureExtractor::debug ) cout << "term " << term << endl;
          if ( rp->parse(term, rls.releaseName, true) )                                      //TODO, maybe link singleton to static var or transform to singleton too
            return topic.substr(1);
        }
        else if ( RegexFunctions::match("."+topic+".", "."+rls.releaseName+"." ) )
          return topic;

      }
      return "*";
    }


    static void getSection(Release &rls)
    {
      if ( FeatureExtractor::debug ) cout << "getSEction()" << endl;
      string section = "";
      if ( rls.features["section"] == "*" )
        rls.features["section"] = "all";

      if ( FeatureExtractor::debug ) cout << "rls.features[section]" << endl;
      string order = rls.features["section"];
      string orderLine = cfr->generalCache.content["order"][order];
      istringstream iss(orderLine);
      string item;

      if ( FeatureExtractor::debug ) cout << "pre while " << orderLine << endl;
      while ( getline( iss, item, ' ' ) )
      {
        if ( FeatureExtractor::debug ) cout << "while " << item << endl;
        if ( item.substr(0,1) == "*" )
          item = item.substr(1);
        if ( rls.features[item] == "*" )
          continue;
        if ( rls.features[item] == "" ) //TODO: check that line
          continue;
        else
          section += "." + rls.features[item];
      }
      rls.section = section.substr(1);
    }    
};

#endif
