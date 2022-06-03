# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <sstream>
# include <time.h>
# include <io.h>
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
vector<pair<string, string>> url_code;
vector<NewsInfo> news;
int type = 0;   //0-并集，1-交集
clock_t t_start, t_end;
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
            else if (line[i] == '\"') line[i] = ' ';
        }
        line += "\"";
        istringstream ss(line);
        int code;
        string url, title;
        ss >> code >> url;
        getline(ss, title, '\"');
        url_code.push_back(make_pair(url, title));
    }
    inFile.close();

    string word="";
    vector<string> words;
    string state="y";
    do{
        cout << "输入要查询的单词(以空格分隔)：";
        getline(cin, line);
        for (unsigned int i=0; i<line.size(); i++){
            if (line[i] == ' ') {
                words.push_back(word);
                word = "";
            }
            else word += line[i];
        }
        if (word != "") words.push_back(word);

        if (words.size() == 0) continue;
        else if (words.size()>1){
            cout << "输入要查询的类型(0-并集，1-交集)：";
            cin >> type;
        }

        t_start = clock();
        cout << "————————————————————————————开始查找————————————————————————————" << endl;

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

                    cout << "查找成功，包含网页数：" << url_list.size() << endl;

                    if (type == 0){     //并集
                        if (news.empty()) {
                            for (unsigned int j=0; j<url_list.size(); j++){
                                news.push_back(NewsInfo(url_list[j], words[i]+":"+to_string(num_list[j])+"次"));
                            }
                        }
                        else {
                            unsigned int len = news.size();
                            for (unsigned int j=0; j<url_list.size(); j++){
                                for (unsigned int k=0; k<len; k++){
                                    if (news[k].url == url_list[j]){
                                        news[k].word_info += ", "+words[i]+":"+to_string(num_list[j])+"次";
                                        break;
                                    }
                                    if (k == len-1) news.push_back(NewsInfo(url_list[j], words[i]+":"+to_string(num_list[j])+"次"));
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
            if (url_list.empty()) cout << "查找失败" << endl;
            inFile.close();
            num_list.clear();
            url_list.clear();
        }

        t_end = clock();
        cout << "————————————————————————————查询结果————————————————————————————" << endl;
        cout << "运行时间"<< (double)(t_end-t_start)/CLOCKS_PER_SEC << "s" <<endl;
        for (unsigned int i=0; i<news.size(); i++){
            cout << i+1 << "： " << url_code[news[i].url].first << endl;
            cout << "标题：" << url_code[news[i].url].second << endl;
            cout << news[i].word_info << endl;
            cout << endl;
        }
        if (news.empty()) cout << "查询结果为空" << endl << endl;;

        cout << "是否继续查询？(y/n)：";
        cin >> state;
        if (state == "y"){
            news.clear();
            words.clear();
            word = "";
            system("cls");
            getchar();
        }
    }while(state=="y");

    return 0;
}