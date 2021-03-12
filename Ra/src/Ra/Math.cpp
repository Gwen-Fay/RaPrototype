/*
 * Matrix Math Functions
 */

#include "Math.h"

#include <cmath>


    void getRotMat3(float(&mat)[3][3], float pitch, float yaw, float roll)
    {
        float pi = atan(1) * 4;
        float sp = sin((pitch * (pi / 180.0))), cp = cos((pitch * (pi / 180.0)));
        float sy = sin((yaw * (pi / 180.0))), cy = cos((yaw * (pi / 180.0)));
        float sr = sin((roll * (pi / 180.0))), cr = cos((roll * (pi / 180.0)));

        mat[0][0] = cp * cy;  mat[0][1] = cp * sy * sr - sp * cr;   mat[0][2] = cp * sy * cr + sp * sr;
        mat[1][0] = sp * cy;    mat[1][1] = sp * sy * sr + cp * cr;         mat[1][2] = sp * sy * cr - cp * sr;
        mat[2][0] = -sy;      mat[2][1] = cy * sr;                    mat[2][2] = cy * cr;
    }

    void mat4Mult(float(&leftMat)[4][4], float(&rightMat)[4][4], float(&retMat)[4][4])
    {
        float lMat[4][4];
        float rMat[4][4];
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                lMat[i][j] = leftMat[i][j];
                rMat[i][j] = rightMat[i][j];
            }
        }
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                retMat[i][j] = lMat[i][0] * rMat[0][j] +
                    lMat[i][1] * rMat[1][j] +
                    lMat[i][2] * rMat[2][j] +
                    lMat[i][3] * rMat[3][j];
            }
        }
    }

    void genTrans(float(&mat)[4][4], const EntitySDF* const entity)
    {
        // inverse rotate matrix
        float pi = atan(1) * 4;
        float sp = sin((-entity->pitch * (pi / 180.0))), cp = cos((-entity->pitch * (pi / 180.0)));
        float sy = sin((-entity->yaw * (pi / 180.0))), cy = cos((-entity->yaw * (pi / 180.0)));
        float sr = sin((-entity->roll * (pi / 180.0))), cr = cos((-entity->roll * (pi / 180.0)));



        mat[0][0] = cp * cy;    mat[0][1] = cp * sy * sr - sp * cr;   mat[0][2] = cp * sy * cr + sp * sr;
        mat[1][0] = sp * cy;    mat[1][1] = sp * sy * sr + cp * cr;   mat[1][2] = sp * sy * cr - cp * sr;
        mat[2][0] = -sy;        mat[2][1] = cy * sr;                   mat[2][2] = cy * cr;

        mat[3][0] = 0;
        mat[3][1] = 0;
        mat[3][2] = 0;

        mat[0][3] = 0;
        mat[1][3] = 0;
        mat[2][3] = 0;
        mat[3][3] = 1;

        float scaleMat[4][4] = {
            1.0 / entity->scaleX, 0 ,0 ,0,
            0, 1.0 / entity->scaleY, 0, 0,
            0, 0, 1 / entity->scaleZ, 0,
            0, 0, 0, 1
        };

        float posMat[4][4] = {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            -entity->x, -entity->y, -entity->z, 1
        };


        mat4Mult(posMat, mat, mat);
        mat4Mult(mat, scaleMat, mat);

    }

    void genPosRot(float(&mat)[4][4], const EntitySDF* const entity)
    {
        // inverse rotate matrix
        float pi = atan(1) * 4;
        float sp = sin((-entity->pitch * (pi / 180.0))), cp = cos((-entity->pitch * (pi / 180.0)));
        float sy = sin((-entity->yaw * (pi / 180.0))), cy = cos((-entity->yaw * (pi / 180.0)));
        float sr = sin((-entity->roll * (pi / 180.0))), cr = cos((-entity->roll * (pi / 180.0)));



        mat[0][0] = cp * cy;    mat[0][1] = cp * sy * sr - sp * cr;   mat[0][2] = cp * sy * cr + sp * sr;
        mat[1][0] = sp * cy;    mat[1][1] = sp * sy * sr + cp * cr;   mat[1][2] = sp * sy * cr - cp * sr;
        mat[2][0] = -sy;        mat[2][1] = cy * sr;                   mat[2][2] = cy * cr;

        mat[3][0] = 0;
        mat[3][1] = 0;
        mat[3][2] = 0;

        mat[0][3] = 0;
        mat[1][3] = 0;
        mat[2][3] = 0;
        mat[3][3] = 1;

        float posMat[4][4] = {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            -entity->x, -entity->y, -entity->z, 1
        };
        mat4Mult(posMat, mat, mat);
    }
