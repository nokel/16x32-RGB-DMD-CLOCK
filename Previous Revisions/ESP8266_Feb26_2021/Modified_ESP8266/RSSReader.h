/*
 *  RSSReader.h - Header file for RSSReader Class
 *
 *  Concept, Design and Implementation by: Craig A. Lindley
 *  Last Update: 11/06/2015
 */

#ifndef RSSREADER_H
#define RSSREADER_H

#define TITLE_BUFFER_SIZE  256
#define DESC_BUFFER_SIZE   512
#define PUBDATE_BUFFER_SIZE 80

// A function pointer for callback
typedef void (*pt2Function)(char *);

class RSSReader {

  public:
    RSSReader(int _timeoutInMS);

    void setTitleCallback(pt2Function _titleCallback);
    void setDescCallback(pt2Function _descCallback);
    void setPubDateCallback(pt2Function _dateCallback);

    bool read(const char *url);

  private:
    int timeoutInMS;
    int titleIndex;
    int descIndex;
    int dateIndex;

    pt2Function titleCallback;
    pt2Function descCallback;
    pt2Function dateCallback;
    
    bool parseURL(const char* url);

    char titleBuffer[TITLE_BUFFER_SIZE];
    char descBuffer[DESC_BUFFER_SIZE];
    char dateBuffer[PUBDATE_BUFFER_SIZE];

    char host[30];
    char path[100];


    void runStateMachine(char ch);

};

#endif
