### これは何か
- Twitterでフォローしているアカウントが、どんな時間・曜日にどんなトピックを呟いているのか、トピック分析モデルの1つであるLDAを使って分析したものです

### どのように分析したか
- 実施したのは以下の内容です
	- Twitter APIによるデータ取得
	- MeCabによる形態素解析
	- gensim (LDA)によるトピックモデル構築
	- トピック毎の出現傾向の可視化


#### Twitter APIによるデータ取得：
- フォローしている400名の過去200件のツイートについて、以下の項目を取得しています
	- user_id（ツイート主のid）
	- created_at（ツイート日時）
	- tweet id（ツイートid）
	- text（ツイート文章）
	- followers_count（ツイート主のフォロワー数）
	- friends_count（ツイート主のフォロー数）
	- retweet_count（ツイートがリツイートされた数）
	- favorite_count（ツイートがいいねされた数）、など
- 今回紹介する分析では、created_at（ツイート日時）、text（ツイート文章）があれば十分です。アカウント主は、今回の分析を発展させて、「どんな時間にどんなトピックを投稿すると、よりいいねやリツイートなどの反応をもらいやすいのか」、という分析を行いたいと思ったので、その他の項目も取得しています

#### MeCabによる形態素解析：
- MeCabを使ってtextを単語の集合に変換します

```python
import MeCab
from gensim.corpora.dictionary import Dictionary
from gensim.models import LdaModel
from collections import defaultdict
from gensim import corpora

# MeCabオブジェクトの生成
mt = MeCab.Tagger('')
mt.parse('')

# 形態素解析
import re
def format_text(text):
    '''
    MeCabに入れる前のツイートの整形方法例
    '''

    text=re.sub(r'https?://[\w/:%#\$&\?\(\)~\.=\+\-…]+', "", text)
    text=re.sub('RT', "", text)
    text=re.sub('お気に入り', "", text)
    text=re.sub('まとめ', "", text)
    text=re.sub(r'[!-~]', "", text)#半角記号,数字,英字
    text=re.sub(r'[︰-＠]', "", text)#全角記号
    text=re.sub('\n', " ", text)#改行文字

    return text

text_list=list(df['t_text']) #df['t_text']はツイート文章が並んだSeries型のオブジェクト
dictionary =corpora.Dictionary([text_list])
train_texts = []
for line in text_list:
    text = []
    line = format_text(line)
    node = mt.parseToNode(line.strip())
    while node:
        fields = node.feature.split(",")
        if fields[0] == '名詞' or fields[0] == '動詞' or fields[0] == '形容詞':
            text.append(node.surface)
        node = node.next
    train_texts.append(text)
```



#### gensim (LDA)によるトピックモデル構築：
- 前述の単語の集合データを使ってトピックモデルを構築します
```python
# トピック数の設定
NUM_TOPICS = 20

# ldaモデルの生成
dictionary = Dictionary(train_texts)

dictionary.filter_extremes(no_below=2,no_above=0.2)
dictionary.save_as_text('最終辞書.txt')

# 学習データでモデル生成
corpus = [dictionary.doc2bow(text) for text in train_texts]
lda = LdaModel(corpus=corpus, num_topics=NUM_TOPICS, id2word=dictionary)

# テストデータ（=学習データと同じもの）をモデルに掛ける
score_by_topic = defaultdict(int)
test_corpus = [dictionary.doc2bow(text) for text in train_texts]
```

#### トピック毎の出現傾向の可視化：
- 時間帯毎、曜日毎のトピックの出現状況を可視化して、どんな時間帯・曜日にどんなトピックが呟かれやすいのかを把握します。例えば、以下は時間帯毎のトピックの出現状況ですが、「Topic11: 動画で勉強するでございます」などのように特定の時間帯に出現する/しないトピックが見受けられます

![topic_by_hour](http://drive.google.com/uc?export=view&id=1kzmRN5o-bfkmXJDeUxGtMl-MjgY9ori4)

- なお、LDAでは分類したトピックの意味合いを自分で考察する必要がありますので、以下のようにトピック毎に代表される単語を抽出してトピックの名称を与えています

```python
# topi12: 積み上げ頑張る
i=12
topic_word_prob[i].head(10)
```
![topic_by_hour](http://drive.google.com/uc?export=view&id=18dYJkyVFd_emtql6LmKYqGlr1OC1NEws)

### 参考文献
- [Pythonで余計な文字列を削除する方法](https://hacknote.jp/archives/19937/)
- [B'zの歌詞をPythonと機械学習で分析してみた 〜LDA編〜](https://pira-nino.hatenablog.com/entry/2018/07/29/B%27z%E3%81%AE%E6%AD%8C%E8%A9%9E%E3%82%92Python%E3%81%A8%E6%A9%9F%E6%A2%B0%E5%AD%A6%E7%BF%92%E3%81%A7%E5%88%86%E6%9E%90%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F_%E3%80%9CLDA%E7%B7%A8%E3%80%9C)
- [【Python】トピックモデル（LDA）](https://qiita.com/kenta1984/items/b08d5caeed6ed9c8abf1)
- [Python3でMeCabを使う方法を現役エンジニアが解説【初心者向け】](https://techacademy.jp/magazine/24037)
- [Tweet Object(ツイートオブジェクト)の説明](https://syncer.jp/Web/API/Twitter/REST_API/Object/Tweet/)
- [python-twitterを利用して、pythonでTwitterを操作する。](https://qiita.com/ti-ginkgo/items/90914db1dc7e229c69c7)
- [Python3でTwitterAPIを使う方法をどのサイトよりも丁寧に解説する](https://cre8cre8.com/python/twitter-api.htm)
- [Googleが公開した自然言語処理の最新技術、BERTとは何者なのか](https://ai-scholar.tech/articles/text-mining/bert-ai-93)
- [BERTとは｜Googleが誇る自然言語処理モデルの特徴、仕組みを解説](https://ledge.ai/bert/)
<!--stackedit_data:
eyJoaXN0b3J5IjpbMTA5MjE3MjkxNiwyMzE0MDc5MzQsNzAwND
I5Mjk5LC01OTQ3NzQ5NjMsNzMwOTk4MTE2XX0=
-->