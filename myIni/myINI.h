#ifndef INI_PARSER_H
#define INI_PARSER_H

#include <map>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

namespace MY_INI
{
struct item
{
    string key;
    string value;
    string comment;
};

struct section
{
    string name;
    vector<item*> items;
    string comment;
};

struct ini
{
    string filename;
    map<string, section*> sections;
    vector<string> commentFlags;
};

class myINIParser
{
public:
    myINIParser();
    ~myINIParser();

    int load(const string &filename);
    section* getSection(const string &section) const;
    item* getItem(const string &section, const string &item) const;

    /*获取注释标记符列表*/
    void getCommentFlags(vector<string> &flags) const;
    /*设置注释标记符列表*/
    void setCommentFlags(const vector<string> &flags);

public:
    /*去掉str后面的c字符*/
    static void trimleft(string &str, char c = ' ');
    /*去掉str前面的c字符*/
    static void trimright(string &str, char c = ' ');
    /*去掉str前面和后面的空格符,Tab符等空白符*/
    static void trim(string &str);

private:
    bool isComment(const string &str);


private:
    ini *m_ini;

};


}



#endif
