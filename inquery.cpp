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


struct NewsInfo{
    int url;
    string word_info;
    NewsInfo(int url, string word_info){
        this->url = url;
        this->word_info = word_info;
    }
    NewsInfo(const NewsInfo &other){
        this->url = other.url;
        this->word_info = other.word_info;
    }
};


string load_invert_index_path = "data/invert_index.csv";
string load_word_code_path = "data/word_code.csv";
string load_url_code_path = "data/url_code.csv";
vector<string> word_code(0x7FFFFF, " ");
vector<string> url_code;
vector<NewsInfo> news;
int type = 0;   //0-并集，1-交集
bool DEBUG = true;


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
    cout << "输入要查询的单词(以空格分隔)：";
    getline(cin, line);
    cout << "输入要查询的类型(0-并集，1-交集)：";
    cin >> type;
    for (unsigned int i=0; i<line.size(); i++){
        if (line[i] == ' ') {
            words.push_back(word);
            word = "";
        }
        else word += line[i];
    }
    if (word != "") words.push_back(word);

    vector<int> num_list, url_list;
    for (unsigned int i=0; i<words.size(); i++){
        cout << "正在查找：" << words[i] << endl;
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
                if (DEBUG)  cout << word_hash << " " << word_code[word_hash] << " " << word_nums << " " << urls << endl;

                int number=0;
                for (unsigned int j=0; j<word_nums.size(); j++){
                    if (word_nums[j] == ',') {
                        num_list.push_back(number);
                        number = 0;
                    }
                    else{
                        number = number*10 + word_nums[j] - '0';
                    }
                }
                num_list.push_back(number);
                number = 0;
                for (unsigned int j=0; j<urls.size(); j++){
                    if (urls[j] == ',') {
                        url_list.push_back(number);
                        number = 0;
                    }
                    else{
                        number = number*10 + urls[j] - '0';
                    }
                }
                url_list.push_back(number);

                if (type == 0){     //并集
                    if (news.empty()) {
                        for (unsigned int j=0; j<url_list.size(); j++){
                            news.push_back(NewsInfo(url_list[j], words[i]+":"+to_string(num_list[j])));
                        }
                    }
                    else {
                        for (unsigned int j=0; j<url_list.size(); j++){
                            for (unsigned int k=0; k<news.size(); k++){
                                if (news[k].url == url_list[j]){
                                    news[k].word_info += ", "+words[i]+":"+to_string(num_list[j]);
                                    break;
                                }
                                if (k == news.size()-1) news.push_back(NewsInfo(url_list[j], words[i]+":"+to_string(num_list[j])+"次"));
                            }
                        }
                    }
                }
                else{   //交集
                    if (news.empty()) {
                        for (unsigned int j=0; j<url_list.size(); j++){
                            news.push_back(NewsInfo(url_list[j], words[i]+":"+to_string(num_list[j])+"次"));
                        }
                    }
                    else {
                        vector<NewsInfo> temp_news_code;
                        for (unsigned int j=0; j<news.size(); j++){
                            for (unsigned int k=0; k<url_list.size(); k++){
                                if (news[j].url == url_list[k]){
                                    temp_news_code.push_back(NewsInfo(url_list[k], news[j].word_info+", "+words[i]+":"+to_string(num_list[k])+"次"));
                                    break;
                                }
                            }
                        }
                        news = temp_news_code;
                    }
                }
                break;
            }
        }
        inFile.close();
        num_list.clear();
        url_list.clear();
    }

    cout << "————————————————————————————查询结果————————————————————————————" << endl;
    for (unsigned int i=0; i<news.size(); i++){
        cout << url_code[news[i].url] << endl;
        cout << news[i].word_info << endl;
        cout << endl;
    }

    return 0;
}