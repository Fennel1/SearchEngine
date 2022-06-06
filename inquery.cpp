# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <sstream>
# include <time.h>
# include <io.h>
using namespace std;


const int MOD = 0x7FFFFF;

struct NewsInfo{
    int url;
    vector<int> word_num;
    vector<string> words;
    NewsInfo(int url, vector<int> word_num, vector<string> words){
        this->url = url;
        this->word_num = word_num;
        this->words = words;
    }
    NewsInfo(const NewsInfo &other){
        this->url = other.url;
        this->word_num = other.word_num;
        this->words = other.words;
    }
};


struct URLInfo{
    string url;
    string title;
    int day, year;
    double score;
    URLInfo(string url, string title, int day, int year, double score){
        this->url = url;
        this->title = title;
        this->day = day;
        this->year = year;
        this->score = score;
    }
    URLInfo(const URLInfo &other){
        this->url = other.url;
        this->title = other.title;
        this->day = other.day;
        this->year = other.year;
        this->score = other.score;
    }
};


unsigned int BKDRHash(const char *str){  // 哈希函数
    unsigned int seed = 131;
    unsigned int hash = 0;
    while (*str)    hash = hash * seed + (*str++);

    // return (hash & 0x7FFFFFFF);
    // return (hash & 0x7FFFFF);
    return (hash & MOD);
}


void QuickSort(vector<NewsInfo> &mylist, int low, int high)
{
    int i=low, j=high;
    if (low > high)     return;

    NewsInfo tmp = mylist[low];
    while (low < high){
        while (mylist[high].word_num.size() <= tmp.word_num.size() && low < high) high--;
        mylist[low] = mylist[high];

        while(mylist[low].word_num.size() >= tmp.word_num.size() && low < high) low++;
        mylist[high] = mylist[low];
    }

    mylist[low] = tmp;
    QuickSort(mylist, i, low-1);
    QuickSort(mylist, low+1, j);
}


void QuickSortTime(vector<NewsInfo> &mylist, vector<URLInfo> &url_code, int low, int high)
{
    int i=low, j=high;
    if (low > high)     return;

    NewsInfo tmp = mylist[low];
    while (low < high){
        while (((url_code[mylist[high].url].year < url_code[tmp.url].year) ||
         ((url_code[mylist[high].url].year == url_code[tmp.url].year) && (url_code[mylist[high].url].day <= url_code[tmp.url].day))) && 
         low < high) high--;
        mylist[low] = mylist[high];

        while(((url_code[mylist[low].url].year > url_code[tmp.url].year) || 
        ((url_code[mylist[low].url].year == url_code[tmp.url].year) && (url_code[mylist[low].url].day >= url_code[tmp.url].day))) && 
        low < high) low++;
        mylist[high] = mylist[low];
    }

    mylist[low] = tmp;
    QuickSortTime(mylist, url_code, i, low-1);
    QuickSortTime(mylist, url_code, low+1, j);
}


void QuickSortScore(vector<NewsInfo> &mylist, vector<URLInfo> &url_code, int low, int high)
{
    int i=low, j=high;
    if (low > high)     return;

    NewsInfo tmp = mylist[low];
    while (low < high){
        while (url_code[mylist[high].url].score <= url_code[tmp.url].score && low < high) high--;
        mylist[low] = mylist[high];

        while(url_code[mylist[low].url].score >= url_code[tmp.url].score && low < high) low++;
        mylist[high] = mylist[low];
    }

    mylist[low] = tmp;
    QuickSortScore(mylist, url_code, i, low-1);
    QuickSortScore(mylist, url_code, low+1, j);
}


void Merge(vector<NewsInfo> &mylist, int low, int high, int mid, int p)
{
    int i=low, j=mid+1, k=0, len=high-low+1;
    vector<NewsInfo> templist(len, NewsInfo(0, vector<int>(), vector<string>()));

    while (i<=mid && j<=high){ 
        if (mylist[i].word_num[p] >= mylist[j].word_num[p]){
            templist[k].url = mylist[i].url;
            templist[k].word_num = mylist[i].word_num;
            templist[k].words = mylist[i].words;
            k++;
            i++;
        }
        else{
            templist[k].url = mylist[j].url;
            templist[k].word_num = mylist[j].word_num;
            templist[k].words = mylist[j].words;
            k++;
            j++;
        }
    }
    while(i <= mid){
        templist[k].url = mylist[i].url;
        templist[k].word_num = mylist[i].word_num;
        templist[k].words = mylist[i].words;
        k++;
        i++;
    }
    while(j <= high) {
        templist[k].url = mylist[j].url;
        templist[k].word_num = mylist[j].word_num;
        templist[k].words = mylist[j].words;
        k++;
        j++;
    }

    for (int i=0; i<len; i++)  mylist[low+i] = templist[i];
}


void MergeSort(vector<NewsInfo> &mylist, int low, int high, int p)
{
    if (low < high)
    {
        int mid = (low+high)/2;
        MergeSort(mylist, low, mid, p);
        MergeSort(mylist, mid+1, high, p);
        Merge(mylist, low, high, mid, p);
    }
}


