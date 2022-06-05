cd spider_news/spider_news
scrapy crawl news --nolog

cd ../../
python jieba_cut.py

copy bin\hash.exe hash.exe /Y
copy bin\Split.exe Split.exe /Y
copy bin\externalSort.exe externalSort.exe /Y

hash.exe
Split.exe
externalSort.exe

del hash.exe
del Split.exe
del externalSort.exe

PAUSE