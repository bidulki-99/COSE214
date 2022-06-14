#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "node.h"
#include "heap.h"

// 허프만 트리를 순회하며 허프만 코드를 생성하여 codes에 저장
// leaf 노드에서만 코드를 생성
// strdup 함수 사용
// make_huffman_code 함수에서 호출
static void traverse_tree( tNode *root, char *code, int depth, char *codes[]);

// 새로운 노드를 생성
// 좌/우 subtree가 NULL(0)이고 문자(data)와 빈도값(freq)이 저장됨
// make_huffman_tree 함수에서 호출
// return value : 노드의 포인터
static tNode *newNode(unsigned char data, int freq);

////////////////////////////////////////////////////////////////////////////////
// 허프만 코드를 화면에 출력
void print_huffman_code( char *codes[])
{
	int i;
	
	for (i = 0; i < 256; i++)
	{
		printf( "%d\t%s\n", i, codes[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
// 허프만 트리로부터 허프만 코드를 생성
// traverse_tree 함수 호출
void make_huffman_code( tNode *root, char *codes[])
{
	char code[256];
	
	traverse_tree( root, code, 0, codes);
}

////////////////////////////////////////////////////////////////////////////////
// 파일에 속한 각 문자(바이트)의 빈도 저장
// return value : 파일에서 읽은 바이트 수
int read_chars( FILE *fp, int ch_freq[]) {
	int letter = 0;
	int byte = 0;

	while ((letter = fgetc(fp)) != EOF) {
		ch_freq[letter]++;
		byte++;

		if (feof(fp))
			break;
	}
	
	return byte;
}

////////////////////////////////////////////////////////////////////////////////
// 파일로부터 문자별 빈도(256개)를 읽어서 ch_freq에 저장
void get_char_freq( FILE *fp, int ch_freq[]) {
	fread(ch_freq, sizeof(int), 256, fp);
}

////////////////////////////////////////////////////////////////////////////////
// 허프만 코드에 대한 메모리 해제
void free_huffman_code( char *codes[]) {
	for (int i = 0; i < 256; i++)
		free(codes[i]);
}

////////////////////////////////////////////////////////////////////////////////
// 허프만 트리를 순회하며 허프만 코드를 생성하여 codes에 저장
// leaf 노드에서만 코드를 생성
// strdup 함수 사용
// make_huffman_code 함수에서 호출
static void traverse_tree( tNode *root, char *code, int depth, char *codes[]) {
	if (root->left == NULL && root->right == NULL) {
		code[depth] = 0;
		codes[root->data] = strdup(code);
		return;
	}

	code[depth] = '0';
	traverse_tree(root->left, code, depth + 1, codes);

	code[depth] = '1';
	traverse_tree(root->right, code, depth + 1, codes);
}

////////////////////////////////////////////////////////////////////////////////
// 새로운 노드를 생성
// 좌/우 subtree가 NULL(0)이고 문자(data)와 빈도값(freq)이 저장됨
// make_huffman_tree 함수에서 호출
// return value : 노드의 포인터
static tNode *newNode(unsigned char data, int freq) {
	tNode *node = (tNode*)malloc(sizeof(tNode));
	node->data = data;
	node->freq = freq;
	node->left = node->right = NULL;

	return node;
}

////////////////////////////////////////////////////////////////////////////////
// 허프만 트리를 생성
// 1. capacity 256 짜리 빈(empty) 힙 생성 // HEAP *heap = heapCreate( 256);
// 2. 개별 알파벳에 대한 노드 생성
// 3. 힙에 삽입 (minheap 구성) // heapInsert( heap, node);
// 4. 2개의 최소값을 갖는 트리 추출 // tNode *left = heapDelete( heap); tNode *right = heapDelete( heap);
// 5. 두 트리를 결합 후 새 노드에 추가
// 6. 새 트리를 힙에 삽입
// 7. 힙에 한개의 노드가 남을 때까지 반복
// return value: 트리의 root 노드의 포인터
tNode *make_huffman_tree( int ch_freq[]){
	HEAP *heap = heapCreate(256);

	for (int i = 0; i < 256; i++)
		heapInsert(heap, newNode(i, ch_freq[i]));

	while (heap->last > 0) {
		tNode *left = heapDelete(heap);
		tNode *right = heapDelete(heap);

		tNode *insertNode = newNode(0, left->freq + right->freq);
		insertNode->left = left;
		insertNode->right = right;

		heapInsert(heap, insertNode);
	}

	tNode *root = heapDelete(heap);
	heapDestroy(heap);
	return root;
}

////////////////////////////////////////////////////////////////////////////////
// 허프만 트리 메모리 해제
void destroyTree( tNode *root) {
	if (root == NULL)
		return;

	destroyTree(root->left);
	destroyTree(root->right);
	free(root);
}

////////////////////////////////////////////////////////////////////////////////
// 입력 텍스트 파일(infp)을 허프만 코드를 이용하여 출력 파일(outfp)로 인코딩
// return value : 인코딩된 텍스트의 바이트 수 (파일 크기와는 다름)
int encoding( char *codes[], int ch_freq[], FILE *infp, FILE *outfp) {
	char code[256];
	char byte = 0;
	int letter;
	int count8 = 0;

	fwrite(ch_freq, sizeof(int), 256, outfp);

	while ((letter = fgetc(infp)) != EOF) {
		strcpy(code, codes[letter]);
		int len = strlen(code);

		for (int i = 0; i < len; i++) {
			byte <<= 1;
			byte |= code[i] - '0';
			count8++;

			if (count8 == 8) {
				fwrite(&byte, sizeof(char), 1, outfp);
				byte = 0;
				count8 = 0;
			}
		}

		if (feof(infp))
			break;
	}

	if (count8) {
		byte <<= 8 - count8;
		fwrite(&byte, sizeof(char), 1, outfp);
	}

	fseek(outfp, 0, SEEK_END);
	int size = ftell(outfp) - sizeof(int) * 256;
	fwrite(&size, sizeof(int), 1, outfp);

	return size;
}

////////////////////////////////////////////////////////////////////////////////
// 입력 파일(infp)을 허프만 트리를 이용하여 텍스트 파일(outfp)로 디코딩
void decoding( tNode *root, FILE *infp, FILE *outfp) {
	fseek(infp, -4, SEEK_END);

	int size;
	fread(&size, sizeof(int), 1, infp);

	fseek(infp, sizeof(int) * 256, SEEK_SET);

	tNode *temp = root;
	int curr = 0;
	int bit = 0;
	char bits;

	while (bit < size * 8) {
		fread(&bits, sizeof(char), 1, infp);

		for (int i = 0; i < 8; i++) {
			curr = bits & (1 << (7 - i));
			bit++;

			if (!curr)
				temp = temp->left;
			else
				temp = temp->right;

			if (temp->left == NULL && temp->right == NULL) {
				fprintf(outfp, "%c", temp->data);
				temp = root;
			}

			if (bit == size * 8)
				break;
		}
	}
}