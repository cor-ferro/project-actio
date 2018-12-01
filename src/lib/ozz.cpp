#include "ozz.h"

namespace libOzz {
    mat4 toNativeType(ozz::math::Float4x4& ozzMat) {
        mat4 mat;

        mat[0][0] = ozzMat.cols[0][0];
        mat[0][1] = ozzMat.cols[0][1];
        mat[0][2] = ozzMat.cols[0][2];
        mat[0][3] = ozzMat.cols[0][3];

        mat[1][0] = ozzMat.cols[1][0];
        mat[1][1] = ozzMat.cols[1][1];
        mat[1][2] = ozzMat.cols[1][2];
        mat[1][3] = ozzMat.cols[1][3];

        mat[2][0] = ozzMat.cols[2][0];
        mat[2][1] = ozzMat.cols[2][1];
        mat[2][2] = ozzMat.cols[2][2];
        mat[2][3] = ozzMat.cols[2][3];

        mat[3][0] = ozzMat.cols[3][0];
        mat[3][1] = ozzMat.cols[3][1];
        mat[3][2] = ozzMat.cols[3][2];
        mat[3][3] = ozzMat.cols[3][3];

        return mat;
    }
}