#include "myINI.h"

#include <iostream>

using namespace std;
using namespace MY_INI;

int main(int argc, char const *argv[])
{
    vector<string> flags = {";", "#"};
    myINIParser temp;
    temp.setCommentFlags(flags);

    temp.load("test1.ini");

    /* temp.delSection("zp");

     temp.delItem("zp1", "x");

     temp.setSectionValue("zp1", "#123");
     temp.setItemValue("zp1", "y", "2222", "#123");

     cout << temp.getInt("zp1", "t", 2) << endl;
     cout << temp.getDouble("zp1", "t2", 2.4444) << endl;
     cout << temp.getString("zp1", "t2", "123") << endl;*/

    temp.save("res.ini");
    return 0;
}
