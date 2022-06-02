# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <sstream>
# include <time.h>
#include <io.h>
using namespace std;


unsigned int BKDRHash(const char *str){  // 哈希函数
    unsigned int seed = 131;
    unsigned int hash = 0;
    while (*str)    hash = hash * seed + (*str++);

    // return (hash & 0x7FFFFFFF);
    return (hash & 0x7FFFFF);
}


string load_invert_index_path = "data/invert_index.csv";
string load_word_code_path = "data/word_code.csv";
string load_url_code_path = "data/url_code.csv";
vector<string> word_code(0x7FFFFF, " ");
vector<string> url_code;
bool DEBUG = false;


int main()
{
    ifstream inFile(load_word_code_path);
    string line;
    while (getline(inFile,line)){
        for (unsigned int i=0; i<line.size(); i++){
            if (line[i] == ',') line[i] = ' ';
        }
        stringstream ss(line);
        int code;
        string word;
        ss >> code >> word;
        word_code[code] = word;
    }
    inFile.close();
    inFile.open(load_url_code_path);
    while (getline(inFile,line)){
        for (unsigned int i=0; i<line.size(); i++){
            if (line[i] == ',') line[i] = ' ';
        }
        stringstream ss(line);
        int code;
        string url;
        ss >> code >> url;
        url_code.push_back(url);
    }
    inFile.close();

    string word="";
    vector<string> words;
    cout << "输入要查询的单词：";
    getline(cin, line);
    for (unsigned int i=0; i<line.size(); i++){
        if (line[i] == ' ') {
            words.push_back(word);
            word = "";
        }
        else word += line[i];
    }
    if (word != "") words.push_back(word);

    vector<int> news_code;
    vector<int> num_list, url_list;
    for (unsigned int i=0; i<words.size(); i++){
        inFile.open(load_invert_index_path);
        while (getline(inFile,line)){
            for (unsigned int j=0; j<line.size(); j++){
                if (line[j] == '\"' && line[j-1] == ',') line[j-1] = ' ';
            }
            stringstream ss(line);
            unsigned int word_hash;
            string word_nums, urls;
            ss >> word_hash >> word_nums >> urls;
            // cout << word_hash << " " << word_code[word_hash] << " " << words[i] << endl;

            if (word_code[word_hash] == words[i]){
                word_nums = word_nums.substr(1, word_nums.size()-2);
                urls = urls.substr(1, urls.size()-2);
                cout << word_hash << " " << word_code[word_hash] << " " << word_nums << " " << urls << endl;
                int number=0;
                for (unsigned int j=0; j<word_nums.size(); j++){
                    if (word_nums[j] == ',') {
                        cout << number << " ";
                        num_list.push_back(number);
                        number = 0;
                    }
                    else{
                        number = number*10 + word_nums[j] - '0';
                    }
                }
                num_list.push_back(number);
                cout << number << endl;
                number = 0;
                for (unsigned int j=0; j<urls.size(); j++){
                    if (urls[j] == ',') {
                        cout << number << " ";
                        url_list.push_back(number);
                        number = 0;
                    }
                    else{
                        number = number*10 + urls[j] - '0';
                    }
                }
                url_list.push_back(number);
                cout << number << endl;

                if (news_code.empty()) {
                    for (unsigned int j=0; j<num_list.size(); j++){
                        news_code.push_back(url_list[j]);
                    }
                }
                else {
                    for (unsigned int j=0; j<num_list.size(); j++){
                        for (unsigned int k=0; k<news_code.size(); k++){
                            if (news_code[k] == url_list[j]) break;
                            if (k == news_code.size()-1) news_code.push_back(url_list[j]);
                        }
                    }
                }
                break;
            }
        }
        inFile.close();
    }

    cout << "查询结果：" << endl;
    for (unsigned int i=0; i<news_code.size(); i++){
        cout << news_code[i] << " " << url_code[news_code[i]] << endl;
    }

    

    return 0;
}