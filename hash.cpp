# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <sstream>
# include <time.h>
using namespace std;


const int MOD = 0x7FFFFF;

unsigned int BKDRHash(const char *str){  // 哈希函数
    unsigned int seed = 131;
    unsigned int hash = 0;
    while (*str)    hash = hash * seed + (*str++);

    // return (hash & 0x7FFFFFFF);
    // return (hash & 0x7FFFFF);
    return (hash & MOD);
}

bool HasBom(const char* decKrc, uint32_t decKrcLen)
{
    if (decKrcLen >= 3 && 
    (unsigned char)decKrc[0] == 0xEF && 
    (unsigned char)decKrc[1] == 0xBB && 
    (unsigned char)decKrc[2] == 0xBF)   return true;
    else    return false;
}

struct WordInfo{
    int word_hash;
    int word_num;
    int url;
    WordInfo(int hash, int num, int url){
        this->word_hash = hash;
        this->word_num = num;
        this->url = url;
    }
    WordInfo(const WordInfo &other){
        this->word_hash = other.word_hash;
        this->word_num = other.word_num;
        this->url = other.url;
    }
};

struct WordCode{
    string word;
    int num;
    WordCode(string word, int num){
        this->word = word;
        this->num = num;
    }
    WordCode(const WordCode &other){
        this->word = other.word;
        this->num = other.num;
    }
};

string load_path = "data/news_words.csv";
string save_temp_index_path = "data/temp_index.csv";
string save_word_code_path = "data/word_code.csv";
// string save_url_code_path = "data/url_code.csv";
vector<WordInfo> temp_index;          // 临时索引文件
vector<WordCode> word_code(MOD, WordCode(" ", 0));    // 单词编号文件
// vector<string> url_code;                    // 网址编号文件
clock_t t_start, t_end;
bool DEBUG = false;

int main()
{
    int conflict_num = 0, conflict_word_num = 0;
    t_start = clock();
    cout << "————————————————————————————开始处理————————————————————————————" << endl;
    ifstream inFile(load_path);
    string line;
    int url_num = 0;
    while (getline(inFile,line)){
        if (HasBom(line.c_str(), line.size()))  line = line.substr(3); //处理UTF-8 BOM文件
        if (DEBUG){
            cout << "————————————————————————————开始处理第" << url_num+1 << "个网页————————————————————————————" << endl;
            cout << line.size() << endl;
        }
        // cout << url_num << endl;

        string url;
        istringstream ss(line);
        getline(ss, url, '\"');
        url = url.substr(0, url.size() - 1);
        // url_code.push_back(url);
        if (DEBUG)  cout << "url: " << url << endl;

        string title, words, word="";
        int word_hash = 0;
        getline(ss, title, '\"');
        getline(ss, words, '\"');
        getline(ss, words, '\"');
        words = title + ',' + words + ',';
        if (DEBUG) cout << "words: " << words << endl;

        for (unsigned int i=0; i<words.length(); i++){
            if (words[i] == ','){
                word_hash = BKDRHash(word.c_str());     // 获取哈希值
                // cout << word << ":" << word_hash << endl;
                if (word_code[word_hash].word ==  " "){    // 单词编号文件中没有该单词
                    word_code[word_hash].word = word;
                    word_code[word_hash].num = 1;
                }
                else if (word_code[word_hash].word == word){   // 单词已经存在，则记录次数
                    word_code[word_hash].num++;
                }
                else{   // 单词冲突
                    if (DEBUG) cout << "冲突：" << word << ":" << word_code[word_hash].word << word_hash << endl;
                    while (true){
                        if (word_code[word_hash].word == " "){
                            word_code[word_hash].word = word;
                            word_code[word_hash].num = 1;
                            conflict_word_num++;
                            break;
                        }
                        else if (word_code[word_hash].word == word){
                            word_code[word_hash].num++;
                            break;
                        }
                        else conflict_num++;
                        word_hash = (word_hash+1)%MOD;
                    }
                }
                word = "";
            }
            else{
                word += words[i];
            }
        }

        for (unsigned int i=0; i<MOD; i++){
            if (word_code[i].word != " " && word_code[i].num != 0){
                temp_index.push_back(WordInfo(i, word_code[i].num, url_num));
                word_code[i].num = 0;
            }
        }

        if (DEBUG) {
            cout << "临时索引表长:" << temp_index.size() << endl;
            cout << "————————————————————————————结束处理第" << url_num+1 << "个网页————————————————————————————" << endl;
        }
        url_num++;
    }
    inFile.close();

    ofstream outFile;
    outFile.open(save_temp_index_path);
    for (unsigned int i=0; i<temp_index.size(); i++){
        outFile << temp_index[i].word_hash << "," << temp_index[i].word_num << "," << temp_index[i].url << endl;
    }
    outFile.close();
    outFile.open(save_word_code_path);
    for (unsigned int i=0; i<word_code.size(); i++){
        if (word_code[i].word != " ")
            outFile << i << "," << word_code[i].word << endl;
    }
    outFile.close();
    // outFile.open(save_url_code_path);
    // for (unsigned int i=0; i<url_code.size(); i++){
    //     outFile << i << "," << url_code[i] << endl;
    // }
    // outFile.close();

    t_end = clock();
    cout << "运行时间"<< (double)(t_end-t_start)/CLOCKS_PER_SEC << "s" <<endl;
    cout << "处理文章数:" << url_num << endl;
    cout << "冲突数:" << conflict_num << endl;
    cout << "冲突单词数:" << conflict_word_num << endl;
    cout << "————————————————————————————处理完成————————————————————————————" << endl;

    return 0;
}