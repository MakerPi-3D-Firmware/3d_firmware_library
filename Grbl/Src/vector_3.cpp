/*
  vector_3.cpp - Vector library for bed leveling
  Copyright (c) 2012 Lars Brubaker.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "user_common_cpp.h"

#if defined(ENABLE_ARM_MATH) and defined(__USE_C99_MATH)
float square(float x)
{
  float result = 0.0f;
  arm_power_f32(&x, 2, &result);
  return result;
}
#else
#define square(x) powf(x, 2)
#endif

namespace sg_grbl
{


#ifdef __cplusplus
  extern "C" {
#endif

  vector_3::vector_3()
  {
    this->x = 0;
    this->y = 0;
    this->z = 0;
  }

  vector_3::vector_3(float x, float y, float z)
  {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  // 两向量叉乘，得出垂直于两向量平面的向量
  vector_3 vector_3::cross(vector_3 left, vector_3 right)
  {
    return vector_3(left.y * right.z - left.z * right.y,
                    left.z * right.x - left.x * right.z,
                    left.x * right.y - left.y * right.x);
  }

  vector_3 vector_3::operator+(vector_3 v)
  {
    return vector_3((x + v.x), (y + v.y), (z + v.z));
  }

  vector_3 vector_3::operator-(vector_3 v)
  {
    return vector_3((x - v.x), (y - v.y), (z - v.z));
  }

  vector_3 vector_3::operator*(float v)
  {
    return vector_3((x * v), (y * v), (z * v));
  }

  vector_3 vector_3::get_normal()
  {
    vector_3 normalized = vector_3(x, y, z);
    normalized.normalize();
    return normalized;
  }

  float vector_3::get_length()
  {
    float input_data = (x * x) + (y * y) + (z * z);
    float length = 0.0f;
#if defined(ENABLE_ARM_MATH) and defined(__USE_C99_MATH)
    arm_sqrt_f32(input_data, &length);
#else
    length = sqrtf(input_data);
#endif
    return length;
  }

  void vector_3::normalize()
  {
    float length = get_length();
    x /= length;
    y /= length;
    z /= length;
  }

  void vector_3::apply_rotation(matrix_3x3 matrix)
  {
    float resultX = x * matrix.matrix[3 * 0 + 0] + y * matrix.matrix[3 * 1 + 0] + z * matrix.matrix[3 * 2 + 0];
    float resultY = x * matrix.matrix[3 * 0 + 1] + y * matrix.matrix[3 * 1 + 1] + z * matrix.matrix[3 * 2 + 1];
    float resultZ = x * matrix.matrix[3 * 0 + 2] + y * matrix.matrix[3 * 1 + 2] + z * matrix.matrix[3 * 2 + 2];
    x = resultX;
    y = resultY;
    z = resultZ;
  }

  float vector_3::triangle_area(vector_3 A, vector_3 B, vector_3 C)//海伦公式
  {
    float AB, BC, AC, P;
#if defined(ENABLE_ARM_MATH) and defined(__USE_C99_MATH)
    float length = 0.0f, result = 0.0f;
    length = square(B.x - A.x) + square(B.y - A.y);
    arm_sqrt_f32(length, &AB);
    length = square(C.x - A.x) + square(C.y - A.y);
    arm_sqrt_f32(length, &AC);
    length = square(C.x - B.x) + square(C.y - B.y);
    arm_sqrt_f32(length, &BC);
    P = (AB + AC + BC) / 2;
    length = P * (P - AB) * (P - AC) * (P - BC);
    arm_sqrt_f32(length, &result);
    return result;
#else
    AB = sqrtf(square(B.x - A.x) + square(B.y - A.y));
    AC = sqrtf(square(C.x - A.x) + square(C.y - A.y));
    BC = sqrtf(square(C.x - B.x) + square(C.y - B.y));
    P = (AB + AC + BC) / 2;
    return sqrtf(P * (P - AB) * (P - AC) * (P - BC));
#endif
  }

  bool vector_3::is_in_triangle(vector_3 A, vector_3 B, vector_3 C)
  {
    const float EPSINON = 0.5F; //0.0001f
    float S1, S2, S3, Ssum;
    vector_3 curr_pos = vector_3(x, y, z);
    S1 = triangle_area(A, B, curr_pos);
    S2 = triangle_area(A, C, curr_pos);
    S3 = triangle_area(B, C, curr_pos);
    Ssum = triangle_area(A, B, C);

    if (EPSINON > fabs(Ssum - S1 - S2 - S3))//注意绝对值
    {
      //    if (fabs(S1) < EPSINON || fabs(S2) < EPSINON || fabs(S3) < EPSINON)
      //    {
      //      USER_EchoLog("The point is on the triangle.");
      //    }
      //    else
      //    {
      //      USER_EchoLog("The point is in the triangle.");
      //    }
      return true;
    }
    else
    {
      //    USER_EchoLog("The point is out the triangle.");
      return false;
    }
  }

  vector_3 vector_3::get_plane_normal(vector_3 A, vector_3 B, vector_3 C)
  {
    vector_3 planeNormal = vector_3::cross(A - C, B - C).get_normal();

    if (planeNormal.z < 0) planeNormal = planeNormal * -1.0f;

    return planeNormal;
  }

  matrix_3x3 matrix_3x3::create_from_rows(vector_3 row_0, vector_3 row_1, vector_3 row_2)
  {
    matrix_3x3 new_matrix;
    new_matrix.matrix[0] = row_0.x;
    new_matrix.matrix[1] = row_0.y;
    new_matrix.matrix[2] = row_0.z;
    new_matrix.matrix[3] = row_1.x;
    new_matrix.matrix[4] = row_1.y;
    new_matrix.matrix[5] = row_1.z;
    new_matrix.matrix[6] = row_2.x;
    new_matrix.matrix[7] = row_2.y;
    new_matrix.matrix[8] = row_2.z;
    return new_matrix;
  }

  void matrix_3x3::set_to_identity()
  {
    matrix[0] = 1;
    matrix[1] = 0;
    matrix[2] = 0;
    matrix[3] = 0;
    matrix[4] = 1;
    matrix[5] = 0;
    matrix[6] = 0;
    matrix[7] = 0;
    matrix[8] = 1;
  }

  matrix_3x3 matrix_3x3::transpose(matrix_3x3 original)
  {
#if defined(ENABLE_ARM_MATH) and defined(__USE_C99_MATH)
    arm_status sta;
    /****浮点数数组******************************************************************/
    float32_t pDataDst[9];
    arm_matrix_instance_f32 pSrc; //6行6列数据
    arm_matrix_instance_f32 pDst; //6行6列数据;
    /****浮点数***********************************************************************/
    pSrc.numCols = 3;
    pSrc.numRows = 3;
    pSrc.pData = original.matrix;
    pDst.numCols = 3;
    pDst.numRows = 3;
    pDst.pData = pDataDst;
    sta = arm_mat_trans_f32(&pSrc, &pDst);
    /*
       sta = ARM_MATH_SUCCESS, 即返回0，表示求逆矩阵成功。
       sta = ARM_MATH_SINGULAR, 即返回-5，表示求逆矩阵失败，也表示不可逆。
       注意，ARM提供的DSP库逆矩阵求发有局限性，通过Matlab验证是可以求逆矩阵的，而DSP库却不能正确求解。

    */
    printf("transpose ----sta %d\r\n", sta);
    matrix_3x3 _transpose;
    memmove(_transpose.matrix, pDataDst, sizeof(float) * 9);
    return _transpose;
