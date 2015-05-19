# 連立一次方程式の解法

## C言語におけるポインタと行列の扱い

- matrix_util.h : ベクトルと行列の生成・開放・読み込み・書き出しのためのユーティリティ関数
- matrix_example.c : matrix_util.h に含まれるユーティリティ関数の使用例
- matrix.dat, vector.dat : 読み込みファイルの例
- pointer_vector.c : C言語におけるポインタの振る舞いを確認するためのテストプログラム(一次元配列)
- pointer_matrix.c : C言語におけるポインタの振る舞いを確認するためのテストプログラム(二次元配列)

## サンプル行列の生成

- mersenne_twister.h : Mersenne-Twister法による乱数生成関数
- random_matrix.c : ランダムな行列の生成例

## ガウスの消去法

- gauss.c : ガウスの消去法(ピボット選択なし)
- input1.dat : 入力サンプル1
- input2.dat : 入力サンプル2

## ラプラス方程式の境界値問題

- laplace-lu.c : LU分解による求解
- laplace-jacobi.c : ヤコビ法による求解(収束判定なし)
