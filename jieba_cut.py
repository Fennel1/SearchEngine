# encoding=utf-8
import jieba
import csv

load_path = "data/news.csv"
save_path = "data/news_words.csv"

words_set = set()
words_list = list()

csvFile = open(load_path, "r", encoding="utf_8_sig")
reader = csv.reader(csvFile)
for item in reader:
    if len(item) > 0:
        print(item[1])          # 新闻标题分词
        title = jieba.lcut_for_search(item[1])
        while ',' in title:
            title.remove(',')
        for c in title:         # 统计词数
            words_set.add(c)
            words_list.append(c)
        print(title)
        title = ','.join(title)

        print(item[2])          # 新闻内容分词
        words = jieba.lcut_for_search(item[2])
        while ',' in words:
            words.remove(',')
        for c in words:         # 统计词数
            words_set.add(c)
            words_list.append(c)
        words = ','.join(words)
        print(words)

        with open(save_path, 'a+', encoding="GBK", newline='') as f:   # 写入分词结果
            writer = csv.writer(f)
            writer.writerow([item[0],title,words])
            # pass
        # break
csvFile.close()

print('num of words_list:',len(words_list))
print('num of words_set:',len(words_set))