string load_invert_index_path = "data/invert_index.csv";
string load_word_code_path = "data/word_code.csv";
string load_url_code_path = "data/url_code.csv";
string save_ans_path = "data/ans.csv";
vector<string> word_code(MOD, " ");
vector<URLInfo> url_code;
vector<NewsInfo> news;
int type = 0;   //0-并集，1-交集
int model = 0;  //0-按照日期, 1-按照权重
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
        int code, day, year;
        double score;
        string url, title;
        ss >> code >> year >> day >> score >> url;
        getline(ss, title, '\"');
        // cout << code << " " << day << " " << score << " " << url << " " << title << endl;
        url_code.push_back(URLInfo(url, title, day, year, score));
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

        cout << "输入要查询的模式(0-按照时间，1-按照权重)：";
        cin >> model;

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
                                vector<int> tmp_word_num;
                                vector<string> tmp_words;
                                tmp_word_num.push_back(num_list[j]);
                                tmp_words.push_back(words[i]);
                                news.push_back(NewsInfo(url_list[j], tmp_word_num, tmp_words));
                            }
                        }
                        else {
                            unsigned int len = news.size();
                            for (unsigned int j=0; j<url_list.size(); j++){
                                for (unsigned int k=0; k<len; k++){
                                    if (news[k].url == url_list[j]){
                                        news[k].word_num.push_back(num_list[j]);
                                        news[k].words.push_back(words[i]);
                                        break;
                                    }
                                    if (k == len-1){
                                        vector<int> tmp_word_num;
                                        vector<string> tmp_words;
                                        tmp_word_num.push_back(num_list[j]);
                                        tmp_words.push_back(words[i]);
                                        news.push_back(NewsInfo(url_list[j], tmp_word_num, tmp_words));
                                    }
                                }
                            }
                        }
                    }
                    else{   //交集
                        if (news.empty()) {
                            for (unsigned int j=0; j<url_list.size(); j++){
                                vector<int> tmp_word_num;
                                vector<string> tmp_words;
                                tmp_word_num.push_back(num_list[j]);
                                tmp_words.push_back(words[i]);
                                news.push_back(NewsInfo(url_list[j], tmp_word_num, tmp_words));
                            }
                        }
                        else {
                            vector<NewsInfo> temp_news_code;
                            for (unsigned int j=0; j<news.size(); j++){
                                for (unsigned int k=0; k<url_list.size(); k++){
                                    if (news[j].url == url_list[k]){
                                        news[j].word_num.push_back(num_list[k]);
                                        news[j].words.push_back(words[i]);
                                        temp_news_code.push_back(NewsInfo(url_list[k], news[j].word_num, news[j].words));
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

        // 排序
        if (!news.empty()){
            if (type == 0){
                QuickSort(news, 0, news.size()-1);
                int num=news[0].word_num.size(), l=0;
                for (unsigned int i=0; i<=news.size(); i++){
                    if (i == news.size()){
                        if (model == 0) QuickSortTime(news, url_code, l, i-1);
                        else    QuickSortScore(news, url_code, l, i-1);
                        for (int j=num-1; j>=0; j--){
                            MergeSort(news, l, i-1, j);
                        }
                        break;
                    }
                    if (int(news[i].word_num.size()) != num){
                        if (model == 0) QuickSortTime(news, url_code, l, i-1);
                        else    QuickSortScore(news, url_code, l, i-1);
                        for (int j=num-1; j>=0; j--){
                            MergeSort(news, l, i-1, j);
                        }
                        l = i;
                        num = news[i].word_num.size();
                    }
                }
            }
            else{
                if (model == 0) QuickSortTime(news, url_code, 0, news.size()-1);
                else    QuickSortScore(news, url_code, 0, news.size()-1);
                for (int i=news[0].word_num.size()-1; i>=0; i--){
                    MergeSort(news, 0, news.size()-1, i);
                }
            }
        }

        t_end = clock();
        cout << "————————————————————————————查询结果————————————————————————————" << endl;
        cout << "运行时间"<< (double)(t_end-t_start)/CLOCKS_PER_SEC << "s" <<endl;
        if (news.empty()) cout << "查询结果为空" << endl << endl;
        else{
            for (unsigned int i=0; i<news.size() && i<200; i++){
                cout << i+1 << "： " << url_code[news[i].url].url << endl;
                cout << "标题：" << url_code[news[i].url].title << endl;
                cout << "关键词：";
                for (unsigned int j=0; j<news[i].words.size(); j++){
                    cout << news[i].words[j] << ":" << news[i].word_num[j] << "次  ";
                }
                cout << endl;
                if (model == 0) cout << "日期：" << url_code[news[i].url].year << "年" << url_code[news[i].url].day/100 << "月" << url_code[news[i].url].day%100 << "日" << endl;
                else    cout << "得分：" << url_code[news[i].url].score << endl;
                cout << endl;
            }
        }
        if (news.size() > 200) cout << "查询结果超过200条，全部查询结果储存于ans.csv" << endl;

        if (!news.empty()){ // 更新权重
            for (unsigned int i=0; i<news.size(); i++){
                for (unsigned int j=0; j<news[i].word_num.size(); j++){
                    url_code[news[i].url].score += double(news[i].word_num[j])/news.size();
                }
            }
            ofstream file;
            file.open(load_url_code_path, fstream::binary);
            file.close();
            file.open(load_url_code_path, ios::app);
            for (unsigned int i=0; i<url_code.size(); i++){
                file << i << "," << url_code[i].year << "," << url_code[i].day << "," << url_code[i].score << "," << url_code[i].url << "," << "\""+url_code[i].title.substr(1, url_code[i].title.size()-1)+"\"" << endl;
            }
            file.close();

            file.open(save_ans_path, fstream::binary);
            file.close();
            file.open(save_ans_path, ios::app);
            for (unsigned int i=0; i<news.size(); i++){
                file << i+1 << "," << url_code[news[i].url].url << "," << url_code[news[i].url].title << "," << url_code[news[i].url].year << "," << url_code[news[i].url].day << "," << url_code[news[i].url].score << endl;
            }
        }

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