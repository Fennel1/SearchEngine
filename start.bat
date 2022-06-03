cd spider_news/spider_news
scrapy crawl news

cd ../../
python jieba_cut.py

copy bin\hash.exe hash.exe /Y
copy bin\Split.exe Split.exe /Y
copy bin\externalSort.exe externalSort.exe /Y
copy bin\inquery.exe inquery.exe /Y

hash.exe
Split.exe
externalSort.exe
inquery.exe

del hash.exe
del Split.exe
del externalSort.exe
del inquery.exe