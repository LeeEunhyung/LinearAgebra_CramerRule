#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

typedef struct matrix {
	char name; //이름
	int** elements; //행렬 배열
	int row; //행
	int column; //열
}matrix; //coefficient 구조체

typedef struct vector {
	int** elements; //백터 행렬 배열
	int row; //행
	int column; //열
}vector; //vector b 구조체

FILE* file_pointer; //입력 파일 포인터
matrix** M; //파일에서 불러온 행렬 구조체
vector b; //파일에서 불러온 vector 행렬 구조체
int linear_num;
float* dets; //행렬들의 det값을 저장하는 실수 배열
int* matrix_num; //각 coefficient 행렬의 크기를 저장하는 정수 배열
char str[60]; //연산 문자열
char** divide_str; //'+', '-'에 따라 연산 문자열을 나눠 저장하는 문자형 이차원 배열
int cnt; //'+', '-' 개수를 나타내는 정수
int flag; //연산 가능 불가능 여부 판단 정수

int getMinimumDet(matrix smallMatrix); //2x2 행렬의 det값을 계산하는 함수
int calculateDet(matrix major_matrix, int size); //전체 행렬의 det값을 cofactor를 통해 계산하는 함수

void setSubMatrix(int cnt); //Cramer's rule을 위한 행렬을 만드는 함수

void printMatrix(matrix M); //행렬을 출력하는 함수
void printVectorMatrix(vector b); //벡터 행렬을 출력하는 함수

void fetchMatrixAndVector(); //파일 패치 함수
void searchFile(); //입력한 파일이 있는지 여부를 확인, 동작하는 함수

void main() {
	int i;
	int cnt;

	printf("linear system의 개수를 입력해주세요: ");
	scanf("%d", &linear_num);

	M = (matrix * *)malloc(sizeof(matrix*) * linear_num);

	matrix_num = (int*)malloc(sizeof(int) * linear_num);
	for (i = 0; i < linear_num; i++) {
		matrix_num[i] = 0;
	}

	searchFile();

	for (cnt = 0; cnt < linear_num; cnt++) {
		dets = (float*)malloc(sizeof(float) * matrix_num[cnt]);
		if (M[cnt][0].row == M[cnt][0].column) {
			for (i = 0; i < matrix_num[cnt]; i++) {
				dets[i] = (float)calculateDet(M[cnt][i], M[cnt][i].row); //det 계산
			}
		}

		//solution 출력
		printf("%d번 solution: ", cnt + 1);
		if (M[cnt][0].row == M[cnt][0].column) {
			if (dets[0] == 0) { //coefficient 행렬의 det가 0일 경우
				printf("The solution is not unique.\n");
			}
			else {
				for (i = 1; i < matrix_num[cnt]; i++) {
					printf("x%d = %.2f      ", i, (float)(dets[i] / dets[0]));
				}
				printf("\n");
			}
		} 
		else {
				printf("Not solvable using Cramer's rule.\n");
		}
	}

	return;
}

int getMinimumDet(matrix smallMatrix) {
	int det;

	det = smallMatrix.elements[0][0] * smallMatrix.elements[1][1] - smallMatrix.elements[0][1] * smallMatrix.elements[1][0];

	return det;
}
int calculateDet(matrix major_matrix, int size) {
	matrix* minor_matrix;
	int* det; //cofactor
	int D = 0; //전체 det 값
	int i, j, k;

	if (size == 1) { //자기 자신이 det
		return major_matrix.elements[0][0];
	}
	else if (size == 2) {
		return getMinimumDet(major_matrix);
	}

	minor_matrix = (matrix*)malloc(sizeof(matrix) * size);
	det = (int*)malloc(sizeof(int) * size);

	for (i = 0; i < size; i++) { //minor_matrix의 크기 설정
		minor_matrix[i].elements = (int**)malloc(sizeof(int*) * (size - 1)); //row
		for (j = 0; j < size - 1; j++) {
			minor_matrix[i].elements[j] = (int*)malloc(sizeof(int) * (size - 1)); //column
		}
	}

	for (i = 0; i < size; i++) {
		for (j = 0; j < size - 1; j++) {
			for (k = 0; k < size - 1; k++) {
				if (i == 0) {
					minor_matrix[i].elements[j][k] = major_matrix.elements[j + 1][k + 1];
				}
				else if (i > 0) {
					if (i > k) {
						minor_matrix[i].elements[j][k] = major_matrix.elements[j + 1][k];
					}
					else if (i <= k) {
						minor_matrix[i].elements[j][k] = major_matrix.elements[j + 1][k + 1];
					}
				}
			}
		}
	}

	for (i = 0; i < size; i++) { //minor_matrix의 det값 계산
		det[i] = calculateDet(minor_matrix[i], size - 1);
	}

	for (i = 0; i < size; i++) { //부호 설정해서 전체 det값 계산
		if (i % 2 == 0) {
			D += det[i] * major_matrix.elements[0][i];
		}
		else {
			D -= det[i] * major_matrix.elements[0][i];
		}
	}

	return D;
}

