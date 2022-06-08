# encoding=utf-8
import jieba
import csv
import time
import RedBlackTree as rbt

load_path = "data/news.csv"
save_news_words_path = "data/news_words.csv"
save_url_code_path = "data/url_code.csv"

t_start = time.time()
print("————————————————————————————开始分词————————————————————————————")

with open(save_news_words_path, 'r+', encoding="utf_8_sig", newline='') as f:  # 清空文件
    f.truncate()
with open(save_url_code_path, 'r+', encoding="utf_8_sig", newline='') as f:  # 清空文件
    f.truncate()

words_set = rbt.RedBlackTree()
words_num = 0

csvFile = open(load_path, "r", encoding="utf_8_sig")
reader = csv.reader(csvFile)
for i, item in enumerate(reader):
    if len(item) > 0:
        # print(item[1])          # 新闻标题分词
        year = ""
        day = ""
        for j in range(len(item[0])):
            if item[0][j] == "/" and (item[0][j-4:j] <= "2022" and item[0][j-4:j] >= "2000"):
                year = item[0][j-4:j]
                day = item[0][j+1:j+5]
            else :
                continue
        with open(save_url_code_path, 'a+', encoding="GBK", newline='') as f:
            writer = csv.writer(f)
            writer.writerow([i, year, day, 0.0, item[0], item[1]])

        title = jieba.lcut_for_search(item[1])
        while ',' in title:
            title.remove(',')
        for c in title:         # 统计词数
            words_set.insert(rbt.Node(c))
            words_num += 1
        # print(title)
        title = ','.join(title)

        # print(item[2])          # 新闻内容分词
        words = jieba.lcut_for_search(item[2])
        while ',' in words:
            words.remove(',')
        for c in words:         # 统计词数
            words_set.insert(rbt.Node(c))
            words_num += 1
        words = ','.join(words)
        # print(words)

        with open(save_news_words_path, 'a+', encoding="GBK", newline='') as f:   # 写入分词结果
            writer = csv.writer(f)
            writer.writerow([item[0],title,words])
            # pass
        # if i == 241:
        #     break
csvFile.close()

t_end = time.time()
print("————————————————————————————分词结束————————————————————————————")
print("分词用时：", t_end - t_start, "s")
print('单词数(有重复):',words_num)
print('单词数(无重复):',len(words_set))
