from apiclient.discovery import build
from oauth2client.service_account import ServiceAccountCredentials
import json
import re
from matplotlib import pyplot as plt
import pandas as pd
import japanize_matplotlib
import datetime
import MyGoogleAnalyticsFunctions


class ExtractGoogleAnalyticsData():
  def __init__(self, params):
    # paramsの読み込み
    self.startDate = params['startDate']
    self.scopes = params['scopes']
    self.key_file_location = params['key_file_location']
    self.view_id = params['view_id']
    self.page_size = params['page_size']
    self.m_list = params['m_list']
    self.d_list = params['d_list']

    #集計軸
    self.freq = self.d_list[-1]

    # google analytics api用の変数名に変換
    self.metrics = [{'expression': 'ga:' + m} for m in self.m_list] 
    self.dimensions = [{'name': 'ga:' + d} for d in self.d_list]

  def initialize_analyticsreporting(self):
    '''Analytics Reporting API V4 service objectの初期化'''
    credentials = ServiceAccountCredentials.from_json_keyfile_name(self.key_file_location, self.scopes)

    # Build the service object.
    self.analytics = build('analyticsreporting', 'v4', credentials=credentials)
      
  def get_report(self):
      """指定した日付から本日までのデータを取得"""
      response = self.analytics.reports().batchGet(
          body={
              'reportRequests': [
                  {
                      'viewId': self.view_id,
                      'dateRanges': [{'startDate': self.startDate, 'endDate': 'today'}],
                      'metrics': self.metrics,
                      'pageSize': self.page_size,
                      'dimensions': self.dimensions
                  }]
          }
      ).execute()

      # DataFrame形式に変換
      df = pd.io.json.json_normalize(response['reports'][0]['data']['rows'])      
      for i, d in enumerate(self.d_list):
          df[d] = df['dimensions'].apply(lambda x: x[i])

      for i, m in enumerate(self.m_list):
          df[m] = df['metrics'].apply(lambda x: x[0]['values'][i])

      df.drop(columns=['dimensions', 'metrics'], inplace=True)

      def f(s):
        '''pagePathの名寄せのための関数、上位４桁がユニークなものを同一のページと見なす'''
        m = re.match(r'[0-9]+',  s[4:])
        try:
            return m.group()
        except:
            return -9999

      df = df.assign( # pagePathの名寄せ&データ型変換
          pagePath = lambda df: df.pagePath.apply(lambda s: f(s)).astype(int),
          pageviews = lambda df: df.pageviews.astype(int),
          TimeOnPage = lambda df: df.avgTimeOnPage.astype(float) * df.pageviews
      )

      # 'pagePath', 'pageTitle'の対応関係を整理
      df_path_titile = df.groupby(
          ['pagePath', 'pageTitle']
      ).agg( # 最終閲覧日
          maxdate = (self.freq, 'max')
      ).reset_index().sort_values( # pagePathに対して複数のpageTitleが紐付く場合は最新のタイトルを割り当て
          by='maxdate', ascending=False
      ).drop_duplicates(
          subset=['pagePath'], keep='first'
      ).pipe( # pagePathがイレギュラーのもの（主に固定ページ）は除外
          lambda df: df[(df.pagePath>0) * (df.pagePath<20000)]
      )

      # 名寄せ後のデータフレーム
      self.df = df.drop(columns=['pageTitle']).merge( #名寄せ用のデータフレームをインナージョイン
          right=df_path_titile, 
          on='pagePath', 
          how='inner'
      )

  def plot_data(self):
      '''日別とページ別の情報に集約してプロットする'''

      # 日別の集計
      df_date = self.df.groupby(self.freq).agg(
          TimeOnPage = ('TimeOnPage', lambda s: s.sum()/(60*60)),
          pageviews = ('pageviews', 'sum'),
          users = ('users', lambda s: s.astype(int).sum()),
          sessions = ('sessions', lambda s: s.astype(int).sum())
      ).assign(
          avgTimeOnPage = lambda df: df.TimeOnPage/df.pageviews * (60*60),
          pageviews_users = lambda df: df.pageviews/df.users
      ).reset_index()

      if self.freq=='date':
        df_date[self.freq] = pd.to_datetime(df_date[self.freq])
      else:
        df_date[self.freq] = df_date[self.freq].apply(lambda x: int(x))
        df_date = df_date.merge(
            right=pd.DataFrame({self.freq: [i for i in range(24)]}),
            how='right',
            on=self.freq
        ).fillna(0)

      df_date = df_date.set_index(self.freq)

      # 日別のグラフ
      plt.rcParams["font.size"] = 10
      plt.figure(figsize=(14, 10))
      ax = plt.subplot(4, 1, 1)
      df_date.TimeOnPage.cumsum().plot()
      ax.tick_params(labelbottom=False)
      plt.text(df_date.index.max(), df_date.TimeOnPage.cumsum().iloc[-1], int(df_date.TimeOnPage.cumsum().iloc[-1]*100)/100, ha='right', va='bottom')
      plt.ylabel('TimeOnPage累計(H)')
      plt.xlabel(None)

      ax = plt.subplot(4, 1, 2)
      df_date.TimeOnPage.plot()
      ax.tick_params(labelbottom=False)
      plt.text(df_date.index.max(), df_date.TimeOnPage.iloc[-1], int(df_date.TimeOnPage.iloc[-1]*100)/100, ha='right', va='bottom')
      plt.ylabel('TimeOnPage(H）')
      plt.xlabel(None)

      ax = plt.subplot(4, 1, 3)
      df_date.avgTimeOnPage.plot()
      ax.tick_params(labelbottom=False)
      plt.text(df_date.index.max(), df_date.avgTimeOnPage.iloc[-1], int(df_date.avgTimeOnPage.iloc[-1]*100)/100, ha='right', va='bottom')
      plt.ylabel('avgTimeOnPage(sec）')
      plt.xlabel(None)

      ax = plt.subplot(4, 1, 4)
      df_date.pageviews.plot()
      plt.text(df_date.index.max(), df_date.pageviews.iloc[-1], int(df_date.pageviews.iloc[-1]*100)/100, ha='right', va='bottom')
      plt.ylabel('pageviews')
      plt.xlabel(None)

      # ページ別の集計
      df_page = self.df.groupby(
          ['pagePath', 'pageTitle']
      ).agg(
          pageviews = ('pageviews', 'sum'),
          TimeOnPage = ('TimeOnPage', lambda s: s.sum()/60/60)
      ).reset_index().assign(
          pageTitle = lambda df: df.pageTitle,
          avgTimeOnPage = lambda df: df.TimeOnPage/df.pageviews * 60 *60
      ).assign(
          TimeOnPage = lambda df: (df.TimeOnPage*100).astype(int)/100,
          avgTimeOnPage = lambda df: (df.avgTimeOnPage*100).astype(int)/100,
          pageTitle = lambda df: df.pageTitle.apply(
                                                          lambda x: (x[:-12]+'     ')[:8]+'***'+x[-17:-12])
      ).sort_values(by='TimeOnPage', ascending=True)

      # ページ別のグラフ
      plt.rcParams["font.size"] = 10
      plt.figure(figsize=(10, (len(df_page)+0.)*0.25))
      plt.subplot(1, 3, 1)
      plt.barh(df_page.pageTitle, df_page.TimeOnPage)
      plt.title('TimeOnPage(H)')
      for x, y in zip(df_page.TimeOnPage, df_page.pageTitle):
          plt.text(x, y, x, ha='left', va='center')
          
      ax = plt.subplot(1, 3, 2)
      plt.barh(df_page.pageTitle, df_page.avgTimeOnPage)
      plt.title('avgTimeOnPage(sec)')
      ax.tick_params(labelleft=False)
      for x, y in zip(df_page.avgTimeOnPage, df_page.pageTitle):
          plt.text(x, y, x, ha='left', va='center')
          
      ax = plt.subplot(1, 3, 3)
      plt.barh(df_page.pageTitle, df_page.pageviews)
      plt.title('pageviews')
      ax.tick_params(labelleft=False)
      for x, y in zip(df_page.pageviews, df_page.pageTitle):
          plt.text(x, y, x, ha='left', va='center')