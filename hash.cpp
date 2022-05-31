# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <sstream>
using namespace std;


unsigned int BKDRHash(const char *str){  // 哈希函数
    unsigned int seed = 131;
    unsigned int hash = 0;
    while (*str)    hash = hash * seed + (*str++);

    // return (hash & 0x7FFFFFFF);
    return (hash & 0x7FFFFF);
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
};

string load_path = "data/news_words.csv";
string save_temp_index_path = "data/temp_index.csv";
string save_word_code_path = "data/word_code.csv";
string save_url_code_path = "data/url_code.csv";
vector<WordInfo> temp_index;          // 临时索引文件
vector<pair<string, int>> word_code(0x7FFFFF, make_pair(" ", 0));    // 单词编号文件
vector<string> url_code;                    // 网址编号文件

int main()
{
    ifstream inFile(load_path);
    string line;
    int url_num = 0;
    while (getline(inFile,line)){
        cout << "————————————————————————————开始处理第" << url_num+1 << "个网页————————————————————————————" << endl;
        cout << line.size() << endl;
        
        if (HasBom(line.c_str(), line.size()))  line = line.substr(3); //处理UTF-8 BOM文件
        // cout << line << endl;

        string url;
        istringstream ss(line);
        getline(ss, url, '\"');
        url = url.substr(0, url.size() - 1);
        url_code.push_back(url);
        cout << "url:" << url << endl;

        string title, words, word="";
        int word_hash = 0;
        getline(ss, title, '\"');
        getline(ss, words, '\"');
        getline(ss, words, '\"');
        words = title + ',' + words + ',';
        cout << words << endl;
        for (unsigned int i=0; i<words.length(); i++){
            if (words[i] == ','){
                word_hash = BKDRHash(word.c_str());     // 获取哈希值
                // cout << word << ":" << word_hash << endl;
                if (word_code[word_hash].first ==  " "){    // 单词编号文件中没有该单词
                    word_code[word_hash].first = word;
                    word_code[word_hash].second = 1;
                    // temp_index.push_back(WordInfo(word_hash, 0, url_num));
                }
                else if (word_code[word_hash].first == word){   // 单词已经存在，则记录次数
                    word_code[word_hash].second++;
                }
                else{   // 单词冲突
                    while (word_code[(++word_hash)%0x7FFFFF].first == " "){
                        word_code[word_hash].first = word;
                        word_code[word_hash].second = 1;
                        // temp_index.push_back(WordInfo(word_hash, 0, url_num));
                        break;
                    }
                }
                word = "";
            }
            else{
                word += words[i];
            }
        }

        for (unsigned int i=0; i<0x7FFFFF; i++){
            if (word_code[i].first != " " && word_code[i].second != 0){
                temp_index.push_back(WordInfo(BKDRHash(word_code[i].first.c_str()), word_code[i].second, url_num));
                word_code[i].second = 0;
            }
        }

        cout << temp_index.size() << endl;
        url_num++;
        // if (url_num == 1)   break;
    }

    ofstream outFile;
    outFile.open(save_temp_index_path);
    for (unsigned int i=0; i<temp_index.size(); i++){
        outFile << temp_index[i].word_hash << "," << temp_index[i].word_num << "," << temp_index[i].url << endl;
    }
    outFile.close();
    outFile.open(save_word_code_path);
    for (unsigned int i=0; i<word_code.size(); i++){
        if (word_code[i].first != " ")
            outFile << i << "," << word_code[i].first << endl;
    }
    outFile.close();
    outFile.open(save_url_code_path);
    for (unsigned int i=0; i<url_code.size(); i++){
        outFile << i << "," << url_code[i] << endl;
    }


    return 0;
}