void setSubMatrix(int cnt) {
	int i, j;

	for (i = 1; i < matrix_num[cnt]; i++) {
		for (j = 0; j < M[cnt][0].row; j++) {
			M[cnt][i].elements[j][i - 1] = b.elements[j][0];
		}
	}

	return;
}

void printMatrix(matrix M) {
	int i, j;

	for (i = 0; i < M.row; i++) {
		for (j = 0; j < M.column; j++) {
			printf("%d ", M.elements[i][j]);
		}
		printf("\n");
	}

	return;
}
void printVectorMatrix(vector b) {
	int i, j;

	for (i = 0; i < b.row; i++) {
		for (j = 0; j < b.column; j++) {
			printf("%d ", b.elements[i][j]);
		}
		printf("\n");
	}

	return;
}

void fetchMatrixAndVector() {
	char name;
	int row, column;
	int i, j, k;
	int cnt;
	int tmp;
	char nothing;
	
	for (cnt = 0; cnt < linear_num; cnt++) {
		fscanf(file_pointer, "%c", &name);
		fscanf(file_pointer, "%c", &nothing);
		fscanf(file_pointer, "%d", &row);
		fscanf(file_pointer, "%c", &nothing);
		fscanf(file_pointer, "%d", &column);

		matrix_num[cnt] = row + 1;

		//coefficient matrix 설정
		M[cnt] = (matrix*)malloc(sizeof(matrix) * matrix_num[cnt]);
		for (i = 0; i < matrix_num[cnt]; i++) {
			M[cnt][i].name = name;
			M[cnt][i].row = row;
			M[cnt][i].column = column;

			M[cnt][i].elements = (int**)malloc(sizeof(int*) * M[cnt][i].row);
			for (j = 0; j < M[cnt][i].row; j++) {
				M[cnt][i].elements[j] = (int*)malloc(sizeof(int) * M[cnt][i].column);
			}
		}

		//vector matrix 설정
		b.row = row;
		b.column = 1;
		b.elements = (int**)malloc(sizeof(int*) * b.row);
		for (i = 0; i < b.row; i++) {
			b.elements[i] = (int*)malloc(sizeof(int) * b.column);
		}

		//coefficient matrix, vector matrix 값 읽어오기
		for (i = 0; i < M[cnt][0].row; i++) {
			for (j = 0; j < M[cnt][0].column; j++) {
				fscanf(file_pointer, "%d", &tmp);
				M[cnt][0].elements[i][j] = tmp;
				fscanf(file_pointer, "%c", &nothing);
			}
			fscanf(file_pointer, "%d", &tmp);
			b.elements[i][0] = tmp;
		}

		for (i = 1; i < matrix_num[cnt]; i++) {
			for (j = 0; j < M[cnt][i].row; j++) {
				for (k = 0; k < M[cnt][i].column; k++) {
					M[cnt][i].elements[j][k] = M[cnt][0].elements[j][k];
				}
			}
		}
		
		setSubMatrix(cnt);
		printf("%d번 coefficient 행렬 = \n", cnt + 1);
		printMatrix(M[cnt][0]);
		printf("\n");
		
		printf("%d번 vector 행렬 = \n", cnt + 1);
		printVectorMatrix(b);
		printf("--------------\n");
	}
	return;
}
void searchFile() {
	char file_root[10] = "\0";
	char file_path[100] = "\0";
	char file_name[20] = "\0";
	int result;

	strcat(file_root, "./");

	while (1) {
		printf("세 개의 linear system을 가지는 파일 이름을 입력하세요(확장자명 제외): ");
		scanf("%s", file_name);

		strcat(file_name, ".txt");
		strcat(file_path, file_root);
		strcat(file_path, file_name);

		result = access(file_path, 0);

		if (result == 0) {
			printf("해당 파일이 존재하므로 행렬을 불러옵니다.\n");
			break;
		}
		else {
			printf("해당 파일이 존재하지 않습니다.\n");
			file_path[0] = '\0';
		}
	}

	file_pointer = fopen(file_path, "r");
	fclose(file_pointer);

	file_pointer = fopen(file_path, "r");
	fetchMatrixAndVector();
	fclose(file_pointer);

	return;
}
