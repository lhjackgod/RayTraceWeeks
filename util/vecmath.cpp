#include "vecmath.h"
template<int N>
std::optional<SquareMatrix<N>> Inverse(const SquareMatrix<N> &m)
{
    int indxc[N], indxr[N];
    int ipiv[N] = {0};
    float minv[N][N];
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            minv[i][j] = m[i][j];
    for (int i = 0; i < N; i++) {
        int irow = 0, icol = 0;
        float big = 0.f;
        // Choose pivot
        for (int j = 0; j < N; j++) {
            if (ipiv[j] != 1) {
                for (int k = 0; k < N; k++) {
                    if (ipiv[k] == 0) {
                        if (std::abs(minv[j][k]) >= big) {
                            big = std::abs(minv[j][k]);
                            irow = j;
                            icol = k;
                        }
                    } else if (ipiv[k] > 1)
                        return {};  // singular
                }
            }
        }
        ++ipiv[icol];
        // Swap rows _irow_ and _icol_ for pivot
        if (irow != icol) {
            for (int k = 0; k < N; ++k)
                pstd::swap(minv[irow][k], minv[icol][k]);
        }
        indxr[i] = irow;
        indxc[i] = icol;
        if (minv[icol][icol] == 0.f)
            return {};  // singular

        // Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
        float pivinv = 1. / minv[icol][icol];
        minv[icol][icol] = 1.;
        for (int j = 0; j < N; j++)
            minv[icol][j] *= pivinv;

        // Subtract this row from others to zero out their columns
        for (int j = 0; j < N; j++) {
            if (j != icol) {
                float save = minv[j][icol];
                minv[j][icol] = 0;
                for (int k = 0; k < N; k++)
                    minv[j][k] = FMA(-minv[icol][k], save, minv[j][k]);
            }
        }
    }
    // Swap columns to reflect permutation
    for (int j = N - 1; j >= 0; j--) {
        if (indxr[j] != indxc[j]) {
            for (int k = 0; k < N; k++)
                pstd::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
        }
    }
    return SquareMatrix<N>(minv);
}