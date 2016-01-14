#include <cstdio>
#include <cstdlib>

#include "myINI.h"
namespace MY_INI
{

const int INI_BUF_SIZE = 2048;

myINIParser::myINIParser()
{
    m_ini = new ini();
}

myINIParser::~myINIParser()
{
    delete m_ini;
}

bool myINIParser::isComment(const string &str)
{
    for (auto x : m_ini->commentFlags)
    {
        if (str.find(x) == 0)
        {
            return true;
        }
    }
    return false;
}

string myINIParser::separateComment(string &str)
{
    string comment;
    string line = str;

    for (auto x : m_ini->commentFlags)
    {
        size_t index = line.find(x);
        if (index != string::npos)
        {
            line = line.substr(0, index);
        }
    }
    comment = str.substr(line.length());
    str = line;
    return comment;
}

void myINIParser::trimleft(string &str, char c/* = ' '*/)
{
    size_t index = str.find_last_not_of(c);

    if (index != string::npos)
    {
        str.erase(index + 1);
    }
    else
    {
        str.clear();
    }
}
void myINIParser::trimright(string &str, char c/* = ' '*/)
{
    size_t index = str.find_first_not_of(c);

    if (index != string::npos)
    {
        str = str.substr(index);
    }
    else
    {
        str.clear();
    }
}

void myINIParser::trim(string &str)
{
    size_t len = str.length();
    size_t i = 0;

    while (isspace(str[i]) && str[i] != '\0')
    {
        i++;
    }

    if (i != 0)
    {
        str = string(str, i, len - i);
    }

    len = str.length();
    for (i = len - 1; i >= 0; --i)
    {
        if (!isspace(str[i]))
        {
            break;
        }
    }

    str = string(str, 0, i + 1);
}

void myINIParser::getCommentFlags(vector<string> &flags) const
{
    flags = m_ini->commentFlags;
}

void myINIParser::setCommentFlags(const vector<string> &flags)
{
    m_ini->commentFlags = flags;
}

int myINIParser::getline(string &str, FILE *fp)
{
    char buf[MAX_LINE_NUM] = {0};
    size_t len = 0;
    if (fgets(buf, MAX_LINE_NUM, fp))
    {
        len = strlen(buf);
    }
    str = buf;
    return len;

}

int myINIParser::load(const string &filename)
{
    FILE *fp = fopen(filename.c_str(), "r");
    if (fp == NULL)
    {
        return -1;
    }

    m_ini->filename = filename;
    string line;
    string comment;
    section *sectionTmp = NULL;

    while (getline(line, fp) > 0)
    {
        trim(line);

        if (!isComment(line))
        {
            string tmpComment = separateComment(line);
            comment += tmpComment;
        }
        else
        {
            comment += line;
            continue;
        }

        trim(line);
        if (line.length() <= 0)
        {
            continue;
        }

        if (line[0] == '[')
        {
            sectionTmp = NULL;
            size_t index = line.find_first_of(']');

            if (index == string::npos)
            {
                fclose(fp);
                printf("没有找到匹配的]\n");
                return -1;
            }

            if (index <= 1)
            {
                printf("段为空\n");
                continue;
            }

            string s(line, 1, index - 1);
            if (getSection(s))
            {
                fclose(fp);
                printf("此段已存在:%s\n", s.c_str());
                return -1;
            }

            sectionTmp = new section();
            sectionTmp->name = s;
            sectionTmp->comment = comment;

            m_ini->sections[s] = sectionTmp;
            comment = "";
        }
        else
        {
            string key;
            string value;
            if (parse(line, key, value))
            {
                trim(key);
                trim(value);

                item* itemTmp = new item();
                itemTmp->key     = key;
                itemTmp->value   = value;
                itemTmp->comment = comment;

                sectionTmp->items.push_back(itemTmp);
                comment = "";
            }
            else
            {
                printf("解析参数失败[%s]\n", line.c_str());
            }
        }

    }

    return 0;
}

bool myINIParser::parse(const string &content, string &key, string &value, char c/*= '='*/)
{
    size_t index = content.find(c);
    if (index == string::npos || index == 0)
    {
        return false;
    }

    key = content.substr(0, index);
    value = content.substr(index + 1);

    return true;
}

section* myINIParser::getSection(const string &sectionStr) const
{
    auto it = m_ini->sections.find(sectionStr);
    if (it != m_ini->sections.end())
    {
        return it->second;
    }

    return NULL;
}

item* myINIParser::getItem(const string &sectionStr, const string &itemStr) const
{
    auto it = m_ini->sections.find(sectionStr);
    if (it == m_ini->sections.end())
    {
        return NULL;
    }

    for ( auto x : it->second->items)
    {
        if (x->key == itemStr)
        {
            return x;
        }
    }

    return NULL;
}

string myINIParser::getString(const string &sectionStr, const string &itemStr, const string &defaultValue/* = ""*/) const
{
    auto x = getItem(sectionStr, itemStr);
    if (!x)
    {
        return defaultValue;
    }
    return x->value;
}

int myINIParser::getInt(const string &sectionStr, const string &itemStr, int defaultValue /*= 0*/) const
{
    auto x = getItem(sectionStr, itemStr);
    if (!x)
    {
        return defaultValue;
    }
    return atoi(x->value.c_str());
}

double myINIParser::getDouble(const string &sectionStr, const string &itemStr, double defaultValue /*= 0*/) const
{
    auto x = getItem(sectionStr, itemStr);
    if (!x)
    {
        return defaultValue;
    }
    return atof(x->value.c_str());
}

void myINIParser::delSection(const string &sectionStr)
{
    auto it = m_ini->sections.find(sectionStr);
    if (it != m_ini->sections.end())
    {
        m_ini->sections.erase(it);
    }
}

void myINIParser::delItem(const string &sectionStr, const string &itemStr)
{
    auto sec = getSection(sectionStr);
    if (!sec)
    {
        return;
    }

    for (auto it = sec->items.begin(); it != sec->items.end();)
    {
        if ((*it)->key == itemStr)
        {
            delete *it;
            sec->items.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void myINIParser::setSectionValue(const string &sectionStr, const string &commentStr)
{
    auto sec = getSection(sectionStr);
    if (!sec)
    {
        return;
    }
    sec->comment = commentStr;
}

void myINIParser::setItemValue(const string &sectionStr, const string &itemStr, const string &valueStr, const string &commentStr/* = ""*/)
{
    auto x = getItem(sectionStr, itemStr);
    if (!x)
    {
        return;
    }
    x->value = valueStr;
    x->comment = commentStr;
}

int myINIParser::save(const string &filename) const
{
    FILE *fp = fopen(filename.c_str(), "w");
    if (fp == NULL)
    {
        return -1;
    }
    string buf = "";
    for (auto sec : m_ini->sections)
    {
        buf += "[" + sec.second->name + "] " + sec.second->comment + EndChars;
        for ( auto it : sec.second->items)
        {
            buf += it->comment + EndChars;
            buf += it->key + " = " + it->value + EndChars;
        }
        buf += EndChars;
    }
    fwrite(buf.c_str() , 1, buf.length(), fp);
    fclose(fp);
    return 0;
}

};
