# encoding=utf-8
import jieba
import csv


words_set = set()
words_list = list()

csvFile = open("news.csv", "r", encoding="utf_8_sig")
reader = csv.reader(csvFile)
for item in reader:
    if len(item) > 0:
        print(item[1])          # 新闻标题分词
        title = jieba.lcut_for_search(item[1])
        while ',' in title:
            title.remove(',')
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

        with open("news_words.csv", 'a+', encoding="utf_8_sig") as f:   # 写入分词结果
            writer = csv.writer(f)
            writer.writerow([item[0],title,words])
            # pass
csvFile.close()

print('num of words_list:',len(words_list))
print('num of words_set:',len(words_set))
