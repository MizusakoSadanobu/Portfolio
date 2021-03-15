# 基本ライブラリ
import numpy as np
import numpy.random as random
import scipy as sp
from pandas import Series,DataFrame
import pandas as pd
import time

# 可視化モジュール
import matplotlib.pyplot as plt
import matplotlib as mpl
#import seaborn as sns
import japanize_matplotlib
#%matplotlib inline

# 機械学習モジュール
import sklearn

# その他
from datetime import datetime
import matplotlib.dates as mdates
import os
import json
import time

# Twitter API
import twitter

# 自作モジュール
import config

# 共通で使う関数・変数の定義
## 関数
def timestamp2str(dt):
    '''pandas datetimeを年月日に変換する'''
    return str(dt.year)+'-'+str(dt.month)+'-'+str(dt.day)

## 変数
date_today = pd.to_datetime(datetime.today()) - pd.Timedelta(hours = 24) #基準日は集計日の1日前とする
str_today = timestamp2str(date_today)
api = twitter.Api(
    consumer_key=config.CONSUMER_KEY,
    consumer_secret=config.CONSUMER_SECRET,
    access_token_key=config.ACCESS_TOKEN,
    access_token_secret=config.ACCESS_TOKEN_SECRET)

print('api initialized')
