import scrapy
import json
import os
import csv


def isChinese(word):
    for ch in word:
        if '\u4e00' <= ch <= '\u9fff':
            return True
    return False

class NewsSpider(scrapy.Spider):
    name = "news"

    start_urls = []
    for dirname,_,filenames in os.walk('../../urls/'):
        for filename in filenames:
            # print(os.path.join(dirname,filename))
            with open(os.path.join(dirname,filename), 'r', encoding="utf_8_sig") as f:
                data = json.load(f)
                start_urls.extend(data['urls'])
    print(len(start_urls), start_urls)          

    with open("../../data/news.csv", 'r+', encoding="utf_8_sig", newline='') as f:  # 清空文件
        f.truncate()

    def parse(self, response):
        path  = "../../data/news.csv"
        for news in response.css('div.main'):
            if news.css('h1::text').get() is not None:
                text = ','.join(news.css('p::text').getall()).replace("\n", ",").replace("\t", ",").replace("\u3000", ",").replace("\xa0", ",")
                for c in text:
                    if not isChinese(c) and c != ',' and not c.isdigit() and c != '.' and c != '%':
                        text = text.replace(c, ",")
                title = news.css('h1::text').get()
                for c in title:
                    if not isChinese(c) and c != ',' and not c.isdigit() and c != '.' and c != '%':
                        title = title.replace(c, ",")
                data = {
                    'url': response.url,
                    'title': title,
                    'news_content': text,
                }
                with open(path, 'a+', encoding="utf_8_sig", newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow([data['url'], data['title'], data['news_content'].split("人民日报违法和不良信息举报电话")[0]])
                yield data
        # anchors_a = response.css('div.relevant_news a')
        # yield from response.follow_all(anchors_a, callback=self.parse)
