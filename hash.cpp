# include <iostream>
# include <fstream>
# include <vector>
# include <string>
using namespace std;

unsigned int BKDRHash(const char *str){
    unsigned int seed = 131;
    unsigned int hash = 0;
    while (*str){
        hash = hash * seed + (*str++);
    }

    // return (hash & 0x7FFFFFFF);
    return (hash & 0x7FFFFF);
}

int main()
{
    cout << BKDRHash("我爱你小宝贝") << endl;

    return 0;
}