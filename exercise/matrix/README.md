# C言語におけるポインタと行列の扱い

- cmatrix.h : ベクトルと行列の生成・開放・読み込み・書き出しのためのユーティリティ関数
- matrix.dat, vector.dat : 読み込みファイルの例
- matrix_example.c : cmatrix.h に含まれるユーティリティ関数の使用例

  実行例) ./matrix_example
  
- pointer.c : C言語におけるポインタの振る舞いを確認するためのテストプログラム

  実行例) ./pointer

## サンプル行列の生成

- random_matrix.c : ランダムな行列の生成例

  実行例) ./random_matrix
  
## 行列・行列積

- multiply.c : for文を使った実装例

  実行例) ./multiply

- multiply_dgemm.c : BLASライブラリのdgemm関数を使った実装

  実行例) ./multiply_dgemm
