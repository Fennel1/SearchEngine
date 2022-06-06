# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <sstream>
# include <time.h>
# include <io.h>
using namespace std;

bool DEBUG = false;
const int MOD = 0x7FFFFF;

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


int GetFileNum(string path)
{
    int hFile=0, file_num=0;
    struct _finddata_t fileInfo;
    string pathName;
    if ((hFile = _findfirst(pathName.assign(path).append("\\*.csv").c_str(), &fileInfo)) == -1) return 0;
    do{
        // cout << fileInfo.name << endl;
        if (fileInfo.name[0]>='0' && fileInfo.name[0]<='9') file_num++;
    }while (_findnext(hFile, &fileInfo) == 0);
    _findclose(hFile);
    return file_num;
}


void Adjust(vector<int> &lt, vector<WordInfo> &now, int i, int k)
{
    int tmp = (i+k)/2;
    while (tmp>0){
        if ((now[i].word_hash > now[lt[tmp]].word_hash)
             || ((now[i].word_hash == now[lt[tmp]].word_hash) 
            //  && (now[i].url > now[lt[tmp]].url))){
             && (now[i].word_num < now[lt[tmp]].word_num))){
            int t = lt[tmp];
            lt[tmp] = i;
            i = t;
        }
        tmp = tmp/2;
    }
    lt[0] = i;
}


void CreateLoserTree(vector<int> &lt, vector<WordInfo> &now, int k)
{
    now[k].word_hash = -1;
    for (int i=0; i<k; i++)    lt[i] = k;
    for (int i=k-1; i>=0; i--)  Adjust(lt, now, i, k);
}


void GetNext(ifstream &file, vector<WordInfo> &now, int i)
{
    string line;
    if (now[i].word_hash == MOD) return;
    if (getline(file, line)){
        for (unsigned int i=0; i<line.size(); i++){
            if (line[i] == ',') line[i] = ' ';
        }
        int word_hash=0, word_num=0, url_code=0;
        stringstream ss(line);
        ss >> word_hash >> word_num >> url_code;
        now[i].word_hash = word_hash;
        now[i].word_num = word_num;
        now[i].url = url_code;
        if (DEBUG)  cout << "now[" << i << "] = " << now[i].word_hash << " " << now[i].word_num << " " << now[i].url << endl;
    }
    else{
        now[i].word_hash = MOD;
    }
}


string load_path = "data/";
string save_path = "data/invert_index.csv";
vector<WordInfo> temp_index;
clock_t t_start, t_end;


int main()
{
    t_start = clock();
    cout << "————————————————————————————开始处理————————————————————————————" << endl;
    int file_num = GetFileNum(load_path);

    vector<int> LoserTree(file_num);
    vector<WordInfo> now(file_num+1, WordInfo(0, 0, 0));
    ifstream files[file_num];
    for (int i=0; i<file_num; i++)  files[i].open(load_path+to_string(i+1)+".csv");

    for (int i=0; i<file_num; i++){
        GetNext(files[i], now, i);
    }
    CreateLoserTree(LoserTree, now, file_num);

    ofstream file;
    file.open(save_path, fstream::binary);
    file.close();
    ofstream outFile;
    outFile.open(save_path, ios::app);
    while(now[LoserTree[0]].word_hash != MOD){
        // cout << now[LoserTree[0]].word_hash << " " << now[LoserTree[0]].word_num << " " << now[LoserTree[0]].url << endl;
        // outFile << now[LoserTree[0]].word_hash << "," << now[LoserTree[0]].word_num << "," << now[LoserTree[0]].url << endl;
        if (!temp_index.empty() && temp_index.back().word_hash != now[LoserTree[0]].word_hash){
            string word_num_list="\""+to_string(temp_index[0].word_num), url_list="\""+to_string(temp_index[0].url);
            for (unsigned int i=1; i<temp_index.size(); i++){
                word_num_list += "," + to_string(temp_index[i].word_num);
                url_list += "," + to_string(temp_index[i].url);
            }
            word_num_list += "\"";
            url_list += "\"";
            outFile << temp_index[0].word_hash << "," << word_num_list << "," << url_list << endl;
            temp_index.clear();
        }
        temp_index.push_back(WordInfo(now[LoserTree[0]].word_hash, now[LoserTree[0]].word_num, now[LoserTree[0]].url));

        GetNext(files[LoserTree[0]], now, LoserTree[0]);
        Adjust(LoserTree, now, LoserTree[0], file_num);
    }

    outFile.close();
    for (int i=0; i<file_num; i++)  files[i].close();

    t_end = clock();
    cout << "运行时间"<< (double)(t_end-t_start)/CLOCKS_PER_SEC << "s" <<endl;
    cout << "————————————————————————————处理完成————————————————————————————" << endl;

    return 0;
}   