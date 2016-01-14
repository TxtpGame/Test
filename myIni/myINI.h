#ifndef INI_PARSER_H
#define INI_PARSER_H

#include <map>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

namespace MY_INI
{
#ifdef WIN32
#define EndChars "\r\n"
#else
#define EndChars "\n"
#endif

#define MAX_LINE_NUM 2048

struct item
{
    string key;
    string value;
    string comment;
    item()
    {
        key     = "";
        value   = "";
        comment = "";
    }
};

struct section
{
    string name;
    vector<item*> items;
    string comment;
    section()
    {
        name    = "";
        comment = "";
    }
    ~section()
    {
        for (auto i : items)
        {
            delete i;
        }
        name    = "";
        comment = "";
    }
};

struct ini
{
    string filename;
    map<string, section*> sections;
    vector<string> commentFlags;
    ini()
    {
        filename = "";
        commentFlags.push_back("#");
    }
    ~ini()
    {
        filename = "";
        for (auto s : sections)
        {
            delete s.second;
        }
    }
};

class myINIParser
{
public:
    myINIParser();
    ~myINIParser();

    int load(const string &filename);
    int save(const string &filename) const;

    section* getSection(const string &sectionStr) const;
    item* getItem(const string &sectionStr, const string &itemStr) const;
    string getString(const string &sectionStr, const string &itemStr, const string &defaultValue = "") const;
    int getInt(const string &sectionStr, const string &itemStr, int defaultValue = 0) const;
    double getDouble(const string &sectionStr, const string &itemStr, double defaultValue = 0) const;

    void delSection(const string &sectionStr);
    void delItem(const string &sectionStr, const string &itemStr);

    void setSectionValue(const string &sectionStr, const string &commentStr);
    void setItemValue(const string &sectionStr, const string &itemStr, const string &valueStr, const string &commentStr = "");

    void getCommentFlags(vector<string> &flags) const;
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
    bool parse(const string &content, string &key, string &value, char c = '=');
    string separateComment(string &str);
    int getline(string &str, FILE *fp);

private:
    ini *m_ini;

};

}
#endif
