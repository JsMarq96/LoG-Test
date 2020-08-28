# LoG
This project is a small and quick implementation of the averaged Laplaccian of a Gaussian implementation in C++, using paralelism and SIMD optimization usign OpenMP.
## Why?
I have been experimenting with the LoG as a meassure of difference between two images, for an upcomming project,and I was not very happy with the speed of the Python implementation (OpenCV + Numpy).<br/>
So I tried to create a simple implementation of just what I needed, and the results speaks for themselves!,<br/>
(And also, I needed to put my mind on other things during exams...!)
## Benchmarks
```
Python Size: 180x300    Time: 29139,738
C++ Test without OpenMP (sequential)
  Img Size: 1x1         Time: 159.24
  Img Size: 100x100     Time: 187.29
  Img Size: 10000x10000 Time: 161.07
C++ Test with OpenMP (paralelized)
  Img Size: 1x1          Time: 44.53
  Img Size: 100x100      Time: 44.82
  Img Size: 10000x10000  Time: 49.26
  
(Note: times in microseconds)
```
In both implementations the time delta is very small, but even in the sequential version, the diference with python is quite significative.<br/>
In fairness to Python, I do not know what kind of optimization have been applied, and how many I am leaving out by misconfiguring it;
<br/>and it being a much more versbose and complex code (151+ lines of code vs 2 calls to OpenCV and Numpy); plus some extra perfomance hit for generalization
in the library. <br/>
This is a custom implementation, tailored to the single use case of obtaining the average LoG score of an image; but it still is a very fun experiment!
