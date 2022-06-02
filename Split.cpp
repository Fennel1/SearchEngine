# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <sstream>
# include <time.h>
using namespace std;

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

void Merge(vector<WordInfo> &mylist, int low, int high, int mid)
{
    int i=low, j=mid+1, k=0, len=high-low+1;
    vector<WordInfo> templist(len, WordInfo(0, 0, 0));

    while (i<=mid && j<=high){ 
        if (mylist[i].word_hash <= mylist[j].word_hash){
            templist[k].word_hash = mylist[i].word_hash;
            templist[k].word_num = mylist[i].word_num;
            templist[k].url = mylist[i].url;
            k++;
            i++;
        }
        else{
            templist[k].word_hash = mylist[j].word_hash;
            templist[k].word_num = mylist[j].word_num;
            templist[k].url = mylist[j].url;
            k++;
            j++;
        }
    }
    while(i <= mid){
        templist[k].word_hash = mylist[i].word_hash;
        templist[k].word_num = mylist[i].word_num;
        templist[k].url = mylist[i].url;
        k++;
        i++;
    }
    while(j <= high) {
        templist[k].word_hash = mylist[j].word_hash;
        templist[k].word_num = mylist[j].word_num;
        templist[k].url = mylist[j].url;
        k++;
        j++;
    }

    for (int i=0; i<len; i++)  mylist[low+i] = templist[i];
}

void MergeSort(vector<WordInfo> &mylist, int low, int high)
{
    if (low < high)
    {
        int mid = (low+high)/2;
        MergeSort(mylist, low, mid);
        MergeSort(mylist, mid+1, high);
        Merge(mylist, low, high, mid);
    }
}

string load_path = "data/temp_index.csv";
string save_path = "data/";
vector<WordInfo> temp_index;          // 临时索引文件
clock_t t_start, t_end;

int main()
{
    t_start = clock();
    cout << "————————————————————————————开始处理————————————————————————————" << endl;

    ifstream inFile(load_path);
    string line;
    int count=0, file_index=0;
    while (getline(inFile,line)){
        for (unsigned int i=0; i<line.size(); i++){
            if (line[i] == ',') line[i] = ' ';
        }

        int word_hash=0, word_num=0, url_code=0;
        stringstream ss(line);
        ss >> word_hash >> word_num >> url_code;
        temp_index.push_back(WordInfo(word_hash, word_num, url_code));

        count++;
        if (count==10000){
            MergeSort(temp_index, 0, temp_index.size()-1);
            count=0;
            file_index++;
            ofstream outFile(save_path+to_string(file_index)+".csv");
            for (unsigned int i=0; i<temp_index.size(); i++){
                outFile << temp_index[i].word_hash << "," << temp_index[i].word_num << "," << temp_index[i].url << endl;
            }
            temp_index.clear();
        }
    }

    if (count>0){
        MergeSort(temp_index, 0, temp_index.size()-1);
        file_index++;
        ofstream outFile(save_path+to_string(file_index)+".csv");
        for (unsigned int i=0; i<temp_index.size(); i++){
            outFile << temp_index[i].word_hash << "," << temp_index[i].word_num << "," << temp_index[i].url << endl;
        }
        temp_index.clear();
    }

    t_end = clock();
    cout << "运行时间"<< (double)(t_end-t_start)/CLOCKS_PER_SEC << "s" <<endl;
    cout << "分割文件数:" << file_index << endl;
    cout << "————————————————————————————处理完成————————————————————————————" << endl;

    return 0;
}