#else
    matrix_3x3 new_matrix;
    new_matrix.matrix[0] = original.matrix[0];
    new_matrix.matrix[1] = original.matrix[3];
    new_matrix.matrix[2] = original.matrix[6];
    new_matrix.matrix[3] = original.matrix[1];
    new_matrix.matrix[4] = original.matrix[4];
    new_matrix.matrix[5] = original.matrix[7];
    new_matrix.matrix[6] = original.matrix[2];
    new_matrix.matrix[7] = original.matrix[5];
    new_matrix.matrix[8] = original.matrix[8];
    return new_matrix;
#endif
  }

  // Create a matrix rotated to point towards a target
  matrix_3x3 matrix_3x3::create_look_at(vector_3 &target)
  {
    const vector_3 z_row = target.get_normal(),
                   x_row = vector_3(1, 0, -target.x / target.z).get_normal(),
                   y_row = vector_3::cross(z_row, x_row).get_normal();
    // x_row.debug(PSTR("x_row"));
    // y_row.debug(PSTR("y_row"));
    // z_row.debug(PSTR("z_row"));
    // create the matrix already correctly transposed
    matrix_3x3 rot = matrix_3x3::create_from_rows(x_row, y_row, z_row);
    // rot.debug(PSTR("rot"));
    return rot;
  }

  matrix_3x3 matrix_3x3::create_look_at(vector_3 target, vector_3 up)
  {
    // There are lots of examples of look at code on the internet that don't do all these noramize and also find the position
    // through several dot products.  The problem with them is that they have a bit of error in that all the vectors arn't normal and need to be.
    vector_3 z_row = /*vector_3(-target.x, -target.y, -target.z)*/target.get_normal();
    vector_3 x_row = /*vector_3(1, 0, -target.x / target.z).get_normal();*/vector_3::cross(up, z_row).get_normal();
    vector_3 y_row = vector_3::cross(z_row, x_row).get_normal();
    matrix_3x3 rot = matrix_3x3::create_from_rows(vector_3(x_row.x, y_row.x, z_row.x),
                     vector_3(x_row.y, y_row.y, z_row.y),
                     vector_3(x_row.z, y_row.z, z_row.z));
    return rot;
  }

  //  矩阵A
  //  |a11 a12 a13|
  //  |a21 a22 a23|
  //  |a31 a32 a33|
  //
  //  伴随矩阵A*
  //  |A11 A12 A13|
  //  |A21 A22 A23|
  //  |A31 A32 A33|
  //
  //  行列式|A|
  //  a11*(a22 * A33 - a32 * a23)-a12*(a21 * a33 - a31 * a23)+a13*(a21 * a32 - a22 * a31)
  //
  //  A的逆矩阵A^-1 = A* / |A|
  //
  //  代数余子式 Mij
  //  Aij = (-1) ^ (i + j) * Mij
  //  A11 = (-1) ^ 2 * (a22 * a33 - a23 * a32)
  //  A12 = (-1) ^ 3 * (a21 * a33 - a31 * a23)
  //  A13 = (-1) ^ 4 * (a21 * a32 - a22 * a31)

  //  A21 = (-1) ^ 3 * (a12 * a33 - a13 * a32)
  //  A22 = (-1) ^ 4 * (a11 * a33 - a13 * a31)
  //  A23 = (-1) ^ 5 * (a11 * a32 - a12 * a31)

  //  A31 = (-1) ^ 4 * (a12 * a23 - a13 * a22)
  //  A32 = (-1) ^ 5 * (a11 * a23 - a13 * a21)
  //  A33 = (-1) ^ 6 * (a22 * a11 - a21 * a12)

  matrix_3x3 matrix_3x3::create_inverse(matrix_3x3 original)
  {
#if defined(ENABLE_ARM_MATH) and defined(__USE_C99_MATH)
    arm_status sta;
    /****浮点数数组******************************************************************/
    float32_t pDataDst[9];
    arm_matrix_instance_f32 pSrc; //6行6列数据
    arm_matrix_instance_f32 pDst; //6行6列数据;
    /****浮点数***********************************************************************/
    pSrc.numCols = 3;
    pSrc.numRows = 3;
    pSrc.pData = original.matrix;
    pDst.numCols = 3;
    pDst.numRows = 3;
    pDst.pData = pDataDst;
    sta = arm_mat_inverse_f32(&pSrc, &pDst);
    /*
        sta = ARM_MATH_SUCCESS, 即返回0，表示求逆矩阵成功。
        sta = ARM_MATH_SINGULAR, 即返回-5，表示求逆矩阵失败，也表示不可逆。
        注意，ARM提供的DSP库逆矩阵求发有局限性，通过Matlab验证是可以求逆矩阵的，而DSP库却不能正确求解。

    */
    printf("create_inverse ----sta %d\r\n", sta);
    matrix_3x3 inverse;
    memmove(inverse.matrix, pDataDst, sizeof(float) * 9);
    return inverse;
#else
    float *A = original.matrix;
    // 求行列式
    float determinant =
      + A[0 * 3 + 0] * (A[1 * 3 + 1] * A[2 * 3 + 2] - A[2 * 3 + 1] * A[1 * 3 + 2])
      - A[0 * 3 + 1] * (A[1 * 3 + 0] * A[2 * 3 + 2] - A[1 * 3 + 2] * A[2 * 3 + 0])
      + A[0 * 3 + 2] * (A[1 * 3 + 0] * A[2 * 3 + 1] - A[1 * 3 + 1] * A[2 * 3 + 0]);
    // 伴随矩阵，求逆矩阵
    matrix_3x3 inverse;
    inverse.matrix[0 * 3 + 0] = +(A[1 * 3 + 1] * A[2 * 3 + 2] - A[2 * 3 + 1] * A[1 * 3 + 2]) / determinant;
    inverse.matrix[0 * 3 + 1] = -(A[0 * 3 + 1] * A[2 * 3 + 2] - A[0 * 3 + 2] * A[2 * 3 + 1]) / determinant;
    inverse.matrix[0 * 3 + 2] = +(A[0 * 3 + 1] * A[1 * 3 + 2] - A[0 * 3 + 2] * A[1 * 3 + 1]) / determinant;
    inverse.matrix[1 * 3 + 0] = -(A[1 * 3 + 0] * A[2 * 3 + 2] - A[1 * 3 + 2] * A[2 * 3 + 0]) / determinant;
    inverse.matrix[1 * 3 + 1] = +(A[0 * 3 + 0] * A[2 * 3 + 2] - A[0 * 3 + 2] * A[2 * 3 + 0]) / determinant;
    inverse.matrix[1 * 3 + 2] = -(A[0 * 3 + 0] * A[1 * 3 + 2] - A[1 * 3 + 0] * A[0 * 3 + 2]) / determinant;
    inverse.matrix[2 * 3 + 0] = +(A[1 * 3 + 0] * A[2 * 3 + 1] - A[2 * 3 + 0] * A[1 * 3 + 1]) / determinant;
    inverse.matrix[2 * 3 + 1] = -(A[0 * 3 + 0] * A[2 * 3 + 1] - A[2 * 3 + 0] * A[0 * 3 + 1]) / determinant;
    inverse.matrix[2 * 3 + 2] = +(A[0 * 3 + 0] * A[1 * 3 + 1] - A[1 * 3 + 0] * A[0 * 3 + 1]) / determinant;
    vector_3 row0 = vector_3(inverse.matrix[0 * 3 + 0], inverse.matrix[0 * 3 + 1], inverse.matrix[0 * 3 + 2]);
    vector_3 row1 = vector_3(inverse.matrix[1 * 3 + 0], inverse.matrix[1 * 3 + 1], inverse.matrix[1 * 3 + 2]);
    vector_3 row2 = vector_3(inverse.matrix[2 * 3 + 0], inverse.matrix[2 * 3 + 1], inverse.matrix[2 * 3 + 2]);
    row0.normalize();
    row1.normalize();
    row2.normalize();
    inverse = matrix_3x3::create_from_rows(row0, row1, row2);
    return inverse;
#endif
  }

#ifdef __cplusplus
}
#endif


}



