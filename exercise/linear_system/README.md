# 連立一次方程式の解法

## ガウスの消去法

- gauss.c : ガウスの消去法(ピボット選択なし)
- input1.dat : 入力サンプル1
- input2.dat : 入力サンプル2 (pivot選択が必要な場合)
- input3.dat : 入力サンプル3 (Aが正定値対称行列の場合)

## LU分解

- lu_decomp.c : LAPACKを使ってLU分解を行う例
- determinant.c : LU分解を使った行列式の計算

## 最急降下法

- steepest_descent.c : 最急降下法を用いた連立一次方程式の求解

## Poisson方程式

- poisson.h : 二次元Poisson方程式の行列表示
- poisson_dense.c : Poisson方程式の行列の確認

  実行例) ./poisson_dense 4

- poisson_sparse.c : Poisson方程式の行列の疎行列表現の確認

  実行例) ./poisson_sparse 4

- poisson_lu.c : Poisson方程式のLU分解による求解

  実行例) ./poisson_lu 4
