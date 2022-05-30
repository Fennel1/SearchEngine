import scrapy
import json
from datetime import date


class NewsSpider(scrapy.Spider):
    name = "urls"
    start_urls = [
        'http://www.people.cn/'
    ]

    def parse(self, response):
        today = date.today() # 获取日期
        urls = []

        lis = response.xpath("//ul/li")
        for li in lis: 
            url = li.xpath("a/@href").get() # 获取链接
            if isinstance(url, str) and f'{today.month:02d}{today.day:02d}' in url:
                urls.append(url)

        data = {
            'nums': len(urls),
            'urls': urls,
        }
        with open(f'../../urls/urls{today.month:02d}{today.day:02d}.json','w')as f: # 写入文件
            f.write(json.dumps(data))

        yield data # 将数据返回给管道