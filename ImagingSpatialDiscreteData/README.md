### これは何か
- 以下のように、離散的な時空間データ$u(x^{obs_i}, t)$を補間して連続的な時空間データ$u(x, t)$を再現するプログラムです

【離散的な時空間データ$u(x^{obs_i}, t)$】

![obs](https://drive.google.com/uc?export=view&id=1-HKFmW-ZCkR-qgX6Bh2pZmsZRuQd0U-9)

↓

【連続的な時空間データ$u(x, t)$】

![obs](https://drive.google.com/uc?export=view&id=1FmbVsEbANJkaG-doY8l5C0EFcp4FIhXr)

- 本プログラムは、以下のサブプログラムから構成されています
  - [./ImagingDiscreteData.ipynb](https://github.com/MizusakoSadanobu/Portfolio/blob/master/ImagingSpatialDiscreteData/ImagingSpatialDiscreteData.ipynb): 結果の可視化用ノートブック
  - [./main.cpp](https://github.com/MizusakoSadanobu/Portfolio/tree/master/ImagingDiscreteData/main.cpp): $u(x^{obs_i}, t)$ →$u(x, t)$を実行するプログラム
    - [./lib/Basic_include_file_set.h](https://github.com/MizusakoSadanobu/Portfolio/tree/master/ImagingSpatialDiscreteData/lib/Basic_include_file_set.h): 共通で利用するヘッダーファイルを定義
    - [./lib/basic.h(pp)](https://github.com/MizusakoSadanobu/Portfolio/tree/master/ImagingSpatialDiscreteData/lib/basic.hpp): ファイルの読み書きなどの基本操作を定義
    - [./lib/matrix_algebra.h(pp)](https://github.com/MizusakoSadanobu/Portfolio/tree/master/ImagingSpatialDiscreteData/lib/matrix_algebra.hpp): 行列・テンソル操作を定義
    - [./lib/fft.h(pp)](https://github.com/MizusakoSadanobu/Portfolio/tree/master/ImagingSpatialDiscreteData/lib/fft.hpp): フーリエ変換関数を定義
    - [./lib/method.h(pp)](https://github.com/MizusakoSadanobu/Portfolio/tree/master/ImagingSpatialDiscreteData/lib/method.hpp): 機械学習モデル（マルチアウトプットOLS/LASSO）を定義
    - [./lib/cluster.h(pp)](https://github.com/MizusakoSadanobu/Portfolio/tree/master/ImagingSpatialDiscreteData/lib/cluster.hpp): 機械学習モデル構築に適した形に観測データを処理するクラスを定義
    - [./lib/element.h(pp)](https://github.com/MizusakoSadanobu/Portfolio/tree/master/ImagingSpatialDiscreteData/lib/element.hpp): 空間分布の表現に適した形に機械学習の予測結果を処理するクラスを定義

- 連続的な時空間分布を得るまでのデータ処理過程とサブプログラムは以下ように対応しています

![obs](https://drive.google.com/uc?export=view&id=1h0C32qgkFOH6xkBI6zioqVUNsAcQcQtj)
<!--stackedit_data:
eyJoaXN0b3J5IjpbMjA0ODI2NDk0MCw3ODI2NDc3MDUsMTMwMj
UwOTA0NCwxMjMyMDUzMDAsNzMwOTk4MTE2XX0=
-->