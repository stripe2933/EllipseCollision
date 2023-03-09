# EllipseCollision
Detect if two arbitrary rotated/scaled/translated ellipses are colliding. Interactive simulation available using SFML.

## Sample
<img width="752" alt="Screenshot 2023-03-09 at 1 49 24 AM" src="https://user-images.githubusercontent.com/63503910/223776777-5e599eb4-2612-4e37-af07-2f8ebc024457.png">

The colliding ellipses are marked with red boundary.

## Build

Clone project:
```bash
git clone https://github.com/stripe2933/EllipseCollision.git
cd EllipseCollision
```

Build using CMake (SFML 3, Eigen 3, and fmt required.)
```bash
mkdir build
cd build
cmake ..
cd ../
cmake --build build #! [--config release] for release build.
```

Run executable
```bash
./build/EllipseCollision
```

## How does it works?

You can read [the answer of Nick Alger](https://math.stackexchange.com/questions/1114879/detect-if-two-ellipses-intersect), or [the original paper](https://ieeexplore.ieee.org/document/6289830) to get the details.

Let two ellipses $E_A$ and $E_B$ that centered at $\mathbf a$ and $\mathbf b$ are representable as the quadratic form as below:

$$
\begin{aligned}
E_A &: (\mathbf x-\mathbf a)^T A (\mathbf x-\mathbf a) \le 1 \\
E_B &: (\mathbf x-\mathbf b)^T B (\mathbf x-\mathbf b) \le 1 
\end{aligned}
$$

where $A$ and $B$ are 2-by-2 matrix. The eigenvector of $A$ are the unit vectors of each semi major/minor axis, and the inverse square roots of the corresponding eigenvalues ($1\sqrt{\lambda}$) are the lengths of each axes. Same holds for $B$.

Now define the following convex scalar function $K:(0,1)\to \mathbb R$,

$$
K(s)=1-(\mathbf b-\mathbf a)^T \left({1\over 1-s} A^{-1}+{1\over s}B^{-1}\right)^{-1}(\mathbf b-\mathbf a)
$$

If the global minimum of this function is nonnegative, then two ellipses are colliding. Note that it also holds if one ellipse contains the whole another ellipse so that each boundaries are not met.

Since the definition of $K(s)$ contains matrix operation, we can optimize it by precompute the eigenvalues and eigenvectors. Let `a`, `b` are two `Eigen::Vector2f` correspond to ellipses' centers and `A`, `B` are two `Eigen::Matrix2f` correspond to each quadratic form matrices. Then the $K(s)$ calculation would be

```c++
Eigen::GeneralizedSelfAdjointEigenSolver<Eigen::Matrix2f> es { A.inverse(), B.inverse() };
auto lambdas = es.eigenvalues();
auto phi = es.eigenvectors();
auto v_squared = (phi.transpose() * (a - b)).array().square();

auto k_function = [&](float s){
    return 1.f - (v_squared * (s * (1.f - s) / (1.f + s * (lambdas.array() - 1.f)))).sum();
};

auto [_, min_k] = optimize(0.5f, k_function);
bool is_collide = min_k >= 0.f;
```

The function `optimize` find the global minimum of convex-downwarded function using gradient descent method. The full code is in [here](https://github.com/stripe2933/EllipseCollision/blob/main/EllipseShape.cpp).
