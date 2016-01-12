#include <cstdio>
#include <cstdlib>

#include "myINI.h"
int INI_BUF_SIZE = 2048;

myINIParser::myINIParser()
{
    m_ini = new ini();
}

bool myINIParser::isComment(const string &str)
{
    for (auto x : m_ini->commentFlags)
    {
        if (str.find(x.c_str(), 0, x.length()) != string::npos)
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
        size_t index = line.find(x.c_str());
        if (index != string::npos)
        {
            line = line.substr(0, index);
        }
    }
    comment = str.substr(line.length());
    str = line;
    return comment;
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
        trimright(line, '\n');
        trimright(line, '\r');
        trim(line);

        if (!isComment(line))
        {
            comment += separateComment(line);
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
                fprintf(stderr, "没有找到匹配的]\n");
                return -1;
            }

            if (index <= 1)
            {
                fprintf(stderr, "段为空\n");
                continue;
            }

            string s(line, 1, index - 1);

            if (getSection(s))
            {
                fclose(fp);
                fprintf(stderr, "此段已存在:%s\n", s.c_str());
                return -1;
            }

            sectionTmp = new IniSection();
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
                item itemTmp;
                itemTmp.key     = key;
                itemTmp.value   = value;
                itemTmp.comment = comment;

                sectionTmp->items.push_back(itemTmp);
            } else {
                fprintf(stderr, "解析参数失败[%s]\n", line.c_str());
            }
        }



    }

    return 0;
}
