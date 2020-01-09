#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

typedef struct matrix {
	char name; //�̸�
	int** elements; //��� �迭
	int row; //��
	int column; //��
}matrix; //coefficient ����ü

typedef struct vector {
	int** elements; //���� ��� �迭
	int row; //��
	int column; //��
}vector; //vector b ����ü

FILE* file_pointer; //�Է� ���� ������
matrix** M; //���Ͽ��� �ҷ��� ��� ����ü
vector b; //���Ͽ��� �ҷ��� vector ��� ����ü
int linear_num;
float* dets; //��ĵ��� det���� �����ϴ� �Ǽ� �迭
int* matrix_num; //�� coefficient ����� ũ�⸦ �����ϴ� ���� �迭
char str[60]; //���� ���ڿ�
char** divide_str; //'+', '-'�� ���� ���� ���ڿ��� ���� �����ϴ� ������ ������ �迭
int cnt; //'+', '-' ������ ��Ÿ���� ����
int flag; //���� ���� �Ұ��� ���� �Ǵ� ����

int getMinimumDet(matrix smallMatrix); //2x2 ����� det���� ����ϴ� �Լ�
int calculateDet(matrix major_matrix, int size); //��ü ����� det���� cofactor�� ���� ����ϴ� �Լ�

void setSubMatrix(int cnt); //Cramer's rule�� ���� ����� ����� �Լ�

void printMatrix(matrix M); //����� ����ϴ� �Լ�
void printVectorMatrix(vector b); //���� ����� ����ϴ� �Լ�

void fetchMatrixAndVector(); //���� ��ġ �Լ�
void searchFile(); //�Է��� ������ �ִ��� ���θ� Ȯ��, �����ϴ� �Լ�

void main() {
	int i;
	int cnt;

	printf("linear system�� ������ �Է����ּ���: ");
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
				dets[i] = (float)calculateDet(M[cnt][i], M[cnt][i].row); //det ���
			}
		}

		//solution ���
		printf("%d�� solution: ", cnt + 1);
		if (M[cnt][0].row == M[cnt][0].column) {
			if (dets[0] == 0) { //coefficient ����� det�� 0�� ���
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
	int D = 0; //��ü det ��
	int i, j, k;

	if (size == 1) { //�ڱ� �ڽ��� det
		return major_matrix.elements[0][0];
	}
	else if (size == 2) {
		return getMinimumDet(major_matrix);
	}

	minor_matrix = (matrix*)malloc(sizeof(matrix) * size);
	det = (int*)malloc(sizeof(int) * size);

	for (i = 0; i < size; i++) { //minor_matrix�� ũ�� ����
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

	for (i = 0; i < size; i++) { //minor_matrix�� det�� ���
		det[i] = calculateDet(minor_matrix[i], size - 1);
	}

	for (i = 0; i < size; i++) { //��ȣ �����ؼ� ��ü det�� ���
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

		//coefficient matrix ����
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

		//vector matrix ����
		b.row = row;
		b.column = 1;
		b.elements = (int**)malloc(sizeof(int*) * b.row);
		for (i = 0; i < b.row; i++) {
			b.elements[i] = (int*)malloc(sizeof(int) * b.column);
		}

		//coefficient matrix, vector matrix �� �о����
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
		printf("%d�� coefficient ��� = \n", cnt + 1);
		printMatrix(M[cnt][0]);
		printf("\n");
		
		printf("%d�� vector ��� = \n", cnt + 1);
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
		printf("�� ���� linear system�� ������ ���� �̸��� �Է��ϼ���(Ȯ���ڸ� ����): ");
		scanf("%s", file_name);

		strcat(file_name, ".txt");
		strcat(file_path, file_root);
		strcat(file_path, file_name);

		result = access(file_path, 0);

		if (result == 0) {
			printf("�ش� ������ �����ϹǷ� ����� �ҷ��ɴϴ�.\n");
			break;
		}
		else {
			printf("�ش� ������ �������� �ʽ��ϴ�.\n");
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
