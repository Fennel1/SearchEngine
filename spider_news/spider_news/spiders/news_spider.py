import scrapy
import json
import os
import csv


class NewsSpider(scrapy.Spider):
    name = "news"

    start_urls = []
    for dirname,_,filenames in os.walk('../../urls/'):
        for filename in filenames:
            # print(os.path.join(dirname,filename))
            with open(os.path.join(dirname,filename), 'r', encoding="utf_8_sig") as f:
                data = json.load(f)
                start_urls.extend(data['urls'])
    print(start_urls)           

    def parse(self, response):
        path  = "../../news.csv"
        for news in response.css('div.main'):
            if news.css('h1::text').get() is not None:
                data = {
                    'url': response.url,
                    'title': news.css('h1::text').get(),
                    'news_content': ' '.join(news.css('p::text').getall()).replace("\n", "").replace("\t", "").replace("\u3000", "").replace("\xa0", ""),
                }
                with open(path, 'a+', encoding="utf_8_sig") as f:
                    writer = csv.writer(f)
                    writer.writerow([data['url'], data['title'], data['news_content'].split("人民日报违法和不良信息举报电话")[0]])
                yield data
        # anchors_a = response.css('div.relevant_news a')
        # yield from response.follow_all(anchors_a, callback=self.parse)
