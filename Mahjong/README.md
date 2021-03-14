
### これは何か
- 麻雀対戦実績データに基づき、真に強いプレーヤーが誰なのか、pystanにより推定を試みたものです
	- ./MahjongAnalysis.ipynb 分析用のノートブック
	- ./MahjongResults.csv 対戦実績データ

### なぜ分析したのか
- アカウント主（ME）は同僚（A~O）と定期的に麻雀対戦を行っており、その対戦結果をデータに残しています

![mahjong_log](http://drive.google.com/uc?export=view&id=1s5tA0EJCP72ncVaBuby_eE8KjaqwWpxh)
- 対戦実績を集計してプレーヤー毎の1位率を集計した結果、アカウント主は16名中2位の成績でした

![winning_rate](http://drive.google.com/uc?export=view&id=1c9FzXZPeGk7FAwAqq-BCoAbTTYR1jDL3)
- アカウント主（ME）の対戦回数は、常連の同僚（A, H, D）と比べて少ないものの、**強いプレーヤーとの対戦では1位を獲得できている**という自負があ離ました

![mahjong_log](http://drive.google.com/uc?export=view&id=1D88L_8JuuFx9Z-bwD7FVpQ9BjObtSPxT)
- **「対戦におけるプレーヤーの組み合わせ」と「対戦回数」を加味した上で、プレーヤーの強さを定量的に評価する術がないかと思い、本ポートフォリオで紹介する分析に至ります**

### どのように分析したのか
- 定量化の不確実性も考慮したいため、ベイズ統計を用いることにしました
- 以下のような仮定を設定しました
	- プレーヤー16名の強さ指標の平均値は0.25となる（1位獲得率に近い指標となるようにこのような制約を設定した）
	- 強さ指標w1, w2, w3, w4のプレーヤーが参加した試合で、プレーヤーw1が勝利する（1位になる）確率は、w1/(w1+w2+w3+w4)である
- 上記の仮定を表現したstanコード（以下）を実行することにより、プレーヤーの強さ指標を推定しました
```stan:model.stan
data {
  int N;//試合数
  int M;//参加者数
  int rank1[N];//試合nにおける1位のプレーヤー番号
  int rank2[N];//試合nにおける2位のプレーヤー番号
  int rank3[N];//試合nにおける3位のプレーヤー番号
  int rank4[N];//試合nにおける4位のプレーヤー番号
}

parameters {
  simplex[M] winning_seeds;//プレーヤーの強さ
}

transformed parameters {
  real winning_rate[M];//平均値が0.25になるように調整したプレーヤーの強さ
  for (m in 1:M) {
    winning_rate[m] = winning_seeds[m] * M / 4;
  }
}

model {
  //試合nに参加したプレーヤーの勝率期待値の算出（以下のように定義）
  // -> 対象プレーヤーの勝率期待値 = 対象プレーヤーの強さ/参加者4名の強さの合計値
  real winning_rate1_[N];//プレーヤー1の勝率期待値
  real winning_rate2_[N];//プレーヤー2の勝率期待値
  real winning_rate3_[N];//プレーヤー3の勝率期待値
  real winning_rate4_[N];//プレーヤー4の勝率期待値
  real winning_rate_sum[N];//試合nに参加したプレーヤーの強さの合計値
  for (i in 1:N) {
    winning_rate_sum[i] = winning_rate[rank1[i]]+winning_rate[rank2[i]]+winning_rate[rank3[i]]+winning_rate[rank4[i]];
    winning_rate1_[i] = winning_rate[rank1[i]]/winning_rate_sum[i];
    winning_rate2_[i] = winning_rate[rank2[i]]/winning_rate_sum[i];
    winning_rate3_[i] = winning_rate[rank3[i]]/winning_rate_sum[i];
    winning_rate4_[i] = winning_rate[rank4[i]]/winning_rate_sum[i];
    
    //確率過程
    1 ~ bernoulli(winning_rate1_[i]);
    0 ~ bernoulli(winning_rate2_[i]);
    0 ~ bernoulli(winning_rate3_[i]);
    0 ~ bernoulli(winning_rate4_[i]);
  }
}
```

### どのような結果が得られたのか
- 50パーセンタイル点（オレンジの線）に着目すると、僅かながら**実績2位のアカウント主（ME）が、実績1位の同僚Aよりも優れている**、という結果が得られました
- また、50パーセンタイル点の16プレーヤーの大小関係は、単純集計の大小関係とは異なっており、対戦相手の実力が考慮されているようにも見受けられます
	- 例えば、単純集計の1位率はM>Dだが、stanによる推定の50パーセンタイル点ではD>Mと順位が逆転しています
- しかし、確率分布として見ると、**非常に裾野の広い分布として推定されており、同僚Aのみならず多くの同僚の強さと比較しても有意な差があると言えない**ようです。アカウント主が「俺、強え」と感じていたのは、思い上がりのようでした

![mahjong_log](http://drive.google.com/uc?export=view&id=1Z_P0y6LaXrGBqmapysFgDvVAttlKok2m)

### 終わりに
- 「1位を取れるかどうか」という観点のみでプレーヤーの強さを評価した場合、高々156試合こなした程度では、有意な差は得られないようでした
- 「1位を取れるかどうか」だけが麻雀の強さではないので、データ数を増やさずとも、強さ指標の定義を見直すことで、より明確な差がつくような結果が得られるかもしれません
<!--stackedit_data:
eyJoaXN0b3J5IjpbMjI0ODIwMzU2LC0xNDEyNTA3Mjg4LC00Mj
IyNTc5MzUsMjkyNjI4NDc4LDczMDk5ODExNl19
